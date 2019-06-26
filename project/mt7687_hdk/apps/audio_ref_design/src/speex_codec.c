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
 

#include "speex_codec.h"
#include "speex_codec_internal.h"
#include "task_def.h"

#ifdef HAL_I2S_MODULE_ENABLED
#include "hal_i2s.h"
#include "nau8810.h"
#include "hal_i2c_master.h"
#else
#error "HAL_I2S_MODULE_ENABLED no defined !!"
#endif/*HAL_I2S_MODULE_ENABLED*/


#if defined SPEEX_CODEC_LOG_ENABLE
log_create_module(SPEEX_CODEC, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(SPEEX_CODEC, "[SPEEX]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(SPEEX_CODEC, "[SPEEX]: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(SPEEX_CODEC, "[SPEEX]: "fmt,##arg)
#else
#define LOGE(fmt,arg...)
#define LOGW(fmt,arg...)
#define LOGI(fmt,arg...)
#endif

#define CBITS_SIZE            200

//#define I2S_TX_VFIFO_LENGTH   4096
#define I2S_TX_VFIFO_LENGTH   1600
#define I2S_RX_VFIFO_LENGTH   100

uint32_t I2S_TX_VFIFO[I2S_TX_VFIFO_LENGTH];
uint32_t I2S_RX_VFIFO[I2S_RX_VFIFO_LENGTH];


/*For keeping global settings*/
static speex_codec_internal_handle_t *speex_codec_internal_handle = NULL;

PRIVILEGED_DATA static QueueHandle_t speex_codec_queue_handle = NULL;
static uint8_t speex_codec_queue_reg_num = 0;
static speex_codec_queue_event_id_t speex_codec_queue_event_id_array[MAX_SPEEX_CODEC_FUNCTIONS];
static speex_codec_internal_callback_t speex_codec_queue_handler[MAX_SPEEX_CODEC_FUNCTIONS];

static void speex_codec_i2s_tx_disable(void);
static void speex_codec_i2s_tx_enable(void);
static void speex_codec_i2s_tx_callback(hal_i2s_event_t event, void *user_data);
static int8_t speex_codec_nau8810_configure(void);
static int8_t speex_codec_i2s_configure(void);
static AUCODEC_SAMPLERATE_SEL_e speex_codec_get_codec_sampling_rate(speex_codec_sample_rate_t sampling_rate);

static void speex_codec_cp16to8(uint8_t* dest, uint16_t* src, int32_t lenth_in_word);


/*ring buffer operation*/
static uint32_t ring_buffer_get_data_byte_count(ring_buffer_information_t *p_info)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t data_byte_count;
    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else {
        data_byte_count = (buffer_byte_count << 1) - read_pointer + write_pointer;
    }
    return data_byte_count;
}

static uint32_t ring_buffer_get_space_byte_count(ring_buffer_information_t *p_info)
{
    return p_info->buffer_byte_count - ring_buffer_get_data_byte_count(p_info);
}

static void ring_buffer_get_write_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t space_byte_count  = ring_buffer_get_space_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t tail_byte_count;
    if (write_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + write_pointer;
        tail_byte_count = buffer_byte_count - write_pointer;
    } else {
        *pp_buffer = buffer_pointer + write_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - write_pointer;
    }
    *p_byte_count = MINIMUM(space_byte_count, tail_byte_count);
    return;
}

static void ring_buffer_get_read_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t data_byte_count   = ring_buffer_get_data_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t tail_byte_count;
    if (read_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + read_pointer;
        tail_byte_count = buffer_byte_count - read_pointer;
    } else {
        *pp_buffer = buffer_pointer + read_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - read_pointer;
    }
    *p_byte_count = MINIMUM(data_byte_count, tail_byte_count);
    return;
}

static void ring_buffer_write_done(ring_buffer_information_t *p_info, uint32_t write_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t write_pointer     = p_info->write_pointer + write_byte_count;
    p_info->write_pointer = write_pointer >= buffer_end ? write_pointer - buffer_end : write_pointer;
    return;
}

static void ring_buffer_read_done(ring_buffer_information_t *p_info, uint32_t read_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t read_pointer      = p_info->read_pointer + read_byte_count;
    p_info->read_pointer = read_pointer >= buffer_end ? read_pointer - buffer_end : read_pointer;
    return;
}

static void speex_codec_set_share_buffer(speex_codec_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    handle->share_buffer.buffer_base = buffer;
    length &= ~0x1; // make buffer size even
    handle->share_buffer.buffer_size = length;
    handle->share_buffer.write = 0;
    handle->share_buffer.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void speex_codec_get_write_buffer(speex_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->share_buffer.read > handle->share_buffer.write) {
        count = handle->share_buffer.read - handle->share_buffer.write - 1;
    } else if (handle->share_buffer.read == 0) {
        count = handle->share_buffer.buffer_size - handle->share_buffer.write - 1;
    } else {
        count = handle->share_buffer.buffer_size - handle->share_buffer.write;
    }
    *buffer = handle->share_buffer.buffer_base + handle->share_buffer.write;
    *length = count;
}


static void speex_codec_get_read_buffer(speex_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->share_buffer.write >= handle->share_buffer.read) {
        count = handle->share_buffer.write - handle->share_buffer.read;
    } else {
        count = handle->share_buffer.buffer_size - handle->share_buffer.read;
    }
    *buffer = handle->share_buffer.buffer_base + handle->share_buffer.read;
    *length = count;
}


static void speex_codec_write_data_done(speex_codec_media_handle_t *handle, uint32_t length)
{
    handle->share_buffer.write += length;
    if (handle->share_buffer.write == handle->share_buffer.buffer_size) {
        handle->share_buffer.write = 0;
    }
}

static void speex_codec_finish_write_data(speex_codec_media_handle_t *handle)
{
    handle->waiting = false;
    handle->underflow = false;
}

static void speex_codec_reset_share_buffer(speex_codec_media_handle_t *handle)
{
    handle->share_buffer.write = 0;
    handle->share_buffer.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void speex_codec_read_data_done(speex_codec_media_handle_t *handle, uint32_t length)
{
    handle->share_buffer.read += length;
    if (handle->share_buffer.read == handle->share_buffer.buffer_size) {
        handle->share_buffer.read = 0;
    }
}


static int32_t speex_codec_get_free_space(speex_codec_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buffer.read - handle->share_buffer.write - 2;
    if (count < 0) {
        count += handle->share_buffer.buffer_size;
    }
    return count;
}

static int32_t speex_codec_get_data_count(speex_codec_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buffer.write - handle->share_buffer.read;
    if (count < 0) {
        count += handle->share_buffer.buffer_size;
    }
    return count;
}


static void speex_codec_reset_pcm_out_buffer(void)
{
    speex_codec_internal_handle_t *internal_handle = speex_codec_internal_handle;
    internal_handle->stream_out_pcm_buff.read_pointer = 0;
    internal_handle->stream_out_pcm_buff.write_pointer = 0;
}


static void speex_codec_buffer_function_init(speex_codec_media_handle_t *handle)
{
    handle->set_share_buffer       = speex_codec_set_share_buffer;
    handle->get_write_buffer       = speex_codec_get_write_buffer;
    handle->get_read_buffer        = speex_codec_get_read_buffer;
    handle->write_data_done        = speex_codec_write_data_done;
    handle->finish_write_data      = speex_codec_finish_write_data;
    handle->reset_bitstream_buffer = speex_codec_reset_share_buffer;
    handle->read_data_done         = speex_codec_read_data_done;
    handle->get_free_space         = speex_codec_get_free_space;
    handle->get_data_count         = speex_codec_get_data_count;
}


static void speex_codec_event_send_from_isr(speex_codec_queue_event_id_t id, void *parameter)
{
    //LOGI("[CTRL]SEND HISR event\n");
    speex_codec_queue_event_t event;
    event.id        = id;
    event.parameter = parameter;
    if (xQueueSendFromISR(speex_codec_queue_handle, &event, 0) != pdPASS) {
        LOGI("queue not pass %d\r\n", id);
        return;
    }

    return;
}


static void speex_codec_event_register_callback(speex_codec_queue_event_id_t reg_id, speex_codec_internal_callback_t callback)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_SPEEX_CODEC_FUNCTIONS; id_idx++) {
        if (speex_codec_queue_event_id_array[id_idx] == SPEEX_CODEC_QUEUE_EVENT_NONE) {
            speex_codec_queue_event_id_array[id_idx] = reg_id;
            speex_codec_queue_handler[id_idx] = callback;
            speex_codec_queue_reg_num++;
            break;
        }
    }
    return;
}


static void speex_codec_event_deregister_callback(speex_codec_queue_event_id_t dereg_id)
{
    LOGI("[CTRL]deregister HISR callback \n");

    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_SPEEX_CODEC_FUNCTIONS; id_idx++) {
        if (speex_codec_queue_event_id_array[id_idx] == dereg_id) {
            speex_codec_queue_event_id_array[id_idx] = SPEEX_CODEC_QUEUE_EVENT_NONE;
            speex_codec_queue_reg_num--;
            break;
        }
    }
    return;
}


static void speex_codec_task_main(void *arg)
{
    speex_codec_queue_event_t event;
    speex_codec_queue_handle = xQueueCreate(SPEEX_CODEC_QUEUE_SIZE, sizeof(speex_codec_queue_event_t));
    /* Initialize queue registration */
    uint8_t id_idx;
    for (id_idx = 0; id_idx < MAX_SPEEX_CODEC_FUNCTIONS; id_idx++) {
        speex_codec_queue_event_id_array[id_idx] = SPEEX_CODEC_QUEUE_EVENT_NONE;
    }

    while (1) {
        if (xQueueReceive(speex_codec_queue_handle, &event, portMAX_DELAY)) {
            speex_codec_queue_event_id_t rece_id = event.id;
            //LOGI("rece_id=%d\n",rece_id);
            uint8_t id_idx;
            for (id_idx = 0; id_idx < MAX_SPEEX_CODEC_FUNCTIONS; id_idx++) {
                if (speex_codec_queue_event_id_array[id_idx] == rece_id) {
                    //LOGI("find queue event id\n");
                    speex_codec_queue_handler[id_idx](event.parameter);
                    break;
                }
            }
        }
    }

}

static void speex_codec_deocde_hisr_handler(void *data)
{
    //LOGI("[CTRL]deocde ISR handler start \n");
    speex_codec_internal_handle_t *internal_handle = speex_codec_internal_handle; 
    speex_codec_media_handle_t *handle=(speex_codec_media_handle_t *)internal_handle;
    uint8_t data_buff[CBITS_SIZE];
    uint8_t *share_buff_read_ptr = NULL;
    uint32_t share_buff_read_len=0;
    uint32_t data_buff_index=0;
    uint32_t remain=0;
    uint32_t consume=0;
    uint32_t nbBytes=0;
    uint32_t i=0;
    uint32_t loop_idx = 0;
    int8_t ret=0;

     do{
         /*Read the size encoded by the encoder(speex_codec_demo.c), this part will likely be 
               different in your application*/
         remain=4;
         data_buff_index=0;
         memset(data_buff, 0, CBITS_SIZE); 
         for (loop_idx = 0; loop_idx < 4; loop_idx++) {
             if(remain==0)
              break;
             share_buff_read_ptr=NULL; 
             share_buff_read_len=0;
             handle->get_read_buffer(handle, &share_buff_read_ptr, &share_buff_read_len);
             if(share_buff_read_len>0){
                 //LOGI(" loop_idx=%d share_buff_read_len=%d\n",loop_idx,share_buff_read_len);
                 consume=MINIMUM(share_buff_read_len,remain); 
                 //LOGI(" consume=%d \n",consume);
                 remain-=consume;
                 memcpy(data_buff+data_buff_index, share_buff_read_ptr, consume);
                 //if(consume>=1)
                  data_buff_index+=consume;
                 handle->read_data_done(handle, consume);
            }
         }
         memcpy(&nbBytes, data_buff, 4);
         LOGI("nbBytes from share buffer=%d\n",(int)nbBytes);
         
          /*check the nbBytes*/
         if((nbBytes > CBITS_SIZE) || (nbBytes==0)){
          LOGW("WARNING! nbBytes=%d , CBITS_SIZE=%d", (int)nbBytes, CBITS_SIZE);
          break;
         }
         
         /*Read the "packet" encoded by the encoder(speex_codec_demo.c)*/
         remain=nbBytes;
         data_buff_index=0;
         memset(data_buff, 0, CBITS_SIZE); 
         for (loop_idx = 0; loop_idx < 4; loop_idx++) {
             if(remain==0)
              break;
             share_buff_read_ptr=NULL; 
             share_buff_read_len=0; 
             handle->get_read_buffer(handle, &share_buff_read_ptr, &share_buff_read_len);
             if(share_buff_read_len>0){
                 consume=MINIMUM(share_buff_read_len,remain); 
                 remain-=consume;
                 memcpy(data_buff+data_buff_index, share_buff_read_ptr, consume);
                 //if(consume>=1)
                 data_buff_index+=consume;
                 handle->read_data_done(handle, consume); 
            }
         }
         memcpy(internal_handle->dec_cbits, data_buff, nbBytes);

     
        /*Copy the data into the bit-stream struct*/
        speex_bits_read_from(&internal_handle->dec_bits, (const char *)internal_handle->dec_cbits, nbBytes);
        /*Decode the data, return status (0 for no error, -1 for end of stream, -2 corrupt stream)*/
        ret=speex_decode(internal_handle->dec_state, &internal_handle->dec_bits, internal_handle->output);
        if(ret == -1){
          LOGW("end of stream\n");
          LOGI("speex_decode ret=%d\n",ret);
        }
        if(ret == -2){
          LOGW("corrupt stream\n");
          LOGI("speex_decode ret=%d\n",ret);
        }
        if(ret != -2){
         /*Copy from float to int16_t (16 bits) for output*/
         for (i=0;i<internal_handle->frame_size;i++){
          *(internal_handle->out+i)= *(internal_handle->output+i);
          //LOGI("i=%d *(out+i)=%d *(output+i)=%f\n\r",i,*(internal_handle->out+i),*(internal_handle->output+i));
        }
     
         uint32_t pcm_count_in_bytes = internal_handle->frame_size*2;
         uint32_t out_buf_index = 0;
     
         for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            uint8_t *stream_out_pcm_buff_ptr    = NULL;
            uint32_t stream_out_count_in_bytes = 0;
     
            ring_buffer_get_write_information(&internal_handle->stream_out_pcm_buff, &stream_out_pcm_buff_ptr, &stream_out_count_in_bytes);
            //LOGI(" loop_idx=%d stream_out_count_in_bytes=%d\n",loop_idx,stream_out_count_in_bytes);
     
            if (stream_out_count_in_bytes > 0) {
                uint32_t consumed_byte_count = MINIMUM(pcm_count_in_bytes, stream_out_count_in_bytes);
                uint32_t consumed_word_count=consumed_byte_count>>1;
                uint16_t *out_base         = (uint16_t *)internal_handle->out;
                uint16_t *out_buf          = out_base + out_buf_index;
                speex_codec_cp16to8(stream_out_pcm_buff_ptr, out_buf, consumed_word_count);
                out_buf_index += consumed_word_count;
                pcm_count_in_bytes  -= consumed_byte_count;
                ring_buffer_write_done(&internal_handle->stream_out_pcm_buff, consumed_byte_count);
                //LOGI(" consumed_byte_count=%d\n",consumed_byte_count);
            } else {
                // stream_out_pcm_buffer full, do nothing.
                break;
            }
         } 
       }
     
     }while((ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff)) >= (internal_handle->frame_size*2));

     if (!handle->waiting) {
         handle->waiting = true;
         handle->handler(handle, SPEEX_CODEC_MEDIA_EVENT_REQUEST);
     }
 
    //LOGI("[CTRL]deocde ISR handler done \n");
}


void speex_codec_task_create(void)
{
    LOGI("[CTRL]speex_codec_task_create \n");
    xTaskCreate(speex_codec_task_main, SPEEX_CODEC_TASK_NAME, SPEEX_CODEC_TASK_STACKSIZE / sizeof(portSTACK_TYPE), NULL, SPEEX_CODEC_TASK_PRIO, NULL);
}

#if 0
static speex_codec_status_t speex_codec_encoder_start(speex_codec_media_handle_t *handle)
{
    return SPEEX_CODEC_STATUS_OK;

}


static speex_codec_status_t speex_codec_encoder_open(speex_codec_media_handle_t *handle)
{
    if (handle->codec_state!= SPEEX_CODEC_STATE_READY ) {
        return SPEEX_CODEC_STATUS_ERROR;
    }
    
    int8_t encoder_quality=8;/*Set the quality to 8 (15 kbps)*/
    int8_t ret=-1;
    speex_codec_internal_handle_t *internal_handle = speex_codec_internal_handle;

    /*Configure the encoder*/
    if(handle->codec_config.speex_codec_mode == SPEEX_CODEC_NB_MODE){
           /*Create a new encoder state in narrowband mode*/
           internal_handle->enc_state = speex_encoder_init(&speex_nb_mode);
           LOGI("encoder in narrowband mode\n");
    }else {
        /*Create a new encoder state in wideband mode*/
        internal_handle->enc_state = speex_encoder_init(&speex_wb_mode);
        LOGI("encoder in wideband mode\n");
    }

    if(internal_handle->frame_size==0)
        speex_encoder_ctl(internal_handle->enc_state, SPEEX_GET_FRAME_SIZE, &internal_handle->frame_size);
    LOGI("frame_size for encoder=%d!\n\r",internal_handle->frame_size);

   internal_handle->in = pvPortMalloc(internal_handle->frame_size * sizeof(int16_t *));
   internal_handle->input = pvPortMalloc(internal_handle->frame_size * sizeof(float *));
   if((internal_handle->in == NULL) || (internal_handle->input == NULL) ){
       LOGE("Allocate buffer for encoder failed !\n");
       return SPEEX_CODEC_STATUS_ERROR;
   }

   ret=speex_encoder_ctl(internal_handle->enc_state, SPEEX_SET_QUALITY, &encoder_quality);
   if(ret != 0){
        LOGE("Set quality for encoder failed!\n\r");
        return SPEEX_CODEC_STATUS_ERROR;
   }

   /*Initialization of the structure that holds the bits*/
   speex_bits_init(&internal_handle->enc_bits);

   return SPEEX_CODEC_STATUS_OK;

}
#endif

static speex_codec_status_t speex_codec_decoder_start(speex_codec_media_handle_t *handle)
{
    LOGI("[CTRL]speex_codec_decoder_start \n");   

 #if 1   
    speex_codec_internal_handle_t *internal_handle = speex_codec_internal_handle; 
    uint8_t data_buff[CBITS_SIZE];
    uint8_t *share_buff_read_ptr=NULL;
    uint32_t share_buff_read_len=0;
    uint32_t data_buff_index=0;
    uint32_t remain=0;
    uint32_t consume=0;
    uint32_t nbBytes=0;
    uint32_t loop_idx=0;
    int8_t ret=0;
    uint32_t i=0;

    /*register HISR callback for decoder*/
    speex_codec_event_register_callback(SPEEX_CODEC_QUEUE_EVENT_DECODE, speex_codec_deocde_hisr_handler);

    do{
        /*Read the size encoded by the encoder(speex_codec_demo.c), this part will likely be 
              different in your application*/
        remain=4;
        data_buff_index=0;
        memset(data_buff, 0, CBITS_SIZE); 
        for (loop_idx = 0; loop_idx < 4; loop_idx++) {
            if(remain==0)
             break;
            share_buff_read_ptr=NULL; 
            share_buff_read_len=0; 
            handle->get_read_buffer(handle, &share_buff_read_ptr, &share_buff_read_len);
            if(share_buff_read_len>0){
                //LOGI(" loop_idx=%d share_buff_read_len=%d\n",loop_idx,share_buff_read_len);
                consume=MINIMUM(share_buff_read_len,remain); 
                //LOGI(" consume=%d \n",consume);
                remain-=consume;
                memcpy(data_buff+data_buff_index, share_buff_read_ptr, consume);
                //if(consume>=1)
                data_buff_index+=consume;
                handle->read_data_done(handle, consume);
           }
        }
        memcpy(&nbBytes, data_buff, 4);
        LOGI("nbBytes from share buffer=%d\n",(int)nbBytes);
        
         /*check the nbBytes*/
        if((nbBytes > CBITS_SIZE) || (nbBytes==0)){
         LOGW("WARNING! nbBytes=%d , CBITS_SIZE=%d", (int)nbBytes, CBITS_SIZE);
         break;
        }
        
        /*Read the "packet" encoded by the encoder(speex_codec_demo.c)*/
        remain=nbBytes;
        data_buff_index=0;
        memset(data_buff, 0, CBITS_SIZE); 
        for (loop_idx = 0; loop_idx < 4; loop_idx++) {
            if(remain==0)
             break;
            share_buff_read_ptr=NULL; 
            share_buff_read_len=0;   
            handle->get_read_buffer(handle, &share_buff_read_ptr, &share_buff_read_len);
            if(share_buff_read_len>0){
                consume=MINIMUM(share_buff_read_len,remain); 
                remain-=consume;
                memcpy(data_buff+data_buff_index, share_buff_read_ptr, consume);
                //if(consume>=1)
                data_buff_index+=consume;
                handle->read_data_done(handle, consume); 
           }
        }
        memcpy(internal_handle->dec_cbits, data_buff, nbBytes);

       /*Copy the data into the bit-stream struct*/
       speex_bits_read_from(&internal_handle->dec_bits, (const char *)internal_handle->dec_cbits, nbBytes);
       /*Decode the data, return status (0 for no error, -1 for end of stream, -2 corrupt stream)*/
       ret=speex_decode(internal_handle->dec_state, &internal_handle->dec_bits, internal_handle->output);
       LOGI("speex_decode ret=%d\n",ret);
       if(ret == -1)
         LOGW("end of stream\n");
       if(ret == -2)
         LOGW("corrupt stream\n");
       if(ret != -2){
        /*Copy from float to int16_t (16 bits) for output*/
        for (i=0;i<internal_handle->frame_size;i++){
         *(internal_handle->out+i)= *(internal_handle->output+i);
         //LOGI("i=%d *(out+i)=%d *(output+i)=%f\n\r",i,*(internal_handle->out+i),*(internal_handle->output+i));
       }

        uint32_t pcm_count_in_bytes = internal_handle->frame_size*2;
        uint32_t out_buf_index = 0;

        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
           uint8_t *stream_out_pcm_buff_ptr    = NULL;
           uint32_t stream_out_count_in_bytes = 0;

           ring_buffer_get_write_information(&internal_handle->stream_out_pcm_buff, &stream_out_pcm_buff_ptr, &stream_out_count_in_bytes);
           //LOGI(" loop_idx=%d stream_out_count_in_bytes=%d\n",loop_idx,stream_out_count_in_bytes);

           if (stream_out_count_in_bytes > 0) {
               uint32_t consumed_byte_count = MINIMUM(pcm_count_in_bytes, stream_out_count_in_bytes);
               uint32_t consumed_word_count=consumed_byte_count>>1;
               uint16_t *out_base         = (uint16_t *)internal_handle->out;
               uint16_t *out_buf          = out_base + out_buf_index;
               speex_codec_cp16to8(stream_out_pcm_buff_ptr, out_buf, consumed_word_count);
               out_buf_index += consumed_word_count;
               pcm_count_in_bytes  -= consumed_byte_count;
               ring_buffer_write_done(&internal_handle->stream_out_pcm_buff, consumed_byte_count);
               //LOGI(" consumed_byte_count=%d\n",consumed_byte_count);
           } else {
               // stream_out_pcm_buffer full, do nothing.
               break;
           }
        } 
      }

    }while((ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff)) >= (internal_handle->frame_size*2));

     if (!handle->waiting) {
         handle->waiting = true;
         handle->handler(handle, SPEEX_CODEC_MEDIA_EVENT_REQUEST);
     }
     
     ret = speex_codec_nau8810_configure();
     if (ret == -1) 
         LOGE("speex_codec_nau8810_configure failed---\n");
         
     ret = speex_codec_i2s_configure();
     if (ret == -1) 
         LOGE("speex_codec_i2s_configure failed---\n");

     hal_i2s_register_tx_vfifo_callback(speex_codec_i2s_tx_callback, NULL);
     /* enable dma interrupt */
     hal_i2s_enable_tx_dma_interrupt();
     speex_codec_i2s_tx_enable();
#endif
     return SPEEX_CODEC_STATUS_OK;
}


static speex_codec_status_t speex_codec_decoder_open(speex_codec_media_handle_t *handle)
{
     LOGI("[CTRL]speex_codec_decoder_open \n");
     
     if (handle->codec_state != SPEEX_CODEC_STATE_READY ) {
         return SPEEX_CODEC_STATUS_ERROR;
     }
     
     speex_codec_internal_handle_t *internal_handle = speex_codec_internal_handle;
     int8_t decoder_enhance=1; /*Set the perceptual enhancement on*/
     int8_t ret=-1;

     /*Configure the decoder*/
     if(handle->codec_config.speex_codec_mode == SPEEX_CODEC_NB_MODE){
            /*Create a new decoder state in narrowband mode*/
            internal_handle->dec_state = speex_decoder_init(&speex_nb_mode);
            LOGI("decoder in narrowband mode\n");
     }else {
         /*Create a new decoder state in wideband mode*/
         internal_handle->dec_state = speex_decoder_init(&speex_wb_mode);
         LOGI("decoder in wideband mode\n");
     }

    //You can get that value in the frame_size variable (expressed in samples, not bytes)
     if(internal_handle->frame_size==0)
         speex_decoder_ctl(internal_handle->dec_state, SPEEX_GET_FRAME_SIZE, &internal_handle->frame_size);
     LOGI("frame_size for decoder=%d samples\n\r",internal_handle->frame_size);

    internal_handle->out = pvPortMalloc(internal_handle->frame_size * sizeof(int16_t *));
    internal_handle->output = pvPortMalloc(internal_handle->frame_size * sizeof(float *));
    internal_handle->dec_cbits= pvPortMalloc(CBITS_SIZE * sizeof(int8_t *));
    if((internal_handle->out == NULL) || (internal_handle->output == NULL) || (internal_handle->dec_cbits == NULL)){
        LOGE("Allocate buffer for decoder failed !\n");
        return SPEEX_CODEC_STATUS_ERROR;
    }

    ret=speex_decoder_ctl(internal_handle->dec_state, SPEEX_SET_ENH, &decoder_enhance);
    if(ret != 0){
         LOGE("open enhance for decoder failed!\n\r");
         return SPEEX_CODEC_STATUS_ERROR;
    }

    /*Initialization of the structure that holds the bits*/
    speex_bits_init(&internal_handle->dec_bits);

    /*configure stream out pcm buffer,*/
    internal_handle->stream_out_pcm_buff_size = (internal_handle->frame_size*2)/*in byte (16 bits/sample)*/ * 32;
    internal_handle->stream_out_pcm_buff.buffer_base_pointer= pvPortMalloc(internal_handle->stream_out_pcm_buff_size * sizeof(uint8_t *));
    internal_handle->stream_out_pcm_buff.buffer_byte_count=internal_handle->stream_out_pcm_buff_size;
    internal_handle->stream_out_pcm_buff.read_pointer = 0;
    internal_handle->stream_out_pcm_buff.write_pointer = 0;
    LOGI("stream_out_pcm_buff_size=%d bytes\n",internal_handle->stream_out_pcm_buff_size);
    if(internal_handle->stream_out_pcm_buff.buffer_base_pointer == NULL){
        LOGE("Allocate buffer for stream out pcm failed !\n");
        return SPEEX_CODEC_STATUS_ERROR;
    }
    
    handle->codec_state = SPEEX_CODEC_STATE_DECODING;
    return speex_codec_decoder_start(handle);
}

static speex_codec_status_t speex_codec_stop(speex_codec_media_handle_t *handle)
{
    LOGI("[CTRL]speex_codec_stop \n");

    if (handle->codec_state != SPEEX_CODEC_STATE_READY  || handle->codec_state != SPEEX_CODEC_STATE_DECODING) {
        return SPEEX_CODEC_STATUS_ERROR;
    }

    speex_codec_i2s_tx_disable();
    aucodec_i2c_deinit();
    speex_codec_reset_share_buffer(handle);
    speex_codec_reset_pcm_out_buffer();   
    speex_codec_event_deregister_callback(SPEEX_CODEC_QUEUE_EVENT_DECODE);

    handle->codec_state = SPEEX_CODEC_STATE_STOP;
    return SPEEX_CODEC_STATUS_OK;

}

speex_codec_status_t speex_codec_close(speex_codec_media_handle_t *handle)
{
    LOGI("[CTRL]speex_codec_close \n");

    speex_codec_internal_handle_t *internal_handle = speex_codec_internal_handle; 
    if (handle->codec_state != SPEEX_CODEC_STATE_STOP ) {
        return SPEEX_CODEC_STATUS_ERROR;
    }

    if(internal_handle->in != NULL){
        vPortFree(internal_handle->in);
        internal_handle->in = NULL;
    }
    
    if(internal_handle->input != NULL){
        vPortFree(internal_handle->input);
        internal_handle->input = NULL;
    }
    
    if(internal_handle->out != NULL){
        vPortFree(internal_handle->out);
        internal_handle->out = NULL;
    }   
    
    if(internal_handle->output != NULL){
        vPortFree(internal_handle->output);
        internal_handle->output = NULL;
    }  

    if(internal_handle->stream_out_pcm_buff.buffer_base_pointer != NULL){
        vPortFree(internal_handle->stream_out_pcm_buff.buffer_base_pointer);
        internal_handle->stream_out_pcm_buff.buffer_base_pointer = NULL;
    }  

    vPortFree(internal_handle);
    internal_handle =speex_codec_internal_handle= NULL;

    return SPEEX_CODEC_STATUS_OK;


}

speex_codec_media_handle_t *speex_codec_open(speex_codec_callback_t speex_codec_callback, const speex_codec_config_t speex_codec_config)
{
    LOGI("[CTRL]speex_codec_open \n");
    
    speex_codec_media_handle_t *handle;
    speex_codec_internal_handle_t *internal_handle; 

    /* alloc internal handler space and initialize it*/
    internal_handle = (speex_codec_internal_handle_t *)pvPortMalloc(sizeof(speex_codec_internal_handle_t));
    memset(internal_handle, 0, sizeof(speex_codec_internal_handle_t));

    /* assign internal handler to be global and static handler*/
    speex_codec_internal_handle = internal_handle;
    
    /* initialize internal handle*/
    internal_handle->in=NULL;
    internal_handle->input=NULL;
    internal_handle->out=NULL;
    internal_handle->output=NULL;
    internal_handle->stream_out_pcm_buff.buffer_base_pointer=NULL;
    internal_handle->frame_size=0;
    internal_handle->stream_out_pcm_buff_size=0;

    /* initialize handle*/
    handle = &internal_handle->handle;
    handle->handler=speex_codec_callback;
    //handle->encode   = speex_codec_encoder_open;
    handle->decode=speex_codec_decoder_open;
    handle->stop=speex_codec_stop;
    handle->codec_state=SPEEX_CODEC_STATE_READY;

    /*initialize buffer control function*/
    speex_codec_buffer_function_init(handle);

    /*set user configuration*/
    handle->codec_config.speex_codec_mode=speex_codec_config.speex_codec_mode;
    handle->codec_config.speex_codec_channer_number=speex_codec_config.speex_codec_channer_number;
    handle->codec_config.speex_codec_sample_rate=speex_codec_config.speex_codec_sample_rate;
    LOGI("COCDEC_MODE=%d \n",(int)handle->codec_config.speex_codec_mode);

    /*create encode/decode task*/
    speex_codec_task_create();

    return handle;

}

static void speex_codec_i2s_tx_callback(hal_i2s_event_t event, void *user_data)
{
    //LOGI("[CTRL]i2s_tx_callback \n");
    speex_codec_internal_handle_t *internal_handle = speex_codec_internal_handle; 
    speex_codec_media_handle_t *handle=(speex_codec_media_handle_t *)internal_handle;
    
    uint8_t loop_idx = 0;
    uint32_t tx_vfifo_space=0;//in 4bytes
    uint32_t consume_samples=0;
    uint32_t i2s_temp_data = 0;
    uint32_t i = 0;
    uint32_t pcm_out_space_in_bytes = 0;

    switch (event) {
        case HAL_I2S_EVENT_DATA_REQUEST:
        {
              for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                    uint8_t *stream_out_pcm_buff_ptr    = NULL;
                    uint32_t stream_out_pcm_buff_data_count = 0;
                    hal_i2s_get_tx_sample_count(&tx_vfifo_space);
                    //LOGI("[TX] loop_idx=%d tx_vfifo_space=%d ",loop_idx,tx_vfifo_space);
                    ring_buffer_get_read_information(&internal_handle->stream_out_pcm_buff, &stream_out_pcm_buff_ptr, &stream_out_pcm_buff_data_count);
                    //LOGI("[TX] stream_out_pcm_buff_data_count=%d ",stream_out_pcm_buff_data_count);
                       
                    if((tx_vfifo_space>0) && (stream_out_pcm_buff_data_count>0)){
                        if( handle->codec_config.speex_codec_channer_number==0)//for mono
                            stream_out_pcm_buff_data_count=stream_out_pcm_buff_data_count/2;

                        //LOGI("[TX] stream_out_pcm_buff_data_count=%d ",stream_out_pcm_buff_data_count);

                        consume_samples = MINIMUM(tx_vfifo_space, stream_out_pcm_buff_data_count);
                        //LOGI("[TX] consume_samples=%d ",consume_samples);

                        if( handle->codec_config.speex_codec_channer_number==0){//for mono
                            for(i = 0; i < consume_samples; i++){
                                i2s_temp_data=0;
                                i2s_temp_data|=((uint32_t)*(stream_out_pcm_buff_ptr+2*i));
                                i2s_temp_data|=((uint32_t)*(stream_out_pcm_buff_ptr+2*i+1))<<8;
                                hal_i2s_tx_write(i2s_temp_data);
                                //LOGI("i2s_temp_data[%d]=%x\n",i,i2s_temp_data);
                            }
                        }
                        
                        //LOGI("[TX] i2s write %d samples \n",consume_samples);
                        if( handle->codec_config.speex_codec_channer_number==0)//for mono
                            ring_buffer_read_done(&internal_handle->stream_out_pcm_buff, (consume_samples * 2));
 
                     }
                                                
             }   

             pcm_out_space_in_bytes= ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff);
             //LOGI("[TX] pcm_out_space_in_bytes=%d bytess \n",pcm_out_space_in_bytes);
            if (pcm_out_space_in_bytes >= internal_handle->frame_size*2){
               speex_codec_event_send_from_isr(SPEEX_CODEC_QUEUE_EVENT_DECODE, NULL);   
            }
        }      
       break;
    };
    
}


static int8_t speex_codec_i2s_configure(void)
{
    LOGI("[CTRL]i2s_configure \n");
    hal_i2s_config_t i2s_config;
    hal_i2s_status_t result = HAL_I2S_STATUS_OK;

    result = hal_i2s_init(HAL_I2S_TYPE_EXTERNAL_MODE);
    if (HAL_I2S_STATUS_OK != result) {
        LOGE("hal_i2s_init failed\n");
        return -1;
    }

    /* Configure I2S  */
    i2s_config.clock_mode = HAL_I2S_SLAVE;
    i2s_config.tx_mode = HAL_I2S_TX_DUPLICATE_DISABLE;
    i2s_config.i2s_out.channel_number = HAL_I2S_STEREO;

    i2s_config.i2s_out.sample_rate = HAL_I2S_SAMPLE_RATE_16K;//no any effect
    i2s_config.i2s_in.sample_rate = HAL_I2S_SAMPLE_RATE_16K;//no any effect
    i2s_config.i2s_in.msb_offset = 0;
    i2s_config.i2s_out.msb_offset = 0;
    i2s_config.i2s_in.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_out.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_in.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
    i2s_config.i2s_out.lr_swap = HAL_I2S_LR_SWAP_DISABLE;

    result = hal_i2s_set_config(&i2s_config);
    if (HAL_I2S_STATUS_OK != result) {
        LOGE("hal_i2s_set_config failed\n");
        return -1;
    }

    result = hal_i2s_setup_tx_vfifo(I2S_TX_VFIFO, I2S_TX_VFIFO_LENGTH / 2, I2S_TX_VFIFO_LENGTH);
    if (HAL_I2S_STATUS_OK != result) {
        LOGE("hal_i2s_setup_tx_vfifo failed\n");
        return -1;
    }

    result = hal_i2s_setup_rx_vfifo(I2S_RX_VFIFO, I2S_RX_VFIFO_LENGTH / 2, I2S_RX_VFIFO_LENGTH);
    if (HAL_I2S_STATUS_OK != result) {
        LOGE("hal_i2s_setup_rx_vfifo failed\n");
        return -1;
    }
    return 1;
}


static int8_t speex_codec_nau8810_configure(void)
{
    LOGI("[CTRL]nau8810_configure \n");
    
    speex_codec_internal_handle_t *internal_handle = speex_codec_internal_handle; 
    speex_codec_media_handle_t *handle=(speex_codec_media_handle_t *)internal_handle;

    /*configure NAU8810*/
    AUCODEC_STATUS_e codec_status;
    AUCODEC_SAMPLERATE_SEL_e codec_fs;
    hal_i2c_port_t i2c_port;
    hal_i2c_frequency_t frequency;

    codec_status = AUCODEC_STATUS_OK;
    i2c_port = HAL_I2C_MASTER_0;
    frequency = HAL_I2C_FREQUENCY_50K;

    codec_status = aucodec_i2c_init(i2c_port, frequency); //init codec
    if (codec_status != AUCODEC_STATUS_OK) {
        LOGE("aucodec_i2c_init failed\n");
    }

    aucodec_softreset();//soft reset

    codec_status = aucodec_init();
    if (codec_status != AUCODEC_STATUS_OK) {
        LOGE("aucodec_init failed\n");
    }

    aucodec_set_dai_fmt(eI2S, e16Bit, eBCLK_NO_INV);//set DAI format

    codec_fs=speex_codec_get_codec_sampling_rate(handle->codec_config.speex_codec_sample_rate);
    codec_status = aucodec_set_dai_sysclk(codec_fs, eMASTER, e32xFS, 16000000, ePLLEnable);
    if (codec_status != AUCODEC_STATUS_OK) {
        LOGE("aucodec_init failed\n");
    }

    //aucodec_set_input(eMicIn);//Input: MIC, Output:  ADCOUT
    //aucodec_set_output(eSpkOut);//Input: DACIN, Output:  speaker out
    aucodec_set_output(eLineOut);//Input: DACIN, Output:  aux out

    if (codec_status == AUCODEC_STATUS_OK) {
        return 1;
    } else {
        return -1;
    }

}


static void speex_codec_i2s_tx_enable(void)
{
    LOGI("[CTRL]i2s open \n");
    hal_i2s_enable_tx();
    hal_i2s_enable_audio_top();
}

static void speex_codec_i2s_tx_disable(void)
{
    LOGI("[CTRL]i2s close \n");
    hal_i2s_disable_tx();
    hal_i2s_disable_audio_top();
}


static void speex_codec_cp16to8(uint8_t* dest, uint16_t* src, int32_t lenth_in_word)
{
    int32_t i=0;
    for(i=0; i<lenth_in_word ;i++ ){
        *(dest+2*i) = (uint8_t)*(src+i) & 0xff;
        *(dest+2*i+1)=(uint8_t)(*(src+i) >> 8) ;
#if 0
        LOGI("[dest[%d]=%x ",2*i,*(dest+2*i) );
        LOGI("[dest[%d]=%x ",2*i+1,*(dest+2*i+1) );
        LOGI("[src[%d]=%x ",i,*(src+i));
#endif        
    }
}

static AUCODEC_SAMPLERATE_SEL_e speex_codec_get_codec_sampling_rate(speex_codec_sample_rate_t sampling_rate)
{

    AUCODEC_SAMPLERATE_SEL_e codec_fs=eSR8KHz;

    switch (sampling_rate) {
        case SPEEX_CODEC_SAMPLE_RATE_8K: 
             codec_fs=eSR8KHz;
             LOGI("CODEC FS is 8KHZ\n");
           break;
        case SPEEX_CODEC_SAMPLE_RATE_16K: 
             codec_fs=eSR16KHz;
             LOGI("CODEC FS is 16KHZ\n");
           break;
        case SPEEX_CODEC_SAMPLE_RATE_32K: 
             codec_fs=eSR32KHz;
             LOGI("CODEC FS is 32KHZ\n");
           break;
    }

    return codec_fs;
}



