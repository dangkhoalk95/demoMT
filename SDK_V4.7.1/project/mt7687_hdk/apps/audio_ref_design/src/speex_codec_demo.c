/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <speex/speex.h>
#include "FreeRTOS.h"
#include "task.h"
#include "speex_codec_demo.h"
#include "hal_log.h"
#include "ff.h"

#if defined SPEEX_CODEC_LOG_ENABLE
log_create_module(SPEEX_CODEC_DEMO, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(SPEEX_CODEC, "[SPEEX]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(SPEEX_CODEC, "[SPEEX]: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(SPEEX_CODEC, "[SPEEX]: "fmt,##arg)
#else
#define LOGE(fmt,arg...)
#define LOGW(fmt,arg...)
#define LOGI(fmt,arg...)
#endif

unsigned int frame_size=0;

int speex_en_demo(void)
{
    LOGI("speex_en_test\n\r");

   FIL fin;
   FIL fout;
   FATFS fatfs;
   FRESULT res;
   UINT length_read, length_written;
    
   short *in;
   float *input;
   
   char cbits[200];
   int nbBytes;
   /*Holds the state of the encoder*/
   void *state=NULL;
   /*Holds bits so they can be read and written to by the Speex routines*/
   SpeexBits bits;
   int i, tmp;
   int codec_satus=-1;

    /* Mount the partition before it is accessed, and need only do it once. */
    res = f_mount(&fatfs, _T("0:"), 1);
    if (FR_OK != res) {
        LOGE("Failed to mount the partition!\n\r");
        return -1;
    }
 
    /* Step1: Create a new file for writing and reading. */
    res = f_open(&fout, _T("SD:/speex/nb_female.en"), FA_CREATE_ALWAYS);
    if (FR_OK != res) {
        LOGE("Failed to create the fout!\n\r");
        return -1;
    }
    
    res = f_open(&fout, _T("SD:/speex/nb_female.en"), FA_OPEN_EXISTING | FA_WRITE | FA_READ);
    if (FR_OK != res) {
        LOGE("Failed to open the fout!\n\r");
        return -1;
    }

    res = f_open(&fin, _T("SD:/speex/nb_female.wav"), FA_OPEN_EXISTING | FA_WRITE | FA_READ);
    if (FR_OK != res) {
        LOGE("Failed to open the fin!\n\r");
        return -1;
    }
    
   /*Create a new encoder state in wideband mode*/
   state = speex_encoder_init(&speex_nb_mode);
   //state = speex_encoder_init(&speex_wb_mode);
   if(state==NULL){
        LOGE("speex_encoder_init failed!\n\r");
        return -1;
   }

   speex_encoder_ctl(state,SPEEX_GET_FRAME_SIZE,&frame_size);
   LOGI("frame_size for encode=%d!\n\r",frame_size);
    

   in = pvPortMalloc(frame_size * sizeof(short *));
   input = pvPortMalloc(frame_size * sizeof(float *));
   
   /*Set the quality to 8 (15 kbps)*/
   tmp=8;
   codec_satus=speex_encoder_ctl(state, SPEEX_SET_QUALITY, &tmp);
   if(codec_satus != 0){
        LOGE("speex_encoder_ctl failed!\n\r");
        return -1;
   }
   
   /*Initialization of the structure that holds the bits*/
   speex_bits_init(&bits);

   
   while (1)
   {
 
      /*Read a 16 bits/sample audio frame ,2*FRAME_SIZE bytes*/
      res=f_read(&fin, in, frame_size*2, &length_read);
      if (FR_OK != res) {
        LOGE("Failed to read the file!\n\r");
        res = f_close(&fin);
        return -1;
      }
      else 
        LOGI("read len from fin=%d ", length_read);
      
      if (f_eof(&fin)){
         LOGI("fin end, stop encode");
         break;
      }
    
      /*Copy the 16 bits values to float so Speex can work on them*/
      for (i=0;i<frame_size;i++){
         input[i]=(float)in[i];
         //log_hal_info("i=%d input=%f in=%d\n\r",i,input[i],in[i]);
      }

      /*Flush all the bits in the struct so we can encode a new frame*/
      speex_bits_reset(&bits);

      /*Encode the frame*/
      speex_encode(state, input, &bits);
      /*Copy the bits to an array of char that can be written*/
      nbBytes = speex_bits_write(&bits, cbits, 200);

      /*Write the size of the frame first. This is what sampledec expects but
            it's likely to be different in your own application*/
      res = f_write(&fout, &nbBytes, sizeof(int), &length_written);
      if (FR_OK != res) {
        LOGE("Failed to write the size into file!\n\r");
        res = f_close(&fout);
        return -1;
      }
      else  
        LOGI("write the size=%d ", nbBytes);   
      
      /*Write the compressed data*/
      res =f_write(&fout, cbits, nbBytes, &length_written);
      if (FR_OK != res) {
        LOGE("Failed to write the compressed data into file!\n\r");
        res = f_close(&fout);
        return -1;
      }
      else      
        LOGI("write the compressed data len=%d ", length_written);
     
   }
   
   /*Destroy the encoder state*/
   speex_encoder_destroy(state);
   /*Destroy the bit-packing struct*/
   speex_bits_destroy(&bits);

   vPortFree(in);
   vPortFree(input);

    LOGI("prepare to fclose");
    /* Close the open file object, flush the cached information of the file back to the volume.*/

    res = f_close(&fout);
    if (FR_OK != res) {
        LOGE("Failed to close the fout!\n\r");
        return -1;
    }   
    
    res = f_close(&fin);
    if (FR_OK != res) {
        LOGE("Failed to close the fin!\n\r");
        return -1;
    }  
   return 0;
}

int speex_dec_demo(void)
{
   LOGI("speex_dec_test\n\r");
    
   FIL fin;
   FIL fout;
   FATFS fatfs;
   FRESULT res;
   UINT length_read, length_written;

   /*Holds the audio that will be written to file (16 bits per sample)*/
   short *out;
   /*Speex handle samples as float, so we need an array of floats*/
   float *output;
   char cbits[200];
   int nbBytes;
   /*Holds the state of the decoder*/
   void *state;
   /*Holds bits so they can be read and written to by the Speex routines*/
   SpeexBits bits;
   int i, tmp;

   /*Create a new decoder state in narrowband mode*/
   state = speex_decoder_init(&speex_nb_mode);
   //state = speex_decoder_init(&speex_wb_mode);
   if(state==NULL){
        LOGE("speex_decoder_init failed!\n\r");
        return -1;
   }

   speex_decoder_ctl(state, SPEEX_GET_FRAME_SIZE, &frame_size); 
   LOGI("frame_size for deocde=%d!\n\r",frame_size);

   out = pvPortMalloc(frame_size * sizeof(short *));
   output = pvPortMalloc(frame_size * sizeof(float *));

   /*Set the perceptual enhancement on*/
   tmp=1;
   speex_decoder_ctl(state, SPEEX_SET_ENH, &tmp);

    /* Mount the partition before it is accessed, and need only do it once. */
    res = f_mount(&fatfs, _T("0:"), 1);
    if (FR_OK != res) {
        LOGE("Failed to mount the partition!\n\r");
        return -1;
    }
    
    /* Step1: Create a new file for writing and reading. */
    res = f_open(&fout, _T("SD:/speex/nb_female_de.wav"), FA_CREATE_ALWAYS);
    if (FR_OK != res) {
        LOGE("Failed to create the fout!\n\r");
        return -1;
    }
    
    res = f_open(&fout, _T("SD:/speex/nb_female_de.wav"), FA_OPEN_EXISTING | FA_WRITE | FA_READ);
    if (FR_OK != res) {
        LOGE("Failed to open the fout!\n\r");
        return -1;
    }
    
    res = f_open(&fin, _T("SD:/speex/nb_female.en"), FA_OPEN_EXISTING | FA_WRITE | FA_READ);
    if (FR_OK != res) {
        LOGE("Failed to open the fin!\n\r");
        return -1;
    }

   /*Initialization of the structure that holds the bits*/
   speex_bits_init(&bits);
   while (1)
   {
      /*Read the size encoded by sampleenc, this part will likely be 
        different in your application*/
      res=f_read(&fin, &nbBytes, sizeof(int), &length_read);
      if (FR_OK != res) {
        LOGE("Failed to read nbBytes!\n\r");
        res = f_close(&fin);
        return -1;
      }
      else       
        LOGI("read len=%d , nbBytes=%d", length_read, nbBytes);

      if (f_eof(&fin)){
         LOGI("fin end, stop encode");
         break;
      }
      
      /*Read the "packet" encoded by sampleenc*/
      f_read(&fin, cbits, nbBytes, &length_read);
      if (FR_OK != res) {
        LOGE("Failed to read packet!\n\r");
        res = f_close(&fin);
        return -1;
      }
      else       
        LOGI("read len2=%d ", length_read);
      
      if (f_eof(&fin)){
         LOGI("fin2 end, stop encode");
         break;
      }
      
      /*Copy the data into the bit-stream struct*/
      speex_bits_read_from(&bits, cbits, nbBytes);

      /*Decode the data*/
      speex_decode(state, &bits, output);

      /*Copy from float to short (16 bits) for output*/
      for (i=0;i<frame_size;i++){
         out[i]=output[i];
         //LOGI("out[%d]=%x",i,out[i]);
      }
      
      /*Write the decoded audio to file*/
      f_write(&fout, out, frame_size*2, &length_written);
      //LOGI("write len=%d ", length_written);
   }
   
   /*Destroy the decoder state*/
   speex_decoder_destroy(state);
   /*Destroy the bit-stream truct*/
   speex_bits_destroy(&bits);

   vPortFree(out);
   vPortFree(output);

   LOGI("prepare to fclose");
    /* Close the open file object, flush the cached information of the file back to the volume.*/
    res = f_close(&fout);
    if (FR_OK != res) {
        LOGE("Failed to close the fout!\n\r");
        return -1;
    }   
    
    res = f_close(&fin);
    if (FR_OK != res) {
        LOGE("Failed to close the fin!\n\r");
        return -1;
    } 
    
   return 0;
}
