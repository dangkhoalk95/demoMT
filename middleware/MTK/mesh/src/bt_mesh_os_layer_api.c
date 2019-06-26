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
 
#if 0
#include "task.h"
#include "semphr.h"
#include "portmacro.h"
#include "queue.h"
#endif
#include "FreeRTOS.h"
#include <timers.h>
#include <string.h>
//#include "syslog.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/queue.h>
#include "bt_mesh_os_layer_api.h"
#include "hal_trng.h"
#include "bt_callback_manager.h"
#include "bt_type.h"
#include "nvdm.h"
#include "hal_nvic.h"


log_control_block_t log_control_block_mesh_os_layer={
	"mesh_os_layer",
	(DEBUG_LOG_ON),
	(PRINT_LEVEL_ERROR),
	print_module_log,
	dump_module_buffer
};

////////////////////////////////////////////////////////////////////////////////
// Private Type Definition /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    LIST_HEAD(L_HEAD, list_entry) head;
    uint32_t tag;
    uint32_t count;
} MESH_OS_LAYER_DLIST;

typedef struct list_entry {
    LIST_ENTRY(list_entry) entry;
    uint32_t tag;
    void *data;
} MESH_OS_LAYER_DLIST_ENTRY;



////////////////////////////////////////////////////////////////////////////////
// Private Type Definition /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    STAILQ_HEAD(Q_HEAD, queue_entry) head;
    uint32_t tag;
    uint32_t count;
} MESH_OS_LAYER_QUEUE;


typedef struct queue_entry {
    STAILQ_ENTRY(queue_entry) entry;
    uint32_t tag;
    void *data;
} MESH_OS_LAYER_QUEUE_ENTRY;


static uint32_t nvic_mask = 0;


/*******************************************
  *************memory operation***************
********************************************/

uint32_t p_test_heap = 0;
uint8_t *bt_mesh_os_layer_memory_alloc(uint16_t size)
{
    if (size == 0) {
        return NULL;
    }
    uint8_t *p = (uint8_t *)pvPortMalloc(size);
    if (!p) {
        LOG_E(mesh_os_layer, "bt_mesh_os_layer_memory_alloc failed, size(%d).\n", size);
    }
    p_test_heap = (uint32_t)p;
    return p;
}

void bt_mesh_os_layer_memory_free(uint8_t *p)
{
    if(p == NULL) {
        return;
    }
    vPortFree(p);
    p = NULL;
}

/*******************************************
  ****************List operation***************
********************************************/

////////////////////////////////////////////////////////////////////////////////
// Public Functions ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *bt_mesh_os_layer_ds_dlist_alloc(void)
{
    MESH_OS_LAYER_DLIST *list;

    list = (MESH_OS_LAYER_DLIST *)bt_mesh_os_layer_memory_alloc(sizeof(MESH_OS_LAYER_DLIST));
    if (!list) {
        LOG_I(mesh_os_layer,"dlist alloc failed.\n");
        return NULL;
    }
    list->tag = 0x19760108;
    list->count = 0;
    list->head.lh_first = NULL;

    return list;
}

void bt_mesh_os_layer_ds_dlist_free(void *list)
{
    void *dat;
    while ((dat = bt_mesh_os_layer_ds_dlist_remove_head(list))) {
        bt_mesh_os_layer_ds_dlist_entry_free(dat);
    }

    bt_mesh_os_layer_memory_free(list);
}

void bt_mesh_os_layer_ds_dlist_empty(void *list)
{
    void *dat;
    while ((dat = bt_mesh_os_layer_ds_dlist_remove_head(list))) {
        bt_mesh_os_layer_ds_dlist_entry_free(dat);
    }
}

void *bt_mesh_os_layer_ds_dlist_entry_alloc(uint32_t size)
{
    MESH_OS_LAYER_DLIST_ENTRY *new_entry;

    new_entry = (MESH_OS_LAYER_DLIST_ENTRY *) bt_mesh_os_layer_memory_alloc(sizeof(MESH_OS_LAYER_DLIST_ENTRY) + size);

    if (new_entry) {
        new_entry->tag = 0x20111118;
        return (uint8_t *)new_entry + offsetof(MESH_OS_LAYER_DLIST_ENTRY, data);
    }

    LOG_I(mesh_os_layer,"dlist entry alloc failed.\n");
    return NULL;
}

void bt_mesh_os_layer_ds_dlist_entry_free(void *dat)
{
    uint32_t *ptag;
    MESH_OS_LAYER_DLIST_ENTRY *pentry;

    if (dat == NULL) {
        return;
    }

    ptag = (uint32_t *)((uint8_t *)dat  - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, tag));
    pentry = (MESH_OS_LAYER_DLIST_ENTRY *)((uint8_t *)dat - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));

    if (*ptag == 0x20111118) { //valid entry
        bt_mesh_os_layer_memory_free((uint8_t *)pentry);
    }
}

void *bt_mesh_os_layer_ds_dlist_remove_head(void *list)
{
    void *pdat = NULL;
    MESH_OS_LAYER_DLIST_ENTRY *entry_removed;

    if (LIST_FIRST(&((MESH_OS_LAYER_DLIST *)list)->head)) {
        pdat = ((uint8_t *)LIST_FIRST(&((MESH_OS_LAYER_DLIST *)list)->head) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));
        entry_removed = (MESH_OS_LAYER_DLIST_ENTRY *)LIST_FIRST(&((MESH_OS_LAYER_DLIST *)list)->head);

        LIST_REMOVE(entry_removed, entry);
        ((MESH_OS_LAYER_DLIST *)list)->count--;
    }

    return pdat;
}

void *bt_mesh_os_layer_ds_dlist_remove(void *list, void *removed)
{
    uint32_t *tag_removed;
    void *pdat = removed;

    MESH_OS_LAYER_DLIST_ENTRY *entry_removed;

    if (pdat == NULL) {
        pdat = bt_mesh_os_layer_ds_dlist_remove_head(list);
    } else {
        tag_removed = (uint32_t *)((uint8_t *)removed  - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, tag));

        if (*tag_removed == 0x20111118) { //valid entry
            entry_removed = (MESH_OS_LAYER_DLIST_ENTRY *)((uint8_t *)removed - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));
            LIST_REMOVE(entry_removed, entry);
            ((MESH_OS_LAYER_DLIST *)list)->count--;
        }
    }

    return pdat;
}

bool bt_mesh_os_layer_ds_dlist_delete(void *list, void *deleted)
{
    bool ret = false;

    if (deleted == NULL) {
        MESH_OS_LAYER_DLIST_ENTRY *head_entry = LIST_FIRST(&((MESH_OS_LAYER_DLIST *)list)->head);

        if (head_entry) {
            LIST_REMOVE(head_entry, entry);
            ((MESH_OS_LAYER_DLIST *)list)->count--;
            return true;
        }
    } else {
        ret = bt_mesh_os_layer_ds_dlist_remove(list, deleted);

        if (ret) {
            bt_mesh_os_layer_ds_dlist_entry_free(deleted);
        }
    }

    return ret;
}

void bt_mesh_os_layer_ds_dlist_insert(void *list, void *inserted)
{
    uint32_t *tag_inserted;
    MESH_OS_LAYER_DLIST_ENTRY *entry_inserted;

    if (inserted == NULL) {
        return;
    }

    tag_inserted = (uint32_t *)((uint8_t *)inserted  - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, tag));

    if (*tag_inserted == 0x20111118) { //valid entry
        entry_inserted = (MESH_OS_LAYER_DLIST_ENTRY *)((uint8_t *)inserted - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));
        LIST_INSERT_HEAD(&((MESH_OS_LAYER_DLIST *)list)->head, entry_inserted, entry);
        ((MESH_OS_LAYER_DLIST *)list)->count++;
    }
}

void bt_mesh_os_layer_ds_dlist_insert_after(void *list, void *inlist, void *inserted)
{
    uint32_t *tag_inlist;
    uint32_t *tag_inserted;

    MESH_OS_LAYER_DLIST_ENTRY *entry_inlist;
    MESH_OS_LAYER_DLIST_ENTRY *entry_inserted;

    if (inlist == NULL || inserted == NULL) {
        return;
    }

    tag_inlist = (uint32_t *)((uint8_t *)inlist  - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, tag));
    tag_inserted = (uint32_t *)((uint8_t *)inserted  - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, tag));

    if (*tag_inlist == 0x20111118 && *tag_inserted == 0x20111118) { //valid entry
        entry_inlist = (MESH_OS_LAYER_DLIST_ENTRY *)((uint8_t *)inlist - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));
        entry_inserted = (MESH_OS_LAYER_DLIST_ENTRY *)((uint8_t *)inserted - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));
        LIST_INSERT_AFTER(entry_inlist, entry_inserted, entry);
        ((MESH_OS_LAYER_DLIST *)list)->count++;
    }
}

void bt_mesh_os_layer_ds_dlist_insert_before(void *list, void *inlist, void *inserted)
{
    uint32_t *tag_inlist;
    uint32_t *tag_inserted;

    MESH_OS_LAYER_DLIST_ENTRY *entry_inlist;
    MESH_OS_LAYER_DLIST_ENTRY *entry_inserted;

    if (inlist == NULL || inserted == NULL) {
        return;
    }

    tag_inlist = (uint32_t *)((uint8_t *)inlist  - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, tag));
    tag_inserted = (uint32_t *)((uint8_t *)inserted  - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, tag));

    if (*tag_inlist == 0x20111118 && *tag_inserted == 0x20111118) { //valid entry
        entry_inlist = (MESH_OS_LAYER_DLIST_ENTRY *)((uint8_t *)inlist - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));
        entry_inserted = (MESH_OS_LAYER_DLIST_ENTRY *)((uint8_t *)inserted - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));
        LIST_INSERT_BEFORE(entry_inlist, entry_inserted, entry);
        ((MESH_OS_LAYER_DLIST *)list)->count++;
    }
}

void *bt_mesh_os_layer_ds_dlist_first(void *list)
{
    void *pdat = NULL;

    if (list != NULL && LIST_FIRST(&((MESH_OS_LAYER_DLIST *)list)->head)) {
        pdat = ((uint8_t *)LIST_FIRST(&((MESH_OS_LAYER_DLIST *)list)->head) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));
    }

    return pdat;
}

void *bt_mesh_os_layer_ds_dlist_next(void *list, void *dat)
{
    uint32_t *ptag;
    void *pdat = NULL;

    MESH_OS_LAYER_DLIST_ENTRY *pentry;

    ptag = (uint32_t *)((uint8_t *)dat  - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data) + offsetof(MESH_OS_LAYER_DLIST_ENTRY, tag));
    pentry = (MESH_OS_LAYER_DLIST_ENTRY *)((uint8_t *)dat - offsetof(MESH_OS_LAYER_DLIST_ENTRY, data));

    if (*ptag == 0x20111118) { //valid entry
        pdat = LIST_NEXT(pentry, entry);
        if (pdat != NULL) {
            pdat += offsetof(MESH_OS_LAYER_DLIST_ENTRY, data);
        }
    }

    return pdat;
}

uint32_t bt_mesh_os_layer_ds_dlist_count(void *list)
{
    if (((MESH_OS_LAYER_DLIST *)list)->tag == 0x19760108) {
        return ((MESH_OS_LAYER_DLIST *)list)->count;
    }

    return 0;
}

/*******************************************
  ****************List operation***************
********************************************/

////////////////////////////////////////////////////////////////////////////////
// Public Functions ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *bt_mesh_os_layer_ds_queue_alloc(void)
{
    MESH_OS_LAYER_QUEUE *queue;

    queue = (MESH_OS_LAYER_QUEUE *)bt_mesh_os_layer_memory_alloc(sizeof(MESH_OS_LAYER_QUEUE));
    if (!queue) {
        LOG_I(mesh_os_layer,"queue alloc failed.\n");
        return NULL;
    }

    queue->tag = 0x19760108;
    queue->count = 0;
    queue->head.stqh_first = NULL;
    queue->head.stqh_last = &(queue->head.stqh_first);

    return queue;
}

void bt_mesh_os_layer_ds_queue_free(void *queue)
{
    void *dat;
    while ((dat = bt_mesh_os_layer_ds_queue_pop(queue))) {

        bt_mesh_os_layer_ds_queue_entry_free(dat);
    }

    bt_mesh_os_layer_memory_free(queue);
}

void *bt_mesh_os_layer_ds_queue_entry_alloc(uint32_t size)
{
    MESH_OS_LAYER_QUEUE_ENTRY *new_entry;

    new_entry = (MESH_OS_LAYER_QUEUE_ENTRY *) bt_mesh_os_layer_memory_alloc(sizeof(MESH_OS_LAYER_QUEUE_ENTRY) + size);

    if (new_entry) {
        new_entry->tag = 0x20111118;
        return (uint8_t *)new_entry + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data);
    }

    LOG_I(mesh_os_layer,"queue entry alloc failed.\n");
    return NULL;
}

void bt_mesh_os_layer_ds_queue_entry_free(void *dat)
{
    uint32_t *ptag;
    MESH_OS_LAYER_QUEUE_ENTRY *pentry;

    ptag = (uint32_t *)((uint8_t *)dat  - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, tag));
    pentry = (MESH_OS_LAYER_QUEUE_ENTRY *)((uint8_t *)dat - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));

    if (*ptag == 0x20111118) { //valid entry
        bt_mesh_os_layer_memory_free((uint8_t *)pentry);
    }
}

void bt_mesh_os_layer_ds_queue_push(void *queue, void *dat)
{
    uint32_t *ptag;

    MESH_OS_LAYER_QUEUE_ENTRY *pentry;

    if (((MESH_OS_LAYER_QUEUE *)queue)->tag == 0x19760108) {
        ptag = (uint32_t *)((uint8_t *)dat  - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, tag));
        pentry = (MESH_OS_LAYER_QUEUE_ENTRY *)((uint8_t *)dat - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));

        if (*ptag == 0x20111118) { //valid entry
            STAILQ_INSERT_TAIL(&((MESH_OS_LAYER_QUEUE *)queue)->head, pentry, entry);
            ((MESH_OS_LAYER_QUEUE *)queue)->count++;
        }
    }
}

void bt_mesh_os_layer_ds_queue_push_front(void *queue, void *dat)
{
    uint32_t *ptag;

    MESH_OS_LAYER_QUEUE_ENTRY *pentry;

    if (((MESH_OS_LAYER_QUEUE *)queue)->tag == 0x19760108) {
        ptag = (uint32_t *)((uint8_t *)dat  - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, tag));
        pentry = (MESH_OS_LAYER_QUEUE_ENTRY *)((uint8_t *)dat - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));

        if (*ptag == 0x20111118) { //valid entry
            STAILQ_INSERT_HEAD(&((MESH_OS_LAYER_QUEUE *)queue)->head, pentry, entry);
            ((MESH_OS_LAYER_QUEUE *)queue)->count++;
        }
    }
}

void *bt_mesh_os_layer_ds_queue_pop(void *queue)
{
    void *pdat = NULL;

    if (STAILQ_FIRST(&((MESH_OS_LAYER_QUEUE *)queue)->head)) {
        pdat = ((uint8_t *)STAILQ_FIRST(&((MESH_OS_LAYER_QUEUE *)queue)->head) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));
        STAILQ_REMOVE_HEAD(&((MESH_OS_LAYER_QUEUE *)queue)->head, entry);
        ((MESH_OS_LAYER_QUEUE *)queue)->count--;
    }

    return pdat;
}

void *bt_mesh_os_layer_ds_queue_first(void *queue)
{
    void *pdat = NULL;

    if (STAILQ_FIRST(&((MESH_OS_LAYER_QUEUE *)queue)->head)) {
        pdat = ((uint8_t *)STAILQ_FIRST(&((MESH_OS_LAYER_QUEUE *)queue)->head) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));
    }

    return pdat;
}

void *bt_mesh_os_layer_ds_queue_next(void *queue, void *dat)
{
    uint32_t *ptag;
    void *pdat = NULL;

    MESH_OS_LAYER_QUEUE_ENTRY *pentry;

    ptag = (uint32_t *)((uint8_t *)dat  - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, tag));
    pentry = (MESH_OS_LAYER_QUEUE_ENTRY *)((uint8_t *)dat - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));

    if (*ptag == 0x20111118) { //valid entry
        if (STAILQ_NEXT(pentry, entry)) {
            pdat = ((uint8_t *)STAILQ_NEXT(pentry, entry) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));
        }
    }

    return pdat;
}

void bt_mesh_os_layer_ds_queue_insert_after(void *queue, void *inqueue, void *inserted)
{
    uint32_t *tag_inqueue;
    uint32_t *tag_inserted;

    MESH_OS_LAYER_QUEUE_ENTRY *entry_inqueue;
    MESH_OS_LAYER_QUEUE_ENTRY *entry_inserted;

    tag_inqueue = (uint32_t *)((uint8_t *)inqueue  - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, tag));
    tag_inserted = (uint32_t *)((uint8_t *)inserted  - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, tag));

    if (*tag_inqueue == 0x20111118 && *tag_inserted == 0x20111118) { //valid entry
        entry_inqueue = (MESH_OS_LAYER_QUEUE_ENTRY *)((uint8_t *)inqueue - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));
        entry_inserted = (MESH_OS_LAYER_QUEUE_ENTRY *)((uint8_t *)inserted - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));
        STAILQ_INSERT_AFTER(&((MESH_OS_LAYER_QUEUE *)queue)->head, entry_inqueue, entry_inserted, entry);
        ((MESH_OS_LAYER_QUEUE *)queue)->count++;
    }

}

void bt_mesh_os_layer_ds_queue_remove(void *queue, void *removed)
{
    uint32_t *tag_removed;

    MESH_OS_LAYER_QUEUE_ENTRY *entry_removed;

    tag_removed = (uint32_t *)((uint8_t *)removed - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data) + offsetof(MESH_OS_LAYER_QUEUE_ENTRY, tag));

    if (*tag_removed == 0x20111118) { //valid entry
        entry_removed = (MESH_OS_LAYER_QUEUE_ENTRY *)((uint8_t *)removed - offsetof(MESH_OS_LAYER_QUEUE_ENTRY, data));
        STAILQ_REMOVE(&((MESH_OS_LAYER_QUEUE *)queue)->head, entry_removed, queue_entry, entry);
        ((MESH_OS_LAYER_QUEUE *)queue)->count--;
    }
}

uint32_t bt_mesh_os_layer_ds_queue_count(void *queue)
{
    if (((MESH_OS_LAYER_QUEUE *)queue)->tag == 0x19760108) {
        return ((MESH_OS_LAYER_QUEUE *)queue)->count;
    }

    return 0;
}

/*******************************************
  ****************Cryption tool ***************
********************************************/
#include "hal_aes.h"
#include "mbedtls/md5.h"
#if !defined(__GNUC__)
#include "mbedtls/aes.h"
#endif
void bt_mesh_os_layer_byte_reverse(uint8_t *dst, const uint8_t *src, uint32_t length)
{
    uint32_t i;
    for (i = 0; i < length; i++) {
        dst[i] = src[length - i - 1];
    }
}

void bt_mesh_os_layer_aes_encrypt(uint8_t *encrypted_data, uint8_t *plain_text, uint8_t *key)
{
#if defined(__GNUC__)
    uint8_t enc_temp_buf[32] = {0};
    uint8_t plain_temp_buf[16] = {0};
    uint8_t key_temp_buf[16] = {0};
    bt_mesh_os_layer_aes_buffer_t enc_temp = {
        .buffer = (uint8_t *)(&enc_temp_buf),
        .length = 32,
    };
    bt_mesh_os_layer_aes_buffer_t plain_temp = {
        .buffer = (uint8_t *)(&plain_temp_buf),
        .length = 16,
    };
    bt_mesh_os_layer_aes_buffer_t key_temp = {
        .buffer = (uint8_t *)(&key_temp_buf),
        .length = 16,
    };
    //LOG_I(mesh_os_layer,"bt_mesh_os_layer_aes_encrypt use hal version");
    #if 0
    LOG_I(mesh_os_layer,"plain_text: ");
    for(int i=0; i < 16; i++) {
        LOG_I(mesh_os_layer,"%02x ", plain_text[0]);
	}
    LOG_I(mesh_os_layer,"\nkey: ");
	for(int i=0; i < 16; i++) {
        LOG_I(mesh_os_layer,"%02x ", key[0]);
	}
	LOG_I(mesh_os_layer,"\n");
	#endif
    memcpy(plain_temp.buffer, plain_text, 16);
    memcpy(key_temp.buffer, key, 16);
    hal_aes_ecb_encrypt((hal_aes_buffer_t *)&enc_temp, (hal_aes_buffer_t *)&plain_temp, (hal_aes_buffer_t *)&key_temp);
    //LOG_I(mesh_os_layer,"enc_len =%d, plain_text_len temp = %d\n", enc_temp.length, plain_temp.length);
    #if 0
    LOG_I(mesh_os_layer,"encrypted_data: ");
    for(int i=0; i < 16; i++) {
        printf("%02x ", enc_temp.buffer[0]);
	}
    printf("\n");
	#endif
    memcpy(encrypted_data, enc_temp.buffer, 16);
#else
    mbedtls_aes_context context;
    mbedtls_aes_init(&context);
    mbedtls_aes_setkey_enc(&context, key, sizeof(bt_key_t) * 8);
    mbedtls_aes_crypt_ecb(&context, MBEDTLS_AES_ENCRYPT, (unsigned char *)plain_text, (unsigned char *)encrypted_data);
    mbedtls_aes_free(&context);
    //LOG_I(mesh_os_layer,"bt_mesh_os_layer_aes_encrypt use mbed version");
#endif
}

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"

mbedtls_ecdh_context ctx_cli;
mbedtls_ctr_drbg_context ctr_drbg;
uint8_t buffer[1024] = {0};
size_t olen;
uint8_t ecdh_buf_byte_1 = 0x00;
#define ECDH_PUBLIC_KEY_LEN 64
#define ECDH_SECRET_LEN     32

void bt_mesh_os_layer_read_local_p256_public_key(uint8_t *public_key)
{

    int ret;
    mbedtls_entropy_context entropy;
    const char pers[] = "ecdh";
//    ((void) argc);
    //  ((void) argv);

    mbedtls_ecdh_init( &ctx_cli );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    /*
     * Initialize random number generation
     */
    LOG_I(mesh_os_layer, "  . Seeding the random number generator...\n" );

    mbedtls_entropy_init( &entropy );
    if ( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                        (const unsigned char *) pers,
                                        sizeof pers ) ) != 0 ) {
        LOG_E(mesh_os_layer, " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    LOG_I(mesh_os_layer, " ok\n" );

    /*
     * Client: inialize context and generate keypair
     */
    LOG_I(mesh_os_layer, "  . Setting up client context...\n" );

    ret = mbedtls_ecp_group_load( &ctx_cli.grp, MBEDTLS_ECP_DP_SECP256R1);
    if ( ret != 0 ) {
        LOG_E(mesh_os_layer, " failed\n  ! mbedtls_ecp_group_load returned %d\n", ret );
        goto exit;
    }

    ret = mbedtls_ecdh_make_public( &ctx_cli, &olen, buffer, sizeof( buffer),
                                    mbedtls_ctr_drbg_random, &ctr_drbg );
    LOG_I(mesh_os_layer,"mbedtls_ecdh_make_public: len = %d\n", olen);
    if ( ret != 0 ) {
        LOG_E(mesh_os_layer, " failed\n  ! mbedtls_ecdh_gen_public returned %d\n", ret );
        goto exit;
    }

    ret = mbedtls_ecp_point_write_binary(&ctx_cli.grp, &ctx_cli.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, (uint8_t*)(&buffer), sizeof(buffer));
    if ( ret != 0 ) {
        LOG_E(mesh_os_layer, " failed\n  ! mbedtls_ecp_point_write_binary returned %d\n", ret );
        goto exit;
    }
    LOG_I(mesh_os_layer,"mbedtls_ecp_point_write_binary:\n olen = %d\n", olen);
    assert(olen == (ECDH_PUBLIC_KEY_LEN + 1));
    LOG_I(mesh_os_layer,"buffer: \n");
    for (int i = 0; i < ECDH_PUBLIC_KEY_LEN + 1; i++) {
        LOG_I(mesh_os_layer,"%02x ", buffer[i]);
    }
    LOG_I(mesh_os_layer,"\n");
    ecdh_buf_byte_1 = buffer[0]; //Tengfei: big endian
    //memcpy(public_key, &(buffer[1]), ECDH_PUBLIC_KEY_LEN);

    bt_mesh_os_layer_byte_reverse(public_key, &(buffer[1]), 32);

    bt_mesh_os_layer_byte_reverse(public_key + 32,  &(buffer[33]), 32);
    memset(buffer, 0, sizeof(buffer));

    LOG_I(mesh_os_layer, " ok\n" );
    return;

exit:

    mbedtls_ecdh_free( &ctx_cli );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );
    LOG_I(mesh_os_layer, " failed\n" );
    return;
}

void bt_mesh_os_layer_generate_dhkey(uint8_t *peer_pb_key, uint8_t *dh_key)
{

    int ret;

    /*
     * Client: read peer's key and generate shared secret
     */
    LOG_I(mesh_os_layer, "  . Client reading server key and computing secret...\n" );
    LOG_I(mesh_os_layer,"bt_mesh_os_layer_generate_dhkey:\n ecdh byte 1 = %02x\n", ecdh_buf_byte_1);
    LOG_I(mesh_os_layer,"peer_pb_key: \n");
    for (int i = 0; i < ECDH_PUBLIC_KEY_LEN; i++) {
        LOG_I(mesh_os_layer,"%02x ", peer_pb_key[i]);
    }
    LOG_I(mesh_os_layer,"\n");

    buffer[0] = ecdh_buf_byte_1;
    memcpy(&(buffer[1]), peer_pb_key, ECDH_PUBLIC_KEY_LEN);

    bt_mesh_os_layer_byte_reverse(&(buffer[1]), peer_pb_key, 32);
    bt_mesh_os_layer_byte_reverse(&(buffer[33]), peer_pb_key + 32, 32);

    ret = mbedtls_ecp_point_read_binary(&ctx_cli.grp, &ctx_cli.Qp, (uint8_t *)(&buffer), ECDH_PUBLIC_KEY_LEN + 1);
    if ( ret != 0 ) {
        LOG_E(mesh_os_layer, " failed\n  ! mbedtls_ecp_point_read_binary returned %d\n", ret );
        goto exit;
    }

    ret = mbedtls_ecdh_calc_secret( &ctx_cli, &olen, buffer, sizeof( buffer ),
                                    mbedtls_ctr_drbg_random, &ctr_drbg);
    if ( ret != 0 ) {
        LOG_E(mesh_os_layer, " failed\n  ! mbedtls_ecdh_calc_secret returned %d\n", ret );
        goto exit;
    }
    assert(olen == ECDH_SECRET_LEN);
    //mbedtls_mpi_write_binary( &ctx_cli.z, dh_key, ECDH_SECRET_LEN );
    bt_mesh_os_layer_byte_reverse(dh_key, buffer, ECDH_SECRET_LEN); // convert to little endian
    LOG_I(mesh_os_layer,"bt_mesh_os_layer_generate_dhkey:");
    for (int i = 0; i < ECDH_SECRET_LEN; i++) {
        LOG_I(mesh_os_layer,"%02x ", dh_key[i]);
    }
    LOG_I(mesh_os_layer,"\n");
    LOG_I(mesh_os_layer, " ok\n" );
    return;

exit:
    LOG_I(mesh_os_layer, " failed\n" );
    assert(0);
    return;
}

//timer
static TimerHandle_t mesh_os_timer = NULL; /**< Timer handler. */
static bt_mesh_os_layer_timer_expired_t bt_rtos_timer_cb;  /**< Timer callback function. */

void bt_mesh_os_layer_register_timer_callback(bt_mesh_os_layer_timer_expired_t callback)
{
    bt_rtos_timer_cb = callback;
}

static void bt_mesh_os_layer_rtos_timer_os_expire(TimerHandle_t timer)
{
    if (bt_rtos_timer_cb != NULL) {
        bt_rtos_timer_cb();
    }
}

void bt_mesh_os_layer_init_timer(void)
{
    if (mesh_os_timer == NULL) {
        mesh_os_timer = xTimerCreate( "mesh timer", 0xffff, pdFALSE, NULL, bt_mesh_os_layer_rtos_timer_os_expire);
    }
}

void bt_mesh_os_layer_deinit_timer(void)
{
    if (mesh_os_timer != NULL) {
        xTimerDelete(mesh_os_timer, 0);
        mesh_os_timer = NULL;
    }
}

uint32_t bt_mesh_os_layer_create_semaphore()
{
    return (uint32_t)xSemaphoreCreateBinary();
}

void bt_mesh_os_layer_take_semaphore(uint32_t semaphore_id)
{
    xSemaphoreTake((SemaphoreHandle_t)semaphore_id, portMAX_DELAY);
}

void bt_mesh_os_layer_give_semaphore(uint32_t semaphore_id)
{
    xSemaphoreGive((SemaphoreHandle_t)semaphore_id);
}

void bt_mesh_os_layer_delete_semaphore(uint32_t semaphore_id)
{
    vSemaphoreDelete((SemaphoreHandle_t)semaphore_id);
}

uint32_t bt_mesh_os_layer_get_system_tick(void)
{
    return xTaskGetTickCount();
}

uint32_t bt_mesh_os_layer_ms_to_tick(uint32_t time_ms)
{
    return (time_ms / portTICK_PERIOD_MS);
}

uint32_t bt_mesh_os_layer_is_timer_active(void)
{
    if ((mesh_os_timer != NULL) && (xTimerIsTimerActive(mesh_os_timer) != pdFALSE)) {
        return 1;
    } else {
        return 0;
    }
}

void bt_mesh_os_layer_start_timer(uint32_t tick)
{
    if (mesh_os_timer == NULL) {
        return;
    }
    if (bt_mesh_os_layer_is_timer_active() == 1) {
        bt_mesh_os_layer_stop_timer();
    }
    //assert(tick > 0);
	
    xTimerChangePeriod(mesh_os_timer, tick + 1, portMAX_DELAY);
    xTimerReset(mesh_os_timer, portMAX_DELAY);
}

void bt_mesh_os_layer_stop_timer(void)
{
    if  ((mesh_os_timer != NULL) && (bt_mesh_os_layer_is_timer_active() == 1)) {
        xTimerStop(mesh_os_timer, portMAX_DELAY);
    }
}



void bt_mesh_os_layer_get_random(uint8_t *rand, uint8_t len)
{
    uint32_t random_seed, i, j;
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
    ret = hal_trng_init();
    if (HAL_TRNG_STATUS_OK != ret) {
        LOG_I(mesh_os_layer,"mesh generate_random_address--error 1");
    }
    for (i = 0; i < 30; ++i) {
        ret = hal_trng_get_generated_random_number(&random_seed);
        if (HAL_TRNG_STATUS_OK != ret) {
            LOG_I(mesh_os_layer,"mesh generate_random_address--error 2");
        }
    }

    for (i = 0; i < (len / 4); i++) {
        ret = hal_trng_get_generated_random_number(&random_seed);
        if (HAL_TRNG_STATUS_OK != ret) {
            LOG_I(mesh_os_layer,"mesh generate_random_address--error 3");
        }
        rand[4 * i + 0] = random_seed & 0xFF;
        rand[4 * i + 1] = (random_seed >> 8) & 0xFF;
        rand[4 * i + 2] = (random_seed >> 16) & 0xFF;
        rand[4 * i + 3] = (random_seed >> 24) & 0xFF;

    }

    j = len % 4;
    if (j > 0) {
        ret = hal_trng_get_generated_random_number(&random_seed);
        if (HAL_TRNG_STATUS_OK != ret) {
            LOG_I(mesh_os_layer,"mesh generate_random_address--error 3");
        }
        while (j > 0) {
            rand[4 * i + (j - 1)] = random_seed >> (8 * (j - 1)) & 0xFF;
            j--;
        }
    }

    hal_trng_deinit();
}


void bt_mesh_os_layer_disable_interrupt(void)
{
    hal_nvic_save_and_set_interrupt_mask((uint32_t *)(&nvic_mask));
}

void bt_mesh_os_layer_enable_interrupt(void)
{
    hal_nvic_restore_interrupt_mask(nvic_mask);
}

void bt_mesh_os_layer_trigger_interrupt(uint32_t is_from_isr)
{
    bt_trigger_interrupt(is_from_isr);
}

void bt_mesh_os_layer_enter_critical(void)
{
}

void bt_mesh_os_layer_exit_critical(void)
{
}

void bt_mesh_os_layer_deregister_callback(void *callback)
{
    bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)callback);
}
void bt_mesh_os_layer_register_callback(void *callback)
{
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SYSTEM | MODULE_MASK_GAP | MODULE_MASK_GATT, (void *)callback);
}

bool bt_mesh_os_layer_read_record_data(const char* data_item_name, void *buffer, uint32_t *len) 
{	
	nvdm_status_t status;
	status = nvdm_read_data_item("MESH", data_item_name,  buffer, len);
	
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_read_record_data: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_write_record_data(const char* data_item_name, void *buffer, uint32_t len) 
{	
	nvdm_status_t status;
	status = nvdm_write_data_item("MESH", data_item_name, NVDM_DATA_ITEM_TYPE_RAW_DATA, buffer, len);
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_write_record_data: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_delete_all_record_data(void) 
{	
	nvdm_status_t status;
	status = nvdm_delete_group("MESH");
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_delete_all_record_data: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_read_sequence_number_record_data(const char* data_item_name, void *buffer, uint32_t *len) 
{	
	nvdm_status_t status;
	status = nvdm_read_data_item("MESH_SEQ", data_item_name,  buffer, len);
	
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_read_sequence_number_record_data: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_write_sequence_number_record(const char* data_item_name, void *buffer, uint32_t len) 
{	
	nvdm_status_t status;
	status = nvdm_write_data_item("MESH_SEQ", data_item_name, NVDM_DATA_ITEM_TYPE_RAW_DATA, buffer, len);
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_write_sequence_number_record: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_delete_all_sequence_number_record(void) 
{	
	nvdm_status_t status;
	status = nvdm_delete_group("MESH_SEQ");
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_delete_sequence_number_record_data: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_read_foundation_model_record_data(const char* data_item_name, void *buffer, uint32_t *len) 
{	
	nvdm_status_t status;
	status = nvdm_read_data_item("MESH_FOU", data_item_name,  buffer, len);
	
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_read_foundation_model_record_data: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_write_foundation_model_record(const char* data_item_name, void *buffer, uint32_t len) 
{	
	nvdm_status_t status;
	status = nvdm_write_data_item("MESH_FOU", data_item_name, NVDM_DATA_ITEM_TYPE_RAW_DATA, buffer, len);
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_write_foundation_model_record: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_delete_all_foundation_model_record(void) 
{	
	nvdm_status_t status;
	status = nvdm_delete_group("MESH_FOU");
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_delete_all_foundation_model_record: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_read_model_record_data(const char* data_item_name, void *buffer, uint32_t *len) 
{	
	nvdm_status_t status;
	status = nvdm_read_data_item("MESH_MOD", data_item_name,  buffer, len);
	
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_read_model_record_data: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_write_model_record(const char* data_item_name, void *buffer, uint32_t len) 
{	
	nvdm_status_t status;
	status = nvdm_write_data_item("MESH_MOD", data_item_name, NVDM_DATA_ITEM_TYPE_RAW_DATA, buffer, len);
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_write_model_record: status = %d", status);
		return false;
    }
	return true;
}

bool bt_mesh_os_layer_delete_all_model_record(void) 
{	
	nvdm_status_t status;
	status = nvdm_delete_group("MESH_MOD");
    if (NVDM_STATUS_OK != status) {
        //LOG_E(mesh_os_layer, "bt_mesh_os_layer_delete_all_model_record: status = %d", status);
		return false;
    }
	return true;
}

