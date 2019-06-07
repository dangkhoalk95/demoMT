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

#ifndef __SPEEX_CODEC_H__
#define __SPEEX_CODEC_H__

/**
 * @addtogroup Audio Decoder
 * @{
 * @addtogroup Common
 * @{
 * @addtogroup SPEEX codecr
 * @{
 * @section speex_codec_api_usage How to use this module
 *
 *
 *     @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


#define     SPEEX_CODEC_LOG_ENABLE
#define     MAX_SPEEX_CODEC_FUNCTIONS          5      /** The maximum number of callback function. */
#define     SPEEX_CODEC_QUEUE_SIZE             10      /** The maximum number of items the queue can hold at any one time.*/
#define     MAXIMUM(A,B)                  (((A)>(B))?(A):(B))
#define     MINIMUM(A,B)                  (((A)<(B))?(A):(B))


/**
 * @defgroup speex_codec_media_events_define Define
 * @{
 */

/** @brief Defines the SPEEX codec media event. */
#define SPEEX_CODEC_MEDIA_EVENT_START    (100)        /**< The start of the Media events. */

/** @brief SPEEX codec state */
typedef enum {
    SPEEX_CODEC_STATE_IDLE,                           /**< The SPEEX codec is inactive. */
    SPEEX_CODEC_STATE_READY,                          /**< The SPEEX codec is ready to decode or encode the media. */
    SPEEX_CODEC_STATE_DECODING,                       /**< The SPEEX codec is in a decoding state. */
    SPEEX_CODEC_STATE_ENCODING,                       /**< The SPEEX codec is in a encoding state. */
    SPEEX_CODEC_STATE_ERROR,                          /**< The SPEEX codec reported an error. */
    SPEEX_CODEC_STATE_STOP                            /**< The SPEEX codec has stopped. */
} speex_codec_state_t;


/** @brief This structure defines the SPEEX codec media events. */
typedef enum {
    SPEEX_CODEC_MEDIA_EVENT_ERROR = SPEEX_CODEC_MEDIA_EVENT_START,          /**< Failed, due to a bitstream error. */
    SPEEX_CODEC_MEDIA_EVENT_OK,                                             /**< Successful. */
    SPEEX_CODEC_MEDIA_EVENT_UNDERFLOW,                                      /**< The bitstreaming is underflow. */
    SPEEX_CODEC_MEDIA_EVENT_REQUEST,                                        /**< Bitstreaming request from the user. */
    SPEEX_CODEC_MEDIA_EVENT_DATA_NOTIFICATION                               /**< Bitstreaming notification to the user. */
} speex_codec_media_event_t;


/** @brief This structure defines the SPEEX codec API return status. */
typedef enum {
    SPEEX_CODEC_STATUS_BASE = -0x2B00,                     /**< The base of the status. */
    SPEEX_CODEC_STATUS_OK = 0,                             /**< Successful. */
    SPEEX_CODEC_STATUS_ERROR = -0x2B01,                    /**< Error. */
    SPEEX_CODEC_STATUS_INVALID_PARAM = -0x2B02             /**< Invalid parameters. */
} speex_codec_status_t;


/** @brief This structure defines the SPEEX codec  operating option. */
typedef enum {
    SPEEX_CODEC_NB_MODE = 0,       /**< Narrowband mode. */
    SPEEX_CODEC_WB_MODE  = 1       /**< Wideband mode. */
} speex_codec_mode_t;

/** @brief This enum defines the SPEEX codec channel number.  */
typedef enum {
    SPEEX_CODEC_MONO = 0,        /**<  A single channel.  */
    SPEEX_CODEC_STEREO  = 1      /**<  Two channels. (invalid)*/
} speex_codec_channer_number_t;

/** @brief This enum defines the SPEEX codec csampling rate.  */
typedef enum {
    SPEEX_CODEC_SAMPLE_RATE_8K        = 0,  /**<  8000Hz  */
    SPEEX_CODEC_SAMPLE_RATE_16K       = 1,  /**<  16000Hz */
    SPEEX_CODEC_SAMPLE_RATE_32K       = 2  /**<  32000Hz */
} speex_codec_sample_rate_t;



/**
 * @}
 */


/** @defgroup speex_codec_struct Struct
  * @{
  */

/** @brief This structure defines the ring buffer structure. */
typedef struct {
    uint8_t         *buffer_base;          /**< Pointer to the ring buffer. */
    int32_t          buffer_size;          /**< Size of the ring buffer. (unit: Byte)*/
    int32_t          write;                /**< Index of the ring buffer to write the data. */
    int32_t          read;                 /**< Index of the ring buffer to read the data. */
    void            *param;                /**< Extended parameter or information. */
} speex_codec_share_buffer_t;


/**
 *  @brief This structure defines the SPEEX codec configuration.
 */
typedef struct {
    speex_codec_mode_t  speex_codec_mode;                       /**< SPEEX codec mode. */
    speex_codec_channer_number_t speex_codec_channer_number;    /**< SPEEX codec channel number. */
    speex_codec_sample_rate_t speex_codec_sample_rate;          /**< SPEEX codec sampling rate.*/
} speex_codec_config_t;


/** @brief speex codec handle structure type.*/
typedef struct _speex_codec_media_handle_t {
    speex_codec_state_t               codec_state;              /**< The SPEEX codec state. */
    speex_codec_config_t              codec_config;             /**< The SPEEX codec configuration. */
    speex_codec_share_buffer_t        share_buffer;         /**< The bitstream buffer information for the SPEEX codec. */
    bool                              underflow;                /**< The SPEEX codec data underflow occurred. */
    bool                              waiting;                  /**< The SPEEX codec is waiting to fill data. */
    uint16_t                          audio_id;                 /**< The audio ID of the SPEEX codec. */

    void (*handler)(struct _speex_codec_media_handle_t *handle, speex_codec_media_event_t event_id);                      /**< The SPEEX codec handler. */
    void (*set_share_buffer)(struct _speex_codec_media_handle_t *handle, uint8_t  *buffer, uint32_t  length);           /**< Set the shared buffer for bitstream. The audio streaming data can be filled through a shared buffer.*/
    void (*get_write_buffer)(struct _speex_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length);               /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*get_read_buffer)(struct _speex_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length);                /**< Get the available length to read from a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(struct _speex_codec_media_handle_t *handle, uint32_t length);                                   /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(struct _speex_codec_media_handle_t *handle);                                                  /**< Indicate the last data transfer. */
    void (*reset_bitstream_buffer)(struct _speex_codec_media_handle_t *handle);                                             /**< Reset the bitstream buffer's information. */
    void (*read_data_done)(struct _speex_codec_media_handle_t *handle, uint32_t length);                                    /**< Update the read pointer to the shared buffer. */
    int32_t (*get_free_space)(struct _speex_codec_media_handle_t *handle);                                                  /**< Get the free space length available in the shared buffer. */
    int32_t (*get_data_count)(struct _speex_codec_media_handle_t *handle);                                                  /**< Get the available data amount of the shared buffer. */

    //speex_codec_status_t (*encode)(struct _speex_codec_media_handle_t *handle);                                             /**< The SPEEX codec encode function. */
    speex_codec_status_t (*decode)(struct _speex_codec_media_handle_t *handle);                                             /**< The SPEEX codec decode function. */
    speex_codec_status_t (*stop)(struct _speex_codec_media_handle_t *handle);                                               /**< The AAC decoder stop function. */
} speex_codec_media_handle_t;


/** @brief  This defines the SPEEX codec callback function prototype.
 *          The user should register a callback function while opening the SPEEX codec. Please refer to #speex_codec_open().
 *  @param[in] handle is the media handle of the SPEEX codec.
 *  @param[in] event_id is the value defined in #speex_codec_media_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*speex_codec_callback_t)(speex_codec_media_handle_t *handle, speex_codec_media_event_t event_id);


/**
  * @}
  */

/**
 * @brief     This function opens the SPEEX codec.
 * @param[in] callback is a pointer to the callback function for the data control.
 * @param[in] config is the configuration of the SPEEX codec.  If the config is NULL,  SPEEX codec will use the default setting values of #speex_codec_config_t.
 * @return    Media handle of the SPEEX codec. Please refer to #speex_codec_media_handle_t.
 */
speex_codec_media_handle_t *speex_codec_open(speex_codec_callback_t callback, const speex_codec_config_t config);

/**
 * @brief     This function closes the SPEEX codec.
 * @param[in] handle is the SPEEX codec handle.
 * @return    If the operation completed successfully, the return value is #SPEEX_CODEC_STATUS_OK, otherwise the return value is #SPEEX_CODEC_STATUS_ERROR.
 */
speex_codec_status_t speex_codec_close(speex_codec_media_handle_t *handle);



#ifdef __cplusplus
}
#endif

/**
* @}
* @}
* @}
*/

#endif /* __SPEEX_CODEC_H__ */

