/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
 
#ifndef MESH_OS_LAYER_API_H
#define MESH_OS_LAYER_API_H
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    uint8_t *buffer;	/**< Buffer pointer. */
    uint32_t length;  /**< Buffer length. */
} bt_mesh_os_layer_aes_buffer_t;


/**
 * @brief     Timer callback function pointer.
 * @return    None.
 */
typedef void (*bt_mesh_os_layer_timer_expired_t)(void);

extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);

#define MESH_MUTEX_LOCK() bt_os_take_stack_mutex()
#define MESH_MUTEX_UNLOCK() bt_os_give_stack_mutex()


/*!
     @defgroup PAGE_API_LIBRARY_DLIST bt_mesh_os_layer_api.h
     @{
        @page PAGE_API_LIBRARY_DLIST bt_mesh_os_layer_api.h
        bt_mesh_os_layer_api.h is a double linked list management API set.

*/

/*!
    @brief Allocate a double-linked list
*/
void *bt_mesh_os_layer_ds_dlist_alloc(void);

/*!
    @brief Release a double-linked list
    @param list The double-linked list @ref bt_mesh_os_layer_ds_dlist_alloc
*/
void bt_mesh_os_layer_ds_dlist_free(void *list);

void bt_mesh_os_layer_ds_dlist_empty(void *list);

/*!
    @brief double-linked list entry and data memory allocation
    @param size data memory size
*/
void *bt_mesh_os_layer_ds_dlist_entry_alloc(uint32_t size);

/*!
    @brief Release a double-linked list enrty and data memory
    @param dat The double-linked list entry @ref bt_mesh_os_layer_ds_dlist_entry_alloc
*/
void bt_mesh_os_layer_ds_dlist_entry_free(void *dat);

void *bt_mesh_os_layer_ds_dlist_remove_head(void *list);

void *bt_mesh_os_layer_ds_dlist_remove(void *list, void *removed);

bool bt_mesh_os_layer_ds_dlist_delete(void *list, void *deleted);

/*!
    @brief Insert a double-linked list enrty into a double-linked list
    @param list The double-linked list @ref bt_mesh_os_layer_ds_queue_alloc
    @param inserted The double-linked list entry @ref bt_mesh_os_layer_ds_dlist_entry_alloc want to be inserted
*/
void bt_mesh_os_layer_ds_dlist_insert(void *list, void *inserted);

/*!
    @brief Insert a double-linked list enrty after a specified double-linked list entry
    @param list The double-linked list @ref bt_mesh_os_layer_ds_dlist_alloc
    @param inlist the target double-linked list entry
    @param inserted The double-linked list entry @ref bt_mesh_os_layer_ds_dlist_entry_alloc want to be inserted
*/
void bt_mesh_os_layer_ds_dlist_insert_after(void *list, void *inlist, void *inserted);

/*!
    @brief Insert a double-linked list enrty before a specified double-linked list entry
    @param list The double-linked list @ref bt_mesh_os_layer_ds_dlist_alloc
    @param inlist the target double-linked list entry
    @param inserted The double-linked list entry @ref bt_mesh_os_layer_ds_dlist_entry_alloc want to be inserted
*/
void bt_mesh_os_layer_ds_dlist_insert_before(void *list, void *inlist, void *inserted);

/*!
    @brief Query the first double-linked enrty from a double-linked
    @param queue The double-linked @ref bt_mesh_os_layer_ds_dlist_alloc
*/
void *bt_mesh_os_layer_ds_dlist_first(void *list);

/*!
    @brief Query the next double-linked list enrty from current double-linked list entry
    @param queue The double-linked list @ref bt_mesh_os_layer_ds_dlist_alloc
    @param dat The double-linked list entry @ref bt_mesh_os_layer_ds_dlist_entry_alloc
*/
void *bt_mesh_os_layer_ds_dlist_next(void *list, void *data);

/*!
    @brief Query the double-linked list entry of queue
    @param list The double-linked list @ref bt_mesh_os_layer_ds_dlist_alloc
    @return the elements count of double-linked list
*/
uint32_t bt_mesh_os_layer_ds_dlist_count(void *list);



/*!
     @defgroup PAGE_API_LIBRARY_QUEUE bt_mesh_os_layer_api.h
     @{
        @page PAGE_API_LIBRARY_QUEUE bt_mesh_os_layer_api.h
        bt_mesh_os_layer_api.h is a queue management API set.

*/

/*!
    @brief Allocate a queue
*/
void *bt_mesh_os_layer_ds_queue_alloc(void);

/*!
    @brief Release a queue
    @param queue The queue @ref bt_mesh_os_layer_ds_queue_alloc
*/
void bt_mesh_os_layer_ds_queue_free(void *queue);

/*!
    @brief Query entry and data memory allocation
    @param size data memory size
*/
void *bt_mesh_os_layer_ds_queue_entry_alloc(uint32_t size);

/*!
    @brief Release a queue enrty and data memory
    @param dat The queue @ref bt_mesh_os_layer_ds_queue_entry_alloc
*/
void bt_mesh_os_layer_ds_queue_entry_free(void *dat);

/*!
    @brief Push a queue enrty into a queue
    @param queue The queue @ref bt_mesh_os_layer_ds_queue_alloc
    @param dat The queue entry @ref bt_mesh_os_layer_ds_queue_entry_alloc
*/
void bt_mesh_os_layer_ds_queue_push(void *queue, void *dat);

/*!
    @brief Push a queue enrty into a queue front
    @param queue The queue @ref bt_mesh_os_layer_ds_queue_alloc
    @param dat The queue entry @ref bt_mesh_os_layer_ds_queue_entry_alloc
*/
void bt_mesh_os_layer_ds_queue_push_front(void *queue, void *dat);

/*!
    @brief Pop a queue enrty from a queue
    @param queue The queue @ref bt_mesh_os_layer_ds_queue_alloc
*/
void *bt_mesh_os_layer_ds_queue_pop(void *queue);

/*!
    @brief Query the first queue enrty from a queue
    @param queue The queue @ref bt_mesh_os_layer_ds_queue_alloc
*/
void *bt_mesh_os_layer_ds_queue_first(void *queue);

/*!
    @brief Query the next queue enrty from current queue entry
    @param queue The queue @ref bt_mesh_os_layer_ds_queue_alloc
    @param dat The queue entry @ref bt_mesh_os_layer_ds_queue_entry_alloc
*/
void *bt_mesh_os_layer_ds_queue_next(void *queue, void *dat);

/*!
    @brief Insert a queue enrty in current queue entry
    @param queue The queue @ref bt_mesh_os_layer_ds_queue_alloc
    @param inqueue The queue entry @ref bt_mesh_os_layer_ds_queue_entry_alloc
    @param inserted The queue entry @ref bt_mesh_os_layer_ds_queue_entry_alloc
*/
void bt_mesh_os_layer_ds_queue_insert_after(void *queue, void *inqueue, void *inserted);

/*!
    @brief Query the queue entry of queue
    @param queue The queue @ref bt_mesh_os_layer_ds_queue_alloc
*/
uint32_t bt_mesh_os_layer_ds_queue_count(void *queue);

void bt_mesh_os_layer_ds_queue_remove(void *queue, void *removed);

void bt_mesh_os_layer_get_random(uint8_t *rand, uint8_t len);

void bt_mesh_os_layer_exit_critical(void);

void bt_mesh_os_layer_enter_critical(void);

void bt_mesh_os_layer_disable_interrupt(void);
void bt_mesh_os_layer_enable_interrupt(void);

void bt_mesh_os_layer_register_timer_callback(bt_mesh_os_layer_timer_expired_t callback);
void bt_mesh_os_layer_init_timer(void);
void bt_mesh_os_layer_deinit_timer(void);
uint32_t bt_mesh_os_layer_create_semaphore(void);
void bt_mesh_os_layer_take_semaphore(uint32_t semaphore_id);
void bt_mesh_os_layer_give_semaphore(uint32_t semaphore_id);
void bt_mesh_os_layer_delete_semaphore(uint32_t semaphore_id);
uint32_t bt_mesh_os_layer_get_system_tick(void);
uint32_t bt_mesh_os_layer_ms_to_tick(uint32_t time_ms);
uint32_t bt_mesh_os_layer_is_timer_active(void);
void bt_mesh_os_layer_start_timer(uint32_t tick);
void bt_mesh_os_layer_stop_timer(void);
void bt_mesh_os_layer_deregister_callback(void *callback);
void bt_mesh_os_layer_register_callback(void *callback);
void bt_mesh_os_layer_aes_encrypt(uint8_t *encrypted_data, uint8_t *plain_text, uint8_t *key);

bool bt_mesh_os_layer_read_record_data(const char *data_item_name, void *buffer, uint32_t *len);
bool bt_mesh_os_layer_write_record_data(const char *data_item_name, void *buffer, uint32_t len);
bool bt_mesh_os_layer_delete_all_record_data(void);
bool bt_mesh_os_layer_read_sequence_number_record_data(const char *data_item_name, void *buffer, uint32_t *len);
bool bt_mesh_os_layer_write_sequence_number_record(const char *data_item_name, void *buffer, uint32_t len);
bool bt_mesh_os_layer_delete_all_sequence_number_record(void);
bool bt_mesh_os_layer_read_foundation_model_record_data(const char* data_item_name, void *buffer, uint32_t *len);
bool bt_mesh_os_layer_write_foundation_model_record(const char* data_item_name, void *buffer, uint32_t len);
bool bt_mesh_os_layer_delete_all_foundation_model_record(void);
bool bt_mesh_os_layer_read_model_record_data(const char* data_item_name, void *buffer, uint32_t *len);
bool bt_mesh_os_layer_write_model_record(const char* data_item_name, void *buffer, uint32_t len);
bool bt_mesh_os_layer_delete_all_model_record(void);
#ifdef __cplusplus
}
#endif

#endif /* MESH_OS_LAYER_API_H */

