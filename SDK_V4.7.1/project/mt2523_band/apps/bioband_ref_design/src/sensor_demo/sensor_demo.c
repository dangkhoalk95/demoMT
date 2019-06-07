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

#include "sensor_demo.h"

#ifdef SENSOR_DEMO

/* sensor subsys includes */
#include "sensor_alg_interface.h"

#include "string.h"
#include "stdlib.h"

#ifdef MTK_SENSOR_BIO_USE_MT2511
#include "vsm_sensor_subsys_adaptor.h"
#endif

#include "hal_gpio.h"
#include "task_def.h"

#include "sensor_bt_spp_server.h"
#include "ble_dtp.h"
#include "nvdm.h"

#ifdef DTP_ENABLE
extern bool dtp_enabled;
extern bool dtp_sent_done;

/* 10 second heart rate data*/
static uint8_t heart_rate_10s_data[17];
static uint32_t heart_data_counter;
static uint8_t heart_rate_variability_data[23];
#endif

#define BP_CAL_TEST 1

int32_t all_sensor_send_digest_callback(sensor_data_t *const output);

#ifdef FUSION_PEDOMETER_USE
sensor_subscriber_t pedometer_subscriber = {
    "ap12", 0, SENSOR_TYPE_PEDOMETER, 0, all_sensor_send_digest_callback, 0
};
static uint32_t pedo_triggered = 0;
#endif

#ifdef FUSION_SLEEP_TRACKER_USE
#include "sleep.h"
sensor_subscriber_t sleep_subscriber = {
    "ap19", 0, SENSOR_TYPE_SLEEP, 0, all_sensor_send_digest_callback, 0
};
static uint32_t sleep_triggered = 0;
static uint32_t st_triggered = 0;
#endif

#ifdef FUSION_SLEEP_STAGING_USE
sensor_subscriber_t sleep_staging_subscriber = {
    "ap25", 0, SENSOR_TYPE_SLEEP_STAGING, 0, all_sensor_send_digest_callback, 0
};
static uint32_t sleep_staging_triggered = 0;
#endif

#ifdef AUTOMATIC_MOTION_DETECT
sensor_subscriber_t motion_detect_subscriber = {
    "ap26", 0, SENSOR_TYPE_MOTION_DETECT , 0, all_sensor_send_digest_callback, 0
};
static uint32_t motion_detect_triggered = 0;
#endif

#ifdef FUSION_HEART_RATE_MONITOR_USE
sensor_subscriber_t heart_rate_monitor_subscriber = {
    "ap21", 0, SENSOR_TYPE_HEART_RATE_MONITOR, 0, all_sensor_send_digest_callback, 0
};
static uint32_t hr_triggered = 0;
#endif /*FUSION_HEART_RATE_MONITOR_USE*/

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
sensor_subscriber_t heart_rate_variability_subscriber = {
    "ap22", 0, SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR, 0, all_sensor_send_digest_callback, 0
};

static uint32_t hrv_triggered = 0;
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
sensor_subscriber_t blood_pressure_subscriber = {
    "ap23", 0, SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 0, all_sensor_send_digest_callback, 0
};

static uint32_t bp_triggered = 0;
#endif

#ifdef FUSION_SLEEP_STAGING_USE
#include "sleep.h"
#endif


#ifdef AUTOMATIC_MOTION_DETECT

#define WATCH_OFF_HR_CHECK_COUNT_MAX  25  /*DEFINE DEPEND ON MODULE*/
void enable_motion_detect(void);
int32_t watch_off_hr_count = 0;

#endif

void enable_sleep_tracker(void);
void disable_sleep_tracker(void);

static int ppg1_buff_count = 0;
static uint32_t ppg2_buff_data;
static uint32_t ppg2_buff_timestamp;
static int ppg2_buff_count = 0;
static int ekg_buff_count = 0;

static int32_t ppg1_buff_data_array[12];
static int32_t ekg_buff_data_array[12];
static int32_t acc_buff_data_array[12];
static int32_t bpm_buff_data_array[12] = {SENSOR_SPP_DATA_MAGIC,SENSOR_TYPE_BLOOD_PRESSURE_MONITOR,0,0,0,0,0,0,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED,SENSOR_SPP_DATA_RESERVED};
static int32_t bisi_data_array[2];
static int32_t ppg1_seq = 1;
static int32_t ekg_seq = 1;
static int32_t acc_seq = 1;
static int32_t bisi_seq = 1;
static int acc_buff_count = 0;
static int32_t ppg1_amb_flag = 1;

/*sleep state transmission start */
static int32_t sleep_tracker_seq;
static int32_t sleep_tracker_buff_count;
static int32_t asleep_tracker_data[12];
/*sleep state transmission end */

/*sleep state transmission start */
static int32_t sleep_feature_seq;
static int32_t sleep_feature_data[12];
/*sleep feature transmission end */

/*pwtt transmission start */
static int32_t pwtt_seq;
static int32_t apwtt_data[12];
/*pwtt transmission end */

/*beat position transmission start */
static int32_t beatpos_seq;
static int32_t abeatpos_data[12];
/*beat position transmission end */

#ifdef GT_PROJECT_ENABLE
#include "watch.h"
#endif //GT_PROJECT_ENABLE

//#define DUMP_RAW_DATA_IN_UART
//#define DUMP_BP_DATA_IN_UART

#if defined(DUMP_RAW_DATA_IN_UART) && defined(DUMP_BP_DATA_IN_UART)
#error Two macro turn on simultaneously
#endif

#if defined(DUMP_RAW_DATA_IN_UART) || defined(DUMP_BP_DATA_IN_UART)
static void sensor_dump_raw_data_in_bt_log_format(uint32_t sensor_type, int32_t seq, int32_t *buff, int32_t len)
{
    if ( len == 12) {
        printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,1490239290\r\n",
                        sensor_type, seq, buff[0], buff[1], 
                        buff[2], buff[3], buff[4], buff[5], 
                        buff[6], buff[7], buff[8], buff[9], 
                        buff[10],buff[11],SENSOR_SPP_DATA_RESERVED);
    } else {
        char output_str[200], input_str[20];
        int i = len;
        memset(output_str,0,sizeof(output_str));
        memset(input_str,0,sizeof(input_str));
        sprintf((char *)input_str, "%d,%d", sensor_type, seq);
        strcat((char *)output_str, (char *)input_str);
        for (i = 0; i < 12; i ++) {
            memset(input_str,0,sizeof(input_str));
            if (i < len) {
                sprintf((char *)input_str, ",%d", buff[i]);
                strcat((char *)output_str, (char *)input_str);
            } else {
                sprintf((char *)input_str, ",%d", 0);
                strcat((char *)output_str, (char *)input_str);
            }
        }
        printf("%s,%d,1490239290\r\n",
                        output_str, SENSOR_SPP_DATA_RESERVED);
    }
}
#endif

#if defined(DUMP_BP_DATA_IN_UART)
#define EXPECT_ECG_LENGTH       (12804)
#define EXPECT_PPG_LENGTH       (12804)
#define DUMP_BP_PPG_DATA
#define DUMP_BP_EKG_DATA
#define PAGE_NUMBER         (100)
#define TIME_TO_WAIT_READY  (10000)
static uint32_t bp_ecg_data[EXPECT_ECG_LENGTH];
static uint32_t bp_ppg_data[EXPECT_PPG_LENGTH];
static int32_t bp_ecg_idx;
static int32_t bp_ppg_idx;
static int32_t bp_print_idx;
static int32_t bp_pwtt_cnt;
static int32_t bp_pwtt_data_array[10][16];

static void sensor_dump_bp_TimerCallback(void *arg);

static void sensor_dump_bp_TimerCallback(void *arg)
{
    int32_t len = EXPECT_ECG_LENGTH/12, i = 0;
    int32_t phase = PAGE_NUMBER;
    int32_t bp_ppg1_buff_data[12] ;
    int32_t bp_ekg_buff_data[12] ;
    vTaskDelay(TIME_TO_WAIT_READY);
    for (i = 0; i < len; i ++) {
        if (bp_print_idx < len) {
            #ifdef DUMP_BP_PPG_DATA
            memcpy(bp_ppg1_buff_data, bp_ppg_data + bp_print_idx * 12, sizeof(uint32_t) * 12);
            sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_BIOSENSOR_PPG1_512HZ, bp_print_idx, bp_ppg1_buff_data, 12);
            #endif

            #ifdef DUMP_BP_EKG_DATA
            memcpy(bp_ekg_buff_data, bp_ecg_data + bp_print_idx * 12, sizeof(uint32_t) * 12);
            sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_BIOSENSOR_EKG, bp_print_idx, bp_ekg_buff_data, 12);
            #endif

            bp_print_idx++;
            vTaskDelay(50);
        }
    }

    /* dump BP result*/
    for (i = 0; i < bp_pwtt_cnt; i ++) {
        sensor_dump_raw_data_in_bt_log_format(bp_pwtt_data_array[i][1],bp_pwtt_data_array[i][2],&(bp_pwtt_data_array[i][3]),12);
        vTaskDelay(50);
    }
    sensor_dump_raw_data_in_bt_log_format(bpm_buff_data_array[1],bpm_buff_data_array[2],&(bpm_buff_data_array[3]),12);
    vTaskDelay(50);

    vTaskDelete(NULL);
}
#endif

void algorithm_on_indication(uint32_t sensor_type, int32_t on)
{
    send_sensor_data_via_btspp(
            SENSOR_SPP_DATA_MAGIC,
            sensor_type | 0xFF00,
            on,
            0,
            0,
            0,
            SENSOR_SPP_DATA_RESERVED);
}

int32_t all_sensor_send_digest_callback(sensor_data_t *const output)
{
    uint32_t i = 0;
    uint32_t bunch_count = 0;

#ifdef GT_PROJECT_ENABLE
    tEvent outEvent;
#endif
    switch (output->data[0].sensor_type) {

        case SENSOR_TYPE_ACCELEROMETER:
#if 0
            SENSOR_DEMO_LOGI("acc sensor  type = %lu , value = ( %ld , %ld , %ld ) (%ld) , timestamp = %lu \r\n",
                             output->data[0].sensor_type,
                             output->data[0].accelerometer_t.x,
                             output->data[0].accelerometer_t.y,
                             output->data[0].accelerometer_t.z,
                             output->data[0].accelerometer_t.status,
                             output->data[0].time_stamp
                            );
#endif
#if 0
            send_sensor_data_via_btspp(
                SENSOR_SPP_DATA_MAGIC,
                SENSOR_TYPE_ACCELEROMETER,
                output->data[0].accelerometer_t.x,
                output->data[0].accelerometer_t.y,
                output->data[0].accelerometer_t.z,
                output->data[0].time_stamp,
                SENSOR_SPP_DATA_RESERVED);
#endif

            if (acc_buff_count < 12) {
                acc_buff_data_array[acc_buff_count]  = output->data[0].accelerometer_t.x;
                acc_buff_data_array[acc_buff_count + 1]  = output->data[0].accelerometer_t.y;
                acc_buff_data_array[acc_buff_count + 2]  = output->data[0].accelerometer_t.z;
                acc_buff_data_array[acc_buff_count + 3]  = output->data[0].time_stamp;
                acc_buff_count += 4;
            }
            if (acc_buff_count == 12) {
                #ifdef DUMP_RAW_DATA_IN_UART
                sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_ACCELEROMETER, acc_seq, (int32_t *)acc_buff_data_array, 12);
                #endif
                send_sensor_16_datas_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_ACCELEROMETER,
                    acc_seq,
                    acc_buff_data_array,
                    12,
                    SENSOR_SPP_DATA_RESERVED);
                acc_seq++;
                acc_buff_count = 0;
            }
            break;
        case SENSOR_TYPE_MOTION_DETECT:
	    #ifdef AUTOMATIC_MOTION_DETECT
            SENSOR_DEMO_LOGI("output->data[0].value[0]:%d", output->data[0].value[0]);
            if(output->data[0].value[0]> 0){
	       enable_motion_detect();
	       vTaskDelay(1000);
	       enable_hr();
            }
	    #endif
            break;
        case SENSOR_TYPE_BIOSENSOR_PPG1_512HZ:
            if (ppg1_amb_flag == 1) {
                // SENSOR_TYPE_BIOSENSOR_PPG1_512HZ only sample ppg , no amb
                if (ppg1_buff_count < 12) {
                    ppg1_buff_data_array[ppg1_buff_count] = output->data[0].bio_data.data;
                    ppg1_buff_count++;
                    #ifdef DUMP_BP_DATA_IN_UART
                    if (bp_ppg_idx < EXPECT_PPG_LENGTH) {
                        bp_ppg_data[bp_ppg_idx++] = output->data[i].bio_data.data;
                    }
                    #endif
                }
                if (ppg1_buff_count == 12) {
                    #ifdef DUMP_RAW_DATA_IN_UART
                    sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_BIOSENSOR_PPG1_512HZ, ppg1_seq, (int32_t *)ppg1_buff_data_array, 12);
                    #endif

                    send_sensor_16_datas_via_btspp(
                        SENSOR_SPP_DATA_MAGIC,
                        SENSOR_TYPE_BIOSENSOR_PPG1_512HZ,
                        ppg1_seq,
                        ppg1_buff_data_array,
                        12,
                        SENSOR_SPP_DATA_RESERVED);
                    ppg1_seq++;
                    ppg1_buff_count = 0;
                }
                ppg1_amb_flag = 0;
            } else {
                ppg1_amb_flag = 1;
            }

            break;

        case SENSOR_TYPE_BIOSENSOR_PPG1:
            if (ppg1_buff_count < 12) {
                ppg1_buff_data_array[ppg1_buff_count] = output->data[0].bio_data.data;
                ppg1_buff_count++;
            }
            if (ppg1_buff_count == 12) {
                #ifdef DUMP_RAW_DATA_IN_UART
                sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_BIOSENSOR_PPG1, ppg1_seq, (int32_t *)ppg1_buff_data_array, 12);
                #endif
                send_sensor_16_datas_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_BIOSENSOR_PPG1,
                    ppg1_seq,
                    ppg1_buff_data_array,
                    12,
                    SENSOR_SPP_DATA_RESERVED);
                ppg1_seq++;
                ppg1_buff_count = 0;
            }
            break;
        case SENSOR_TYPE_BIOSENSOR_PPG2:
            if (ppg2_buff_count == 0) {
                ppg2_buff_data = output->data[0].bio_data.data;
                ppg2_buff_timestamp = output->data[0].time_stamp;
                ppg2_buff_count++;
            } else {
                send_sensor_data_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_BIOSENSOR_PPG2,
                    ppg2_buff_data,
                    ppg2_buff_timestamp,
                    output->data[0].bio_data.data,
                    output->data[0].time_stamp,
                    SENSOR_SPP_DATA_RESERVED);
                ppg2_buff_count = 0;
            }
            break;
        case SENSOR_TYPE_BIOSENSOR_BISI:
            {
                #ifdef DUMP_RAW_DATA_IN_UART
                uint32_t buff[2];
                buff[0] = output->data[0].bio_data.data;
                buff[1] = output->data[0].time_stamp;
                sensor_dump_raw_data_in_bt_log_format(output->data[0].sensor_type, acc_seq, (int32_t *)acc_buff_data_array, 2);
                #endif
                bisi_data_array[0] = output->data[0].bio_data.data;
                bisi_data_array[1] = output->data[0].time_stamp;

                send_sensor_16_datas_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_BIOSENSOR_BISI,
                    bisi_seq,
                    bisi_data_array,
                    2,
                    SENSOR_SPP_DATA_RESERVED);
                bisi_seq++;
            }
            break;
        case SENSOR_TYPE_BIOSENSOR_EKG:
            if (ekg_buff_count < 12) {
                ekg_buff_data_array[ekg_buff_count] = output->data[0].bio_data.data;
                ekg_buff_count++;
                #ifdef DUMP_BP_DATA_IN_UART
                if (bp_ecg_idx < EXPECT_PPG_LENGTH) {
                    bp_ecg_data[bp_ecg_idx++] = output->data[i].bio_data.data;
                }
                #endif
            }
            if (ekg_buff_count == 12) {
                #ifdef DUMP_RAW_DATA_IN_UART
                sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_BIOSENSOR_EKG, ekg_seq, (int32_t *)ekg_buff_data_array,12);
                #endif

                send_sensor_16_datas_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_BIOSENSOR_EKG,
                    ekg_seq,
                    ekg_buff_data_array,
                    12,
                    SENSOR_SPP_DATA_RESERVED);
                ekg_seq++;
                ekg_buff_count = 0;
            }
            break;

        case SENSOR_TYPE_PEDOMETER:
            SENSOR_DEMO_LOGI("pedometer sensor  type = %lu , value =  %ld , %ld , %ld , %ld , %ld timestamp = %lu \r\n",
                             output->data[0].sensor_type,
                             output->data[0].pedometer_t.accumulated_step_count,
                             output->data[0].pedometer_t.accumulated_step_length,
                             output->data[0].pedometer_t.step_frequency,
                             output->data[0].pedometer_t.step_length,
                             output->data[0].pedometer_t.step_type,
                             output->data[0].time_stamp
                            );
            #ifdef DUMP_RAW_DATA_IN_UART
            uint32_t buff[2];
            buff[0] = output->data[0].bio_data.data;
            buff[1] = output->data[0].time_stamp;
            sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_PEDOMETER, 0, (int32_t *)buff, 2);
            #endif
            send_sensor_data_via_btspp(
                SENSOR_SPP_DATA_MAGIC,
                SENSOR_TYPE_PEDOMETER,
                output->data[0].pedometer_t.accumulated_step_count,
                output->data[0].pedometer_t.step_type,
                output->data[0].time_stamp,
                SENSOR_SPP_DATA_RESERVED,
                SENSOR_SPP_DATA_RESERVED);

#ifdef GT_PROJECT_ENABLE

            if (queAppMgr != NULL) {
                outEvent.event = EVENT_UPDATE_PEDO;
                outEvent.userdata = &output->data[0].pedometer_t;
                xQueueSend(queAppMgr, &outEvent, 0);
            }

#endif


            break;
        case SENSOR_TYPE_SLEEP:
        case SENSOR_TYPE_SLEEP_STAGING:
            if (output->data[0].sleep_data_t.feature == NULL) {
                /* sleep state output */
                if (output->data_exist_count > 0) {
                    SENSOR_DEMO_LOGI("sleep tracker notification (%d).\n", output->data_exist_count);

                    i = 0;
                    while (i < output->data_exist_count) {
                      #ifdef FUSION_SLEEP_STAGING_USE
                        if (output->data[i].sleep_data_t.state == INVOKE_STAGING) {
                        	// unsubscribe tracker
                        	disable_sleep_tracker();
                        	// subscribe stager
                                enable_sleep_staging();

                        } else if (output->data[i].sleep_data_t.state == INVOKE_TRACKER) {
                        	// unsubscribe stager
                        	disable_sleep_staging();
                        	// subscribe tracker
                        	enable_sleep_tracker();
                        } else
                      #endif
                        {
                            /* transmit or save sleep states*/
                            asleep_tracker_data[sleep_tracker_buff_count] = output->data[i].sleep_data_t.state;
                            asleep_tracker_data[sleep_tracker_buff_count + 1] = output->data[i].time_stamp;
                            sleep_tracker_buff_count += 2;

                            if (sleep_tracker_buff_count == 12) {
                                sleep_tracker_buff_count = 0;
                                #ifdef DUMP_RAW_DATA_IN_UART
                                sensor_dump_raw_data_in_bt_log_format(output->data[0].sensor_type, sleep_tracker_seq, (int32_t *)asleep_tracker_data, 12);
                                #endif
                                SENSOR_DEMO_LOGI("sleep tracker. %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", asleep_tracker_data[0],
                                                 asleep_tracker_data[1], asleep_tracker_data[2], asleep_tracker_data[3], asleep_tracker_data[4], asleep_tracker_data[5], asleep_tracker_data[6],
                                                 asleep_tracker_data[7], asleep_tracker_data[8], asleep_tracker_data[9], asleep_tracker_data[10], asleep_tracker_data[11]);

                                send_sensor_16_datas_via_btspp(
                                    SENSOR_SPP_DATA_MAGIC,
                                    (output->data[0].sensor_type),
                                    sleep_tracker_seq,
                                    asleep_tracker_data,
                                    12,
                                    SENSOR_SPP_DATA_RESERVED);
                                sleep_tracker_seq++;
                            }
                        }
                        i++;
                    }
                }
            } else {
                /* sleep feature output */
                sleep_feature_data[0] = output->data[0].sleep_data_t.feature_type; /* feature type*/
                sleep_feature_data[1] = 0; /* status, always == 0 */
                sleep_feature_data[2] = output->data[0].time_stamp; /* timestamp */
                bunch_count = output->data[0].sleep_data_t.feature_num;
                if (bunch_count > 9) {
                    bunch_count = 9; /* maximum data  to transmit is 9 */
                }
                i = 0;
                while (i < bunch_count) {
                    sleep_feature_data[3 + i] = output->data[0].sleep_data_t.feature[i];
                    i++;
                }
                SENSOR_DEMO_LOGI("sleep feature (%d) %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", output->data[0].sleep_data_t.feature_num, sleep_feature_data[0],
                                 sleep_feature_data[1], sleep_feature_data[2], sleep_feature_data[3], sleep_feature_data[4], sleep_feature_data[5], sleep_feature_data[6],
                                 sleep_feature_data[7], sleep_feature_data[8], sleep_feature_data[9], sleep_feature_data[10], sleep_feature_data[11]);
                send_sensor_16_datas_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_INTERNAL_SLEEP_FEATURE,
                    sleep_feature_seq,
                    sleep_feature_data,
                    12,
                    SENSOR_SPP_DATA_RESERVED);
                sleep_feature_seq++;
            }
            break;
        case SENSOR_TYPE_HEART_RATE_MONITOR:
#if 0
            SENSOR_DEMO_LOGI("Heart rate , bpm = %ld , timestampe = %lu \r\n",
                             output->data[0].heart_rate_t.bpm,
                             output->data[0].time_stamp);
#endif

#ifdef GT_PROJECT_ENABLE


            if (queAppMgr != NULL) {
                outEvent.event = EVENT_UPDATE_HR;
                outEvent.userdata = &output->data[0].heart_rate_t;
                xQueueSend(queAppMgr, &outEvent, 0);
            }


#endif
            #ifdef DUMP_RAW_DATA_IN_UART
            {
                uint32_t buff[3];
                buff[0] = output->data[0].heart_rate_t.bpm;
                buff[1] = output->data[0].heart_rate_t.status;
                buff[2] = output->data[0].time_stamp;
                sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_HEART_RATE_MONITOR, 0, (int32_t *)buff, 2);
            }
            #endif
            send_sensor_data_via_btspp(
                SENSOR_SPP_DATA_MAGIC,
                SENSOR_TYPE_HEART_RATE_MONITOR,
                output->data[0].heart_rate_t.bpm,
                output->data[0].heart_rate_t.status,
                output->data[0].time_stamp,
                SENSOR_SPP_DATA_RESERVED,
                SENSOR_SPP_DATA_RESERVED);

	    #ifdef AUTOMATIC_MOTION_DETECT
	    if ((output->data[0].heart_rate_t.status&0x000000FF) == 0xFF){
		 watch_off_hr_count++;	
		 if(watch_off_hr_count >= WATCH_OFF_HR_CHECK_COUNT_MAX) {
                    watch_off_hr_count = 0;				   
	            enable_hr();
	            vTaskDelay(1000);
	            enable_motion_detect();	   
                 }		
	    }else {
		 watch_off_hr_count = 0;
	    }
	    #endif	
				
#ifdef DTP_ENABLE
            heart_rate_10s_data[7 + heart_data_counter] = (uint8_t)(output->data[0].heart_rate_t.bpm);
            heart_data_counter++;
            if (heart_data_counter == 10) {
                heart_rate_10s_data[0] = 1;
                heart_rate_10s_data[1] = 17;
                heart_rate_10s_data[6] = 1;
                /* send the 10s data */
                SENSOR_DEMO_LOGI("dtp_enabled = %d, dtp_sent_done = %d\n", dtp_enabled, dtp_sent_done);
                if (dtp_enabled && dtp_sent_done) {
                    ble_dtp_send_data(17, heart_rate_10s_data);
                    dtp_sent_done = false;
                }
                SENSOR_DEMO_LOGI("HR: ble_dtp_send_data %ld\r\n", heart_data_counter);
                heart_data_counter = 0;
            }
#endif /*DTP_ENABLE*/

            break;

        case SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR: {
            if (output->data[0].heart_rate_variability_t.beatPos == NULL) {
#ifdef GT_PROJECT_ENABLE
                if (queAppMgr != NULL) {
                    outEvent.event = EVENT_UPDATE_HRV;
                    outEvent.userdata = &output->data[0].heart_rate_variability_t;
                    xQueueSend(queAppMgr, &outEvent, 0);
                }
#endif
                SENSOR_DEMO_LOGI("Heart rate variability: T(%ld), SDNN(%ld), LF(%ld), HF(%ld), LF_HF(%ld)\r\n", output->data[0].time_stamp, output->data[0].heart_rate_variability_t.SDNN,
                                 output->data[0].heart_rate_variability_t.LF, output->data[0].heart_rate_variability_t.HF, output->data[0].heart_rate_variability_t.LF_HF);
#ifdef DTP_ENABLE
                int32_t tLF = output->data[0].heart_rate_variability_t.LF;
                int32_t tHF = output->data[0].heart_rate_variability_t.HF;
                int32_t tLF_HF = output->data[0].heart_rate_variability_t.LF_HF;
                int32_t tSDNN = output->data[0].heart_rate_variability_t.SDNN;
                heart_rate_variability_data[0] = 4;
                heart_rate_variability_data[1] = 23;
                heart_rate_variability_data[6] = 1;
                heart_rate_variability_data[7] = tLF & 0xFF;
                heart_rate_variability_data[8] = (tLF & 0xFF00) >> 8;
                heart_rate_variability_data[9] = (tLF & 0xFF0000) >> 16;
                heart_rate_variability_data[10] = (tLF & 0xFF000000) >> 24;;
                heart_rate_variability_data[11] = tHF & 0xFF;
                heart_rate_variability_data[12] = (tHF & 0xFF00) >> 8;
                heart_rate_variability_data[13] = (tHF & 0xFF0000) >> 16;
                heart_rate_variability_data[14] = (tHF & 0xFF000000) >> 24;
                heart_rate_variability_data[15] = tLF_HF & 0xFF;
                heart_rate_variability_data[16] = (tLF_HF & 0xFF00) >> 8;
                heart_rate_variability_data[17] = (tLF_HF & 0xFF0000) >> 16;
                heart_rate_variability_data[18] = (tLF_HF & 0xFF000000) >> 24;
                heart_rate_variability_data[19] = tSDNN & 0xFF;
                heart_rate_variability_data[20] = (tSDNN & 0xFF00) >> 8;
                heart_rate_variability_data[21] = (tSDNN & 0xFF0000) >> 16;
                heart_rate_variability_data[22] = (tSDNN & 0xFF000000) >> 24;

                LOG_I(hal, "HRV: dtp_enabled = %d, dtp_sent_done = %d\n", dtp_enabled, dtp_sent_done);
                if (dtp_enabled && dtp_sent_done) {
                    ble_dtp_send_data(23, heart_rate_variability_data);
                    dtp_sent_done = false;
                }
#endif /*DTP_ENABLE*/
                #ifdef DUMP_RAW_DATA_IN_UART
                {
                    uint32_t buff[5];
                    buff[0] = output->data[0].heart_rate_variability_t.SDNN;
                    buff[1] = output->data[0].heart_rate_variability_t.LF;
                    buff[2] = output->data[0].heart_rate_variability_t.HF;
                    buff[3] = output->data[0].heart_rate_variability_t.LF_HF;
                    buff[4] = output->data[0].time_stamp;
                    sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR, 0, (int32_t *)buff, 2);
                }
                #endif
                send_sensor_data_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR,
                    output->data[0].heart_rate_variability_t.SDNN,
                    output->data[0].heart_rate_variability_t.LF,
                    output->data[0].heart_rate_variability_t.HF,
                    output->data[0].heart_rate_variability_t.LF_HF,
                    output->data[0].time_stamp
                );
                /* To unsubscribe here? */
#ifdef FUSION_HEART_RATE_VARIABILITY_USE
                int32_t unsubscription;
                hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_LOW);
                hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
                /* HRV disable indication*/
                algorithm_on_indication(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR, 0);
                unsubscription = sensor_unsubscribe_sensor(heart_rate_variability_subscriber.handle);
                if (unsubscription < 0) {
                    SENSOR_DEMO_LOGI("HRV unsubscription fail\r\n");
                } else {
                    hrv_triggered = 0;
                    SENSOR_DEMO_LOGI("HRV unsubscribed\r\n");
                }
                xTimerChangePeriod(xtimer_ui_main, 5 * 1000 / portTICK_PERIOD_MS, 1000 / portTICK_PERIOD_MS);
                xTimerReset(xtimer_ui_main, 1000 / portTICK_PERIOD_MS);
#endif
            } else {/* beat position data*/
                memset(abeatpos_data, 0, sizeof(int32_t));
                abeatpos_data[0] = output->data[0].heart_rate_variability_t.feature_type; /* feature type*/
                abeatpos_data[1] = output->data[0].heart_rate_variability_t.status; /* status*/
                abeatpos_data[2] = output->data[0].time_stamp; /* timestamp */
                bunch_count = output->data[0].heart_rate_variability_t.numBeatPos;
                if (bunch_count > 9) {
                    bunch_count = 9; /* maximum data  to transmit is 9 */
                }
                i = 0;
                while (i < bunch_count) {
                    abeatpos_data[3 + i] = output->data[0].heart_rate_variability_t.beatPos[i];
                    i++;
                }
                SENSOR_DEMO_LOGI("beat pos (%d) in HRV %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", output->data[0].heart_rate_variability_t.numBeatPos, abeatpos_data[0],
                                 abeatpos_data[1], abeatpos_data[2], abeatpos_data[3], abeatpos_data[4], abeatpos_data[5], abeatpos_data[6],
                                 abeatpos_data[7], abeatpos_data[8], abeatpos_data[9], abeatpos_data[10], abeatpos_data[11]);
                #ifdef DUMP_RAW_DATA_IN_UART
                sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_INTERNAL_BEATPOS, beatpos_seq, (int32_t *)abeatpos_data, 12);
                #endif
                send_sensor_16_datas_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_INTERNAL_BEATPOS,
                    beatpos_seq,
                    abeatpos_data,
                    12,
                    SENSOR_SPP_DATA_RESERVED);
                beatpos_seq++;
            }
            break;
        }

        case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR: {
            if (output->data[0].blood_pressure_t.pwtt == NULL) {
#ifdef GT_PROJECT_ENABLE
                if (queAppMgr != NULL) {
                    outEvent.event = EVENT_UPDATE_BP;
                    outEvent.userdata = &output->data[0].blood_pressure_t;
                    xQueueSend(queAppMgr, &outEvent, 0);
                }
#endif

#if defined(MTK_SENSOR_BIO_USE_MT2511)
                uint32_t agc_read_reg_tx;
                uint32_t agc_read_reg_rx;
                bus_data_t ppg_reg_info;

                ppg_reg_info.addr = 0x33;
                ppg_reg_info.reg  = 0x2C;
                ppg_reg_info.data_buf = (uint8_t *)&agc_read_reg_tx;
                ppg_reg_info.length = sizeof(agc_read_reg_tx);
                vsm_driver_read_register(&ppg_reg_info);

                ppg_reg_info.reg  = 0x18;
                ppg_reg_info.data_buf = (uint8_t *)&agc_read_reg_rx;
                ppg_reg_info.length = sizeof(agc_read_reg_rx);
                vsm_driver_read_register(&ppg_reg_info);
                #if defined(DUMP_RAW_DATA_IN_UART) || defined(DUMP_BP_DATA_IN_UART)
                {
                    uint32_t buff[4];
                    buff[0] = (agc_read_reg_tx & 0xFF);
                    buff[1] = ((agc_read_reg_rx & 0x00000038)>>3);
                    buff[2] = ((agc_read_reg_rx >> 22) & 0x00000007);
                    buff[3] = ((agc_read_reg_rx >> 25) & 0x00000007);
                    #ifdef DUMP_RAW_DATA_IN_UART
                    sensor_dump_raw_data_in_bt_log_format(8002, 0, (int32_t *)buff, 4);
                    #endif

                    #ifdef DUMP_BP_DATA_IN_UART
                    int32_t j = 0;
                    bp_pwtt_data_array[bp_pwtt_cnt][0] = SENSOR_SPP_DATA_MAGIC;
                    bp_pwtt_data_array[bp_pwtt_cnt][1] = SENSOR_TYPE_AGC_CONFIG;
                    bp_pwtt_data_array[bp_pwtt_cnt][2] = 0;
                    for (j = 0; j < 4; j++) {
                        bp_pwtt_data_array[bp_pwtt_cnt][j+3] = buff[j];
                    }
                    bp_pwtt_cnt++;
                    #endif
                }
                #endif

                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            SENSOR_TYPE_AGC_CONFIG,
                            (agc_read_reg_tx & 0xFF),
                            ((agc_read_reg_rx & 0x00000038)>>3),
                            ((agc_read_reg_rx >> 22) & 0x00000007),
                            ((agc_read_reg_rx >> 25) & 0x00000007),
                            SENSOR_SPP_DATA_RESERVED);
#endif

                SENSOR_DEMO_LOGI("Blood pressure sbp(%ld), dbp(%ld), status(%ld) \r\n", output->data[0].blood_pressure_t.sbp,
                                 output->data[0].blood_pressure_t.dbp, output->data[0].blood_pressure_t.status);

                {
                    bpm_buff_data_array[2] = output->data[0].blood_pressure_t.sbp;
                    bpm_buff_data_array[3] = output->data[0].blood_pressure_t.dbp;
                    bpm_buff_data_array[4] = output->data[0].blood_pressure_t.bpm;
                    bpm_buff_data_array[5] = output->data[0].blood_pressure_t.status;
                    bpm_buff_data_array[6] = output->data[0].time_stamp;
                #ifdef DUMP_RAW_DATA_IN_UART
                    sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 0, (int32_t *)bpm_buff_data_array, 12);
                #endif
                }
                LOG_I(sensor,"start sending bp off indication and type 24\n");
                /* BP disable indication*/
                algorithm_on_indication(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 0);

                send_sensor_data_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_BLOOD_PRESSURE_MONITOR,
                    output->data[0].blood_pressure_t.sbp,
                    output->data[0].blood_pressure_t.dbp,
                    output->data[0].blood_pressure_t.bpm,
                    output->data[0].blood_pressure_t.status,
                    output->data[0].time_stamp
                );

#ifdef FUSION_BLOOD_PRESSURE_USE
                uint8_t buf[11];
                uint32_t buf_size;
                #define BP_CALIBRATION_MODE 1023

                nvdm_status_t nvdm_status;

                buf_size = sizeof(buf);
                nvdm_status = nvdm_read_data_item("2511","mode",(uint8_t*)buf,&buf_size);
                if(nvdm_status != NVDM_STATUS_OK)
                {
                    LOG_I(hal,"read nvdm mode fail\n");
                }
                else
                {
                    LOG_I(hal, "mode %s\n", buf);
                    if (atoi((char const *)buf) == BP_CALIBRATION_MODE)
                        nvdm_write_data_item("2511", "mode", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)"0", 4);
                }


                int32_t unsubscription;
                hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_LOW);
                hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
                
                unsubscription = sensor_unsubscribe_sensor(blood_pressure_subscriber.handle);
                if (unsubscription < 0) {
                    SENSOR_DEMO_LOGI("B.P unsubscription fail\r\n");
                } else {
                    bp_triggered = 0;
                    SENSOR_DEMO_LOGI("B.P. unsubscribed\r\n");
                    #if defined(DUMP_BP_DATA_IN_UART)
                    xTaskCreate(sensor_dump_bp_TimerCallback, "sensor_dump_bp", 2048 / sizeof(StackType_t), NULL, TASK_PRIORITY_NORMAL, NULL);
                    #endif
                }
                xTimerChangePeriod(xtimer_ui_main, 5 * 1000 / portTICK_PERIOD_MS, 1000 / portTICK_PERIOD_MS);
                xTimerReset(xtimer_ui_main, 1000 / portTICK_PERIOD_MS);
#endif
            } else { /* pwtt data*/
                memset(apwtt_data, 0, sizeof(int32_t));
                //apwtt_data[0] = output->data[0].blood_pressure_t.feature_type; /* feature type*/
                #if BP_CAL_TEST
                //apwtt_data[0] = output->data[0].blood_pressure_t.pwtt[9]; //20161125 struct no feature_type, use pwtt[9] to replace
				#endif
				apwtt_data[0] = output->data[0].blood_pressure_t.feature_type;
                apwtt_data[1] = output->data[0].blood_pressure_t.status; /* status*/
                apwtt_data[2] = output->data[0].time_stamp; /* timestamp */
                bunch_count = output->data[0].blood_pressure_t.numPwtt;
                if (bunch_count > 9) {
                    bunch_count = 9; /* maximum data  to transmit is 9 */
                }
                i = 0;
                while (i < bunch_count) {
                    apwtt_data[3 + i] = output->data[0].blood_pressure_t.pwtt[i];
                    i++;
                }
                #ifdef DUMP_RAW_DATA_IN_UART
                sensor_dump_raw_data_in_bt_log_format(SENSOR_TYPE_INTERNAL_PWTT, pwtt_seq, (int32_t *)apwtt_data, 12);
                #endif
                SENSOR_DEMO_LOGI("pwtt (%d) in B.P. %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", output->data[0].blood_pressure_t.numPwtt, apwtt_data[0],
                                 apwtt_data[1], apwtt_data[2], apwtt_data[3], apwtt_data[4], apwtt_data[5], apwtt_data[6],
                                 apwtt_data[7], apwtt_data[8], apwtt_data[9], apwtt_data[10], apwtt_data[11]);
                
                #ifdef DUMP_BP_DATA_IN_UART
                int32_t j = 0;
                bp_pwtt_data_array[bp_pwtt_cnt][0] = SENSOR_SPP_DATA_MAGIC;
                bp_pwtt_data_array[bp_pwtt_cnt][1] = SENSOR_TYPE_INTERNAL_PWTT;
                bp_pwtt_data_array[bp_pwtt_cnt][2] = pwtt_seq;
                for (j = 0; j < 12; j++) {
                    bp_pwtt_data_array[bp_pwtt_cnt][j+3] = apwtt_data[j];
                }
                bp_pwtt_cnt++;
                #endif

                send_sensor_16_datas_via_btspp(
                    SENSOR_SPP_DATA_MAGIC,
                    SENSOR_TYPE_INTERNAL_PWTT,
                    pwtt_seq,
                    apwtt_data,
                    12,
                    SENSOR_SPP_DATA_RESERVED);
                pwtt_seq++;
            }
            break;
        }

        default:
            SENSOR_DEMO_LOGI("type = %lu,  timestamp = %lu \r\n",
                             output->data[0].sensor_type,
                             output->data[0].time_stamp);
            break;
    }


    //TODO collect sensor data and send to Android device via BT
    //...

    return 0;
}

void enable_all_sensors(void)
{
    sensor_manager_init();

    int32_t subscription;

    (void)subscription;

#if defined(FUSION_HEART_RATE_MONITOR_USE) && !defined(GT_PROJECT_ENABLE)
    subscription = sensor_subscribe_sensor(&heart_rate_monitor_subscriber);
    if (subscription < 0) {
        SENSOR_DEMO_LOGI("Heart rate monitor subscription fail\r\b");
    }
#endif

#if defined(FUSION_PEDOMETER_USE) && !defined(GT_PROJECT_ENABLE)
    subscription = sensor_subscribe_sensor(&pedometer_subscriber);
    if (subscription < 0) {
        SENSOR_DEMO_LOGI("Pedometer subscription fail\r\b");
    }
#endif

#if defined(FUSION_SLEEP_TRACKER_USE) && !defined(GT_PROJECT_ENABLE)
    subscription = sensor_subscribe_sensor(&sleep_subscriber);
    if (subscription < 0) {
        SENSOR_DEMO_LOGI("Sleep subscription fail\r\b");
    }
#endif

}

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
void enable_hrv(void)
{
    int32_t subscription = 0;

    if (hrv_triggered == 0) {

        hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
        vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);

        hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
        hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_HIGH);
        /* HRV enable indication*/
        algorithm_on_indication(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR, 1);
        subscription = sensor_subscribe_sensor(&heart_rate_variability_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("Heart rate variability subscription fail\r\n");
        } else {
            SENSOR_DEMO_LOGI("Heart rate variability triggered\r\n");
            hrv_triggered = 1;
        }
    }
}
#endif /*FUSION_HEART_RATE_VARIABILITY_USE*/

#ifdef FUSION_BLOOD_PRESSURE_USE
void enable_bp(void)
{
    int32_t subscription = 0;

    if (bp_triggered == 0) {

        hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
        vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);

        hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
        hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_HIGH);
        /* BP enable indication*/
        algorithm_on_indication(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 1);
        subscription = sensor_subscribe_sensor(&blood_pressure_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("B.P. subscription fail\r\n");
        } else {
            /* reset bt buffer index of input start */
            ekg_buff_count = 0;
            ppg1_buff_count = 0;
            pwtt_seq = 0;
#ifdef DUMP_BP_DATA_IN_UART
            bp_pwtt_cnt = 0;
            bp_print_idx = 0;
#endif
            /* reset bt buffer index of input end */
            SENSOR_DEMO_LOGI("B.P. triggered\r\n");
            bp_triggered = 1;
        }
    }
}
#endif

#ifdef FUSION_HEART_RATE_MONITOR_USE
void enable_hr(void)
{
    int32_t subscription = 0;
    int32_t unsubscription = 0;

    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
    vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
    if (hr_triggered == 0) {
        /* HR enable indication*/
        algorithm_on_indication(SENSOR_TYPE_HEART_RATE_MONITOR, 1);
        subscription = sensor_subscribe_sensor(&heart_rate_monitor_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("Heart rate monitor subscription fail\r\n");
        } else {
            /* reset bt buffer index of input start */
            acc_buff_count = 0;
            ppg1_buff_count = 0;
            /* reset bt buffer index of input end */
            /* reset dtp buffer index start*/
        #ifdef DTP_ENABLE
            heart_data_counter = 0;
        #endif
            /* reset dtp buffer index end*/

            SENSOR_DEMO_LOGI("Heart rate monitor triggered\r\n");
            hr_triggered = 1;
        }
    } else {
        /* HR disable indication*/
        algorithm_on_indication(SENSOR_TYPE_HEART_RATE_MONITOR, 0);
        unsubscription = sensor_unsubscribe_sensor(heart_rate_monitor_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("HR unsubscription fail\r\n");
        } else {
            hr_triggered = 0;
            SENSOR_DEMO_LOGI("HR unsubscribed\r\n");
        }
    }
}
#endif /*FUSION_HEART_RATE_MONITOR_USE*/

#ifdef FUSION_SLEEP_TRACKER_USE
void app_enable_sleep_tracker(void)
{
    int32_t subscription = 0;

    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
    vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
    if (sleep_triggered == 0) {
        /* sleep enable indication*/
        algorithm_on_indication(SENSOR_TYPE_SLEEP_STAGING, 1);
        subscription = sensor_subscribe_sensor(&sleep_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("app S.T. subscription fail\r\n");
        } else {
            /* reset bt buffer index of input start */
            acc_buff_count = 0;
            ppg1_buff_count = 0;
            /* reset bt buffer index of input end */

            SENSOR_DEMO_LOGI("app S.T. triggered\r\n");
            sleep_tracker_seq = 0;
            sleep_tracker_buff_count = 0;
            sleep_triggered = 1;
            st_triggered = 1;
        }
    } else {
        /* sleep disable indication*/
        algorithm_on_indication(SENSOR_TYPE_SLEEP_STAGING, 0);
        SENSOR_DEMO_LOGI("app disable sleep \r\n");
        disable_sleep_staging();
        disable_sleep_tracker();
        sleep_triggered = 0;
    }
}

void enable_sleep_tracker(void)
{
    int32_t subscription = 0;

    if (st_triggered == 0) {
        subscription = sensor_subscribe_sensor(&sleep_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("S.T. subscription fail\r\n");
        } else {
            SENSOR_DEMO_LOGI("S.T. triggered\r\n");
            st_triggered = 1;
        }
    }
}

void disable_sleep_tracker(void)
{
    int32_t unsubscription;

    if (st_triggered > 0) {
        unsubscription = sensor_unsubscribe_sensor(sleep_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("S.T. unsubscription fail\r\n");
        } else {
            st_triggered = 0;
            SENSOR_DEMO_LOGI("S.T. unsubscribed\r\n");
        }
    }
}
#else
void enable_sleep_tracker(void)
{
}

void disable_sleep_tracker(void)
{
}
#endif /*FUSION_SLEEP_TRACKER_USE*/

#ifdef FUSION_SLEEP_STAGING_USE
void enable_sleep_staging(void)
{
    int32_t subscription = 0;

    subscription = sensor_subscribe_sensor(&sleep_staging_subscriber);
    if (subscription < 0) {
        SENSOR_DEMO_LOGI("Sleep staging subscription fail\r\n");
    } else {
        SENSOR_DEMO_LOGI("Sleep staging triggered\r\n");
        sleep_staging_triggered = 1;
    }
}

void disable_sleep_staging(void)
{
    int32_t unsubscription = 0;

    if (sleep_staging_triggered > 0) {
        unsubscription = sensor_unsubscribe_sensor(sleep_staging_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("Sleep staging unsubscription fail\r\n");
        } else {
            sleep_staging_triggered = 0;
            SENSOR_DEMO_LOGI("Sleep staging unsubscribed\r\n");
        }
    }
}

#else
void enable_sleep_staging(void)
{
}

void disable_sleep_staging(void)
{
}

#endif

#ifdef FUSION_PEDOMETER_USE
void enable_pedometer(void)
{
    int32_t subscription = 0;
    int32_t unsubscription = 0;

    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
    vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
    if (pedo_triggered == 0) {
        subscription = sensor_subscribe_sensor(&pedometer_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("PEDO subscribe fail\r\n");
        } else {
            /* reset bt buffer index of input start */
            acc_buff_count = 0;
            /* reset bt buffer index of input end */

            SENSOR_DEMO_LOGI("pedo triggered\r\n");
            pedo_triggered = 1;
        }
    } else {

        unsubscription = sensor_unsubscribe_sensor(pedometer_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("PEDO unsubscription fail\r\n");
        } else {
            pedo_triggered = 0;
            SENSOR_DEMO_LOGI("PEDO unsubscribed\r\n");
        }
    }

}
#endif /*FUSION_PEDOMETER_USE*/

#ifdef AUTOMATIC_MOTION_DETECT

void enable_motion_detect(void)
{
    int32_t subscription = 0;
    int32_t unsubscription = 0;

    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
    vTaskDelay(3 * 1000 / portTICK_PERIOD_MS);

    hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
    if (motion_detect_triggered == 0) {
        subscription = sensor_subscribe_sensor(&motion_detect_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("motion_detect subscription fail\r\n");
        } else {
            // reset bt buffer index of input start 
            acc_buff_count = 0;
            // reset bt buffer index of input end 

            SENSOR_DEMO_LOGI("motion_detect triggered\r\n");
            motion_detect_triggered = 1;
        }
    } else {

        unsubscription = sensor_unsubscribe_sensor(motion_detect_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("motion_detect unsubscription fail\r\n");
        } else {
            motion_detect_triggered = 0;
            SENSOR_DEMO_LOGI("motion_detect unsubscribed\r\n");
        }
    }
} 

#else
void enable_motion_detect(void)
{
}

#endif /*AUTOMATIC_MOTION_DETECT*/

#endif /*SENSOR_DEMO*/

