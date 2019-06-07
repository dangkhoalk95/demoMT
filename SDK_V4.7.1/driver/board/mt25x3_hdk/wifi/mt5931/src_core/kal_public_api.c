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


#include "stdio.h"   /* for kal_sprintf */
//#include "gl_typedef.h"
//#include "ucos_ii.h"
#include "freertos_ii.h"

#include "lwipopts.h"
#include "pbuf.h"
#include "kal_general_types.h"
#include "kal_public_defs.h"
#include "stack_ltlcom.h"
#include "kal_public_api.h"
#include "FreeRTOS.h"
#include "task.h"


typedef struct {
    ilm_struct      ilm;
    unsigned char   used;
    module_type     mod;
} ILM_STRUCT_ARRAY;

typedef struct {
    unsigned int    alloc_cnt;
    unsigned int    free_cnt;
} ILM_STRUCT_MOD_CNT;


typedef enum {
    ILM_NUM_MOD_WNDRV   = 50,
    ILM_NUM_MOD_SUPC    = 50,
    ILM_NUM_MOD_HOSTAP  = 20,
    ILM_NUM_MOD_ABM     = 40,
    ILM_NUM_MOD_WNDRV_HISR = 50,
    ILM_NUM_MOD_TCPIP   = 30,
    ILM_NUM_MOD_BWCS    = 0,
    ILM_NUM_MOD_TIMER   = 0,
    ILM_NUM_MOD_NIL     = 0,
    ILM_NUM_MOD_FT      = 10,
    ILM_NUM_MOD_MMI     = 0,
    ILM_NUM_MOD_BT      = 0,
    ILM_NUM_MOD_NVRAM   = 0,
    ILM_NUM_MOD_TIMER_HOSTAP = 10,
    ILM_NUM_MOD_TOTAL   = (ILM_NUM_MOD_WNDRV + ILM_NUM_MOD_SUPC + ILM_NUM_MOD_HOSTAP + ILM_NUM_MOD_ABM\
                           + ILM_NUM_MOD_WNDRV_HISR + ILM_NUM_MOD_TCPIP + ILM_NUM_MOD_BWCS + ILM_NUM_MOD_TIMER\
                           + ILM_NUM_MOD_NIL + ILM_NUM_MOD_FT + ILM_NUM_MOD_MMI + ILM_NUM_MOD_BT + ILM_NUM_MOD_NVRAM\
                           + ILM_NUM_MOD_TIMER_HOSTAP)
} ilm_module_num;

typedef enum {
    ILM_OFFSET_MOD_WNDRV        = 0,
    ILM_OFFSET_MOD_SUPC         = ILM_OFFSET_MOD_WNDRV  + ILM_NUM_MOD_WNDRV,
    ILM_OFFSET_MOD_HOSTAP       = ILM_OFFSET_MOD_SUPC   + ILM_NUM_MOD_SUPC,
    ILM_OFFSET_MOD_ABM          = ILM_OFFSET_MOD_HOSTAP + ILM_NUM_MOD_HOSTAP,
    ILM_OFFSET_MOD_WNDRV_HISR   = ILM_OFFSET_MOD_ABM    + ILM_NUM_MOD_ABM,
    ILM_OFFSET_MOD_TCPIP        = ILM_OFFSET_MOD_WNDRV_HISR + ILM_NUM_MOD_WNDRV_HISR,
    ILM_OFFSET_MOD_BWCS         = ILM_OFFSET_MOD_TCPIP  + ILM_NUM_MOD_TCPIP,
    ILM_OFFSET_MOD_TIMER        = ILM_OFFSET_MOD_BWCS   + ILM_NUM_MOD_BWCS,
    ILM_OFFSET_MOD_NIL          = ILM_OFFSET_MOD_TIMER  + ILM_NUM_MOD_TIMER,
    ILM_OFFSET_MOD_FT           = ILM_OFFSET_MOD_NIL    + ILM_NUM_MOD_NIL,
    ILM_OFFSET_MOD_MMI          = ILM_OFFSET_MOD_FT     + ILM_NUM_MOD_FT,
    ILM_OFFSET_MOD_BT           = ILM_OFFSET_MOD_MMI    + ILM_NUM_MOD_MMI,
    ILM_OFFSET_MOD_NVRAM        = ILM_OFFSET_MOD_BT     + ILM_NUM_MOD_BT,
    ILM_OFFSET_MOD_TIMER_HOSTAP = ILM_OFFSET_MOD_NVRAM  + ILM_NUM_MOD_NVRAM,
    ILM_OFFSET_MOD_END          = ILM_OFFSET_MOD_TIMER_HOSTAP + ILM_NUM_MOD_TIMER_HOSTAP
} ilm_module_offset;

static unsigned short ilm_num_table[] = {
    ILM_NUM_MOD_WNDRV,
    ILM_NUM_MOD_SUPC,
    ILM_NUM_MOD_HOSTAP,
    ILM_NUM_MOD_ABM,
    ILM_NUM_MOD_WNDRV_HISR,
    ILM_NUM_MOD_TCPIP,
    ILM_NUM_MOD_BWCS,
    ILM_NUM_MOD_TIMER,
    ILM_NUM_MOD_NIL,
    ILM_NUM_MOD_FT,
    ILM_NUM_MOD_MMI,
    ILM_NUM_MOD_BT,
    ILM_NUM_MOD_NVRAM,
    ILM_NUM_MOD_TIMER_HOSTAP
};

static unsigned short ilm_offset_table[] = {
    ILM_OFFSET_MOD_WNDRV,
    ILM_OFFSET_MOD_SUPC,
    ILM_OFFSET_MOD_HOSTAP,
    ILM_OFFSET_MOD_ABM,
    ILM_OFFSET_MOD_WNDRV_HISR,
    ILM_OFFSET_MOD_TCPIP,
    ILM_OFFSET_MOD_BWCS,
    ILM_OFFSET_MOD_TIMER,
    ILM_OFFSET_MOD_NIL,
    ILM_OFFSET_MOD_FT,
    ILM_OFFSET_MOD_MMI,
    ILM_OFFSET_MOD_BT,
    ILM_OFFSET_MOD_NVRAM,
    ILM_OFFSET_MOD_TIMER_HOSTAP
};

ILM_STRUCT_ARRAY    ilm_struct_array[ILM_NUM_MOD_TOTAL];
ILM_STRUCT_MOD_CNT  ilm_struct_mod_cnt[MOD_TOTAL_NUM];

log_create_module(wifi, PRINT_LEVEL_INFO);
log_create_module(supc, PRINT_LEVEL_INFO);
kal_int32 kal_create_sem(kal_sem *sem, unsigned char count)
{
#if 0//Need implement
    *sem = OSSemCreate((unsigned short)count);
    if (*sem == NULL) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "kal_create_sem FAIL:%d\n", count);
        return -1;
    }
    return 0;
#endif
    SemaphoreHandle_t ret = NULL;

    ret = xSemaphoreCreateCounting((UBaseType_t)count, 0);

    if (ret == NULL) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s kal_create_sem FAIL!%d\n", __func__);
    }

    *sem = (kal_sem)ret;

    return (kal_int32)ret;

}

kal_uint32 kal_take_sem(kal_sem *sem, unsigned int timeout)
{
#if 0//Need implement
    unsigned char ucErr;
    unsigned int ucos_timeout, timeout_new;

    if (timeout != 0) {
        ucos_timeout = (timeout * OS_TICKS_PER_SEC) / 1000; // convert to timetick
        if (ucos_timeout < 1) {
            ucos_timeout = 1;
        }
    } else {
        ucos_timeout = 0;
    }

    timeout = OSTimeGet();

    OSSemPend(*sem, (unsigned short)ucos_timeout, (unsigned char *)&ucErr);
    /*  only when timeout! */
    if (ucErr == OS_TIMEOUT) {
        KAL_PUBLIC_TRACE(TRACE_WARNING, "kal_take_sem:timerout %d\n", timeout);
        timeout = 0xffffffffUL;
    } else {

        /* for pbuf_free, may be called from an ISR */
        timeout_new = OSTimeGet();
        if (timeout_new >= timeout) {
            timeout_new = timeout_new - timeout;
        } else {
            timeout_new = 0xffffffff - timeout + timeout_new;
        }
        /* convert to milisecond */
        timeout = (timeout_new * 1000 / OS_TICKS_PER_SEC + 1);
    }
    return timeout;
#endif
    BaseType_t ret;
    ret = xSemaphoreTake((SemaphoreHandle_t) * sem, timeout);
    if (ret != pdTRUE) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s kal_take_sem FAIL! ret:%d\n", __func__, ret);
    }

    return ret;

}
void kal_give_sem(kal_sem *sem)
{
#if 0//Need implement
    OSSemPost(*sem);
#endif
    BaseType_t ret;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    ret = xSemaphoreGiveFromISR((SemaphoreHandle_t) * sem, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken != pdTRUE) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s kal_give_sem FAIL! ret:%d\n", __func__, ret);
    }
}

void kal_delete_sem(kal_sem *sem)
{
#if 0//Need implement
    unsigned char     ucErr;
    (void)OSSemDel(*sem, OS_DEL_ALWAYS, &ucErr);
    *sem = NULL;
#endif
    vSemaphoreDelete((SemaphoreHandle_t)*sem);
}


kal_mutexid kal_create_mutex(kal_uint8 prio)
{
#if 0//Need implement
    unsigned char ret = 0;
    return OSMutexCreate(prio, &ret);
#endif
    SemaphoreHandle_t mutex_ptr = NULL;
    mutex_ptr = xSemaphoreCreateBinary();
    if (mutex_ptr == NULL) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s kal_create_mutex FAIL!%d\n", __func__);
    }
    return (kal_mutexid)mutex_ptr;

}

void kal_take_mutex(kal_mutexid mutex_ptr)
{
#if 0//Need implement
    unsigned char ret = 0;
    OSMutexPend(mutex_ptr, 0, &ret);
#endif
    BaseType_t ret;
    ret = xSemaphoreTake((SemaphoreHandle_t)mutex_ptr, portMAX_DELAY);
    if (ret != pdTRUE) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s kal_take_mutex FAIL! ret:%d\n", __func__, ret);
    }
}
void kal_give_mutex(kal_mutexid mutex_ptr)
{
#if 0//Need implement
    OSMutexPost(mutex_ptr);
#endif
    BaseType_t ret;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    ret = xSemaphoreGiveFromISR((SemaphoreHandle_t)mutex_ptr, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken != pdTRUE) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s kal_give_mutex FAIL! ret:%d\n", __func__, ret);
    }
}

kal_uint8  kal_create_task(void (*taskproc)(void *param), KAL_TASK_CFG *task_cfg,
                           void *param)
{
#if 0//Need implement
    kal_uint8 ret;

    if (task_cfg->ubPriority > OS_LOWEST_PRIO) {
        return 0xFE;
    }

    ret = OSTaskCreateExt(taskproc,
                          param,
                          (OS_STK *)(task_cfg->ptos),
                          task_cfg->ubPriority,
                          task_cfg->ubPriority,
                          (OS_STK *)(task_cfg->pbos),
                          (task_cfg->ptos - task_cfg->pbos) / sizeof(OS_STK) + 1,
                          (void *) 0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    if (OS_NO_ERR != ret) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s FAIL! ret:%d\n", __func__, ret);
        return 0xFF;
    }

    OSTaskNameSet(task_cfg->ubPriority, task_cfg->pTaskName, &ret);
    if (OS_NO_ERR != ret) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s - OSTaskNameSet FAIL! ret:%d\n", __func__, ret);
        return 0xFF;
    }

    return task_cfg->ubPriority;
#endif
    BaseType_t ret_val = xTaskCreate(taskproc,
                                     task_cfg->pTaskName,
                                     task_cfg->usStackDepth / 4,
                                     param,
                                     task_cfg->ubPriority,
                                     NULL);

    if (pdPASS != ret_val) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "%s xTaskCreate fail.\n", __func__);
        return 0xFF;
    }

    return task_cfg->ubPriority;

}

void kal_sleep_task(kal_uint32 time_in_ticks)
{
#if 0//Need implement

    unsigned short msDelay;

    if (time_in_ticks >= 600) {
        KAL_PUBLIC_TRACE(TRACE_WARNING, "%s:%d, Warning!\n", __func__, time_in_ticks);
    }
    while (time_in_ticks > 0) {
        msDelay = (time_in_ticks < 0xFFFF) ? time_in_ticks : 0XFFFF;
        OSTimeDly(msDelay);
        time_in_ticks -= msDelay;
    }
#endif
    vTaskDelay(time_in_ticks);

}
void kal_delete_task(TaskHandle_t task)
{
    vTaskDelete(task);
}

void kal_task_stack_chk(unsigned char prio)
{
#if 0//Need implement
    OS_TCB ostcb;
    OSTaskQuery(prio, &ostcb);
    KAL_PUBLIC_TRACE(TRACE_INFO, "OSTCBStkSize:0x%X, OSTCBStkBottom:0x%X,OSTCBStkPtr:0x%X\n",
                     ostcb.OSTCBStkSize, ostcb.OSTCBStkBottom, ostcb.OSTCBStkPtr);
#endif

}

#define EXT_ASSERT_TRACE_FUNC(exp, e1, e2, e3)                              \
    do {                                                        \
        if (!(exp))                                             \
            printc(RED_ON"[MT5931] ASSERT FAIL:"#exp", %s, 0x%X, 0x%X\n"RED_OFF, e1, e2, e3)    ; \
    } while (0)

#if (TRACE_BUFF_DEBUG == 0)

void *GET_BUFFER(unsigned long size, char *fun)
{
    void *ptr = NULL;

    ptr = MEM_MALLOC(size);
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[M_M]0x%X,%d\n", ptr, size);
    EXT_ASSERT_TRACE_FUNC(ptr, fun, ptr, size);
    return (ptr);
}

void FREE_BUFFER(void *ptr, char *fun)
{
    ASSERT(ptr);
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[M_F]0x%X\n", ptr);
    EXT_ASSERT_TRACE_FUNC(ptr, fun, ptr, 0);
    MEM_FREE(ptr);
    ptr = NULL;
}
#else

#define TEST_MAGIC_DATA     0xAB
#define TEST_MAGIC_CNT      8

#define ILM_RECORD_SEM_TIMEOUT  100
static kal_sem ilm_record_sem;

void *GET_BUFFER(unsigned long size, char *fun)
{
    void *ptr = NULL;
    unsigned char *testptr = NULL;

    ptr = MEM_MALLOC(size + TEST_MAGIC_CNT * 2);
    EXT_ASSERT_TRACE_FUNC(ptr, fun, ptr, size);

    testptr = (unsigned char *)ptr;
    *testptr = (unsigned char)(size & 0xFF);
    *(testptr + 1) = (unsigned char)((size >> 8) & 0xFF);
    memset(testptr + 2, TEST_MAGIC_DATA, TEST_MAGIC_CNT - 2);

    testptr = (unsigned char *)ptr + TEST_MAGIC_CNT + size;
    *testptr = (unsigned char)(size & 0xFF);
    *(testptr + 1) = (unsigned char)((size >> 8) & 0xFF);
    memset(testptr + 2, TEST_MAGIC_DATA, TEST_MAGIC_CNT - 2);

    //KAL_PUBLIC_TRACE(TRACE_BUFF,"[M_M]0x%X,%d\n",(unsigned char *)ptr + TEST_MAGIC_CNT,size);
    return (void *)((unsigned char *)ptr + TEST_MAGIC_CNT);
}

void FREE_BUFFER(void *ptr, char *fun)
{
    void *original_ptr = (void *)((unsigned char *)ptr - TEST_MAGIC_CNT);

    //KAL_PUBLIC_TRACE(TRACE_BUFF,"[M_F]0x%X\n",ptr);
    EXT_ASSERT_TRACE_FUNC(ptr, fun, ptr, 0);
    MEM_FREE(original_ptr);
}

#define ILM_RECORD_NUM  15
typedef struct {
    unsigned int id;
    unsigned int used;
    unsigned int peer_ptr_change_flag;
    sap_type sap;
    msg_type msg_id;
    ilm_struct *ilm_ptr;
    local_para_struct *local_para_ptr; /* local_para pointer */
    peer_buff_struct  *peer_buff_ptr;  /* peer_buff pointer */
} ILM_RECORD;

ILM_RECORD ilmRecord[ILM_RECORD_NUM];

static unsigned char check_data_err_flag = FALSE;

#define EXT_ASSERT_TEST(exp, e1, e2, e3, e4, e5, e6)            \
    do {                                                        \
        if (!(exp))                                             \
        {                                                       \
            check_data_err_flag = TRUE;                         \
            printc(RED_ON"[ILM_BUF%d] ASSERT FAIL:"#exp", 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n"RED_OFF,e1, e2, e3, e4, e5, e6)    ; \
            return;                                             \
        }                                                       \
    } while (0)


void ILMChangeCheck(ILM_STRUCT_ARRAY *ilm_strcut_array_ptr, unsigned short point)
{
    EXT_ASSERT_TEST((ilm_strcut_array_ptr->used == 1), point, ilm_strcut_array_ptr, ilm_strcut_array_ptr->used, ilm_strcut_array_ptr->mod, 0, 0);
}

void BuffChangeCheck(unsigned char *ptr, unsigned short point)
{
    unsigned char i = 0;
    unsigned short size = 0;
    unsigned short size_after = 0;
    unsigned char *testptr = NULL;

    testptr = (unsigned char *)ptr - TEST_MAGIC_CNT;
    size = *testptr + ((*(testptr + 1)) << 8);
    for (i = 2; i < TEST_MAGIC_CNT; i++) {
        EXT_ASSERT_TEST((*(testptr + i) == TEST_MAGIC_DATA), point, ptr, testptr, size, i, *(testptr + i));
    }

    testptr = (unsigned char *)ptr + size;
    size_after = *testptr + ((*(testptr + 1)) << 8);
    for (i = 2; i < TEST_MAGIC_CNT; i++) {
        EXT_ASSERT_TEST((*(testptr + i) == TEST_MAGIC_DATA), point, ptr, testptr, size_after, i, *(testptr + i));
    }

    EXT_ASSERT_TEST((size == size_after), point, (unsigned char *)ptr, size, size_after, 0, 0);

}

void ILMTraceInitial(void)
{
    unsigned int i = 0;

    for (i = 0; i < ILM_RECORD_NUM; i++) {
        ilmRecord[i].id = ILM_RECORD_NUM + 1;
        ilmRecord[i].used = FALSE;
        ilmRecord[i].peer_ptr_change_flag = FALSE;
        ilmRecord[i].sap = INVALID_SAP;
        ilmRecord[i].msg_id = 0;
        ilmRecord[i].ilm_ptr = NULL;
        ilmRecord[i].local_para_ptr = NULL;
        ilmRecord[i].peer_buff_ptr = NULL;
    }
    kal_create_sem(&ilm_record_sem, 1);
}

void ILMTraceRecord(ilm_struct *ilm_ptr)
{
    unsigned int i = 0;
    static unsigned char initalFlag = FALSE;

    if (initalFlag == FALSE) {
        ILMTraceInitial();
        initalFlag = TRUE;
    }

    if (ilm_ptr->src_mod_id == MOD_TIMER) {
        return;
    }

    kal_take_sem(&ilm_record_sem, ILM_RECORD_SEM_TIMEOUT);
    for (i = 0; i < ILM_RECORD_NUM; i++) {
        if (ilmRecord[i].used == FALSE) {
            ilmRecord[i].used = TRUE;
            ilmRecord[i].id = i;
            ilmRecord[i].peer_ptr_change_flag = FALSE;
            ilmRecord[i].sap = ilm_ptr->sap_id;
            ilmRecord[i].msg_id = ilm_ptr->msg_id;
            ilmRecord[i].ilm_ptr = ilm_ptr;
            ilmRecord[i].local_para_ptr = ilm_ptr->local_para_ptr;
            ilmRecord[i].peer_buff_ptr = ilm_ptr->peer_buff_ptr;
            //KAL_PUBLIC_TRACE(TRACE_BUFF,"[I_R]0x%X msg:%d\n",ilm_ptr,ilm_ptr->msg_id);
            kal_give_sem(&ilm_record_sem);
            return;
        }
    }

    kal_give_sem(&ilm_record_sem);
    KAL_PUBLIC_TRACE(TRACE_ERROR, "[I_B1000]FAIL:no enough buff to record:0x%X msg_id:%d\n", ilm_ptr, ilm_ptr->msg_id);
}

static kal_bool check_wifi_buff_flag = FALSE;
void ILMTraceSetCheckflag(kal_bool checkflag)
{
    check_wifi_buff_flag = checkflag;
}

void ILMTraceCheck(unsigned short point)
{
    unsigned int i = 0;

    if (check_wifi_buff_flag == FALSE) {
        return;
    }

    kal_take_sem(&ilm_record_sem, ILM_RECORD_SEM_TIMEOUT);
    for (i = 0; i < ILM_RECORD_NUM; i++) {
        if (ilmRecord[i].used == TRUE) {
            check_data_err_flag = FALSE;
            if (ilmRecord[i].sap != STACK_TIMER_SAP) {
                ILMChangeCheck((ILM_STRUCT_ARRAY *)(ilmRecord[i].ilm_ptr), 3000 + point);
            }
            if (ilmRecord[i].local_para_ptr != ilmRecord[i].ilm_ptr->local_para_ptr) {
                KAL_PUBLIC_TRACE(TRACE_WARNING, "[I_B%d]PARA ptr already changed,ilm:0x%X; para:0x%X; ilm_ptr->para:0x%X\n", 7000 + point, ilmRecord[i].ilm_ptr, ilmRecord[i].local_para_ptr, ilmRecord[i].ilm_ptr->local_para_ptr);
            }
            if (ilmRecord[i].peer_buff_ptr != ilmRecord[i].ilm_ptr->peer_buff_ptr) {
                if (ilmRecord[i].peer_ptr_change_flag == FALSE) {
                    if (ilmRecord[i].msg_id != MSG_ID_TCPIP_WNDRV_DATA_REQ) { //TODO  for special case
                        KAL_PUBLIC_TRACE(TRACE_WARNING, "[I_B%d]BUFF ptr already changed,ilm:0x%X; buff:0x%X; ilm_ptr->buff:0x%X\n", 7000 + point, ilmRecord[i].ilm_ptr, ilmRecord[i].peer_buff_ptr, ilmRecord[i].ilm_ptr->peer_buff_ptr);
                    }
                }
            }
            if (ilmRecord[i].local_para_ptr) {
                if (ilmRecord[i].ilm_ptr->local_para_ptr->ref_count != 0) {
                    BuffChangeCheck((unsigned char *)ilmRecord[i].ilm_ptr->local_para_ptr, 4000 + point);
                } else {
                    KAL_PUBLIC_TRACE(TRACE_WARNING, "[I_B%d]PARA already released,ilm:0x%X; para:0x%X 0x%X; buff:0x%X 0x%X\n", 6000 + point, ilmRecord[i].ilm_ptr, ilmRecord[i].local_para_ptr, ilmRecord[i].ilm_ptr->local_para_ptr, ilmRecord[i].peer_buff_ptr, ilmRecord[i].ilm_ptr->peer_buff_ptr);
                }
            }
            if ((ilmRecord[i].peer_buff_ptr) && (ilmRecord[i].sap != WNDRV_TCPIP_SAP)) {
                if (ilmRecord[i].ilm_ptr->peer_buff_ptr->ref_count != 0) {
                    BuffChangeCheck((unsigned char *)ilmRecord[i].ilm_ptr->peer_buff_ptr, 5000 + point);
                } else {
                    KAL_PUBLIC_TRACE(TRACE_WARNING, "[I_B%d]BUFF already released,ilm:0x%X; para:0x%X 0x%X; buff:0x%X 0x%X\n", 6000 + point, ilmRecord[i].ilm_ptr, ilmRecord[i].local_para_ptr, ilmRecord[i].ilm_ptr->local_para_ptr, ilmRecord[i].peer_buff_ptr, ilmRecord[i].ilm_ptr->peer_buff_ptr);
                }
            }

            if (check_data_err_flag == TRUE) {
                KAL_PUBLIC_TRACE(TRACE_WARNING, "[I_B%d]ilm:0x%X; para:0x%X 0x%X; buff:0x%X 0x%X\n", 6000 + point, ilmRecord[i].ilm_ptr, ilmRecord[i].local_para_ptr, ilmRecord[i].ilm_ptr->local_para_ptr, ilmRecord[i].peer_buff_ptr, ilmRecord[i].ilm_ptr->peer_buff_ptr);
            }
        }
    }

    kal_give_sem(&ilm_record_sem);
    //KAL_PUBLIC_TRACE(TRACE_BUFF,"[I_CK%d]%d\n",point,recordcnt);
}

void ILMTraceCancel(ilm_struct *ilm_ptr)
{
    unsigned int i = 0;

    kal_take_sem(&ilm_record_sem, ILM_RECORD_SEM_TIMEOUT);
    for (i = 0; i < ILM_RECORD_NUM; i++) {
        if (ilmRecord[i].used == TRUE) {
            if (ilmRecord[i].ilm_ptr == ilm_ptr) {
                ilmRecord[i].used = FALSE;
                ilmRecord[i].sap = INVALID_SAP;
                ilmRecord[i].peer_ptr_change_flag = FALSE;
                ilmRecord[i].msg_id = 0;
                ilmRecord[i].ilm_ptr = NULL;
                ilmRecord[i].local_para_ptr = NULL;
                ilmRecord[i].peer_buff_ptr = NULL;
                //KAL_PUBLIC_TRACE(TRACE_BUFF,"[I_C]0x%X\n",ilm_ptr);
                kal_give_sem(&ilm_record_sem);
                return;
            }
        }
    }

    kal_give_sem(&ilm_record_sem);
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[I_B2000]no record found to free :0x%X msg_id:%d\n", ilm_ptr, ilm_ptr->msg_id);
}


void ILMTraceCancelPeerBuff(peer_buff_struct *peer_buff_ptr)
{
    unsigned int i = 0;

    kal_take_sem(&ilm_record_sem, ILM_RECORD_SEM_TIMEOUT);
    for (i = 0; i < ILM_RECORD_NUM; i++) {
        if (ilmRecord[i].used == TRUE) {
            if (ilmRecord[i].peer_buff_ptr == peer_buff_ptr) {
                ilmRecord[i].peer_buff_ptr = NULL;
                ilmRecord[i].peer_ptr_change_flag = TRUE;
                //KAL_PUBLIC_TRACE(TRACE_BUFF,"[I_C_P]0x%X(0x%X)\n", ilmRecord[i].ilm_ptr, peer_buff_ptr);
                kal_give_sem(&ilm_record_sem);
                return;
            }
        }
    }

    kal_give_sem(&ilm_record_sem);
    //KAL_PUBLIC_TRACE(TRACE_BUFF,"[I_B2000_P]no record found to free, maybe flc2 bufff allocate by wndrv :0x%X\n",peer_buff_ptr);
}

#endif


//for this os: 1tick=1ms
#define KAL_TIMER_PERIOD    200     //ms    
//if for a long timer, no internal module timer used, also send MSG_ID_TIMER_EXPIRY
#define KAL_TIMER_TIMEROUT_THRESHOLD    (4000/KAL_TIMER_PERIOD)
#define KAL_MAX_SCHED_NUM   3
#define KAL_MAX_TIMER_NUM_PER_SCHED 13

typedef struct {
    kal_timer_func_ptr  kalTimerFunc;
    kal_uint32          interval;
    kal_uint32          triggerCnt;
    kal_uint32          cnt;
    kal_bool            timeoutflag;
    kal_bool            set;
    void                *timerhandle;
    kal_uint8           timer_id;
    void                *sched_ptr;
} KAL_OS_TIMER;

typedef struct {
    event_scheduler     scheduler;
    unsigned char       used;       //whether this sched was used
    module_type         module_id;  //which module this sched used for
    KAL_OS_TIMER        timerTab[KAL_MAX_TIMER_NUM_PER_SCHED]; //module internal timer
} KAL_OS_SCHED;
KAL_OS_SCHED kal_os_sched[KAL_MAX_SCHED_NUM];


ilm_struct timer_ilm_struct;

static void KalTimerCallback(TimerHandle_t ptimer)//(void *ptmr, void *parg)
{

#if 0//Need implement
    ilm_struct *ilm_p;
    KAL_OS_SCHED *kal_os_sched_ptr = (KAL_OS_SCHED *)parg;
    module_type dst_mod = kal_os_sched_ptr->module_id;
    unsigned char timer_id = 0;
    unsigned char timer_send_msg_flag = KAL_FALSE;

    for (timer_id = 0; timer_id < KAL_MAX_TIMER_NUM_PER_SCHED; timer_id++) {
        if (kal_os_sched_ptr->timerTab[timer_id].set == TRUE) {
            if (kal_os_sched_ptr->timerTab[timer_id].timeoutflag == FALSE) {
                if (kal_os_sched_ptr->timerTab[timer_id].cnt++ >= kal_os_sched_ptr->timerTab[timer_id].triggerCnt) {
                    kal_os_sched_ptr->timerTab[timer_id].cnt = 0;
                    kal_os_sched_ptr->timerTab[timer_id].timeoutflag = TRUE;
                    timer_send_msg_flag = KAL_TRUE;
                }
            }
        }
    }

    if (timer_send_msg_flag == KAL_TRUE) {
        ilm_p = &timer_ilm_struct;

        ilm_p->msg_id         = MSG_ID_TIMER_EXPIRY;
        ilm_p->local_para_ptr = NULL;
        ilm_p->peer_buff_ptr  = NULL;

        SEND_ILM(MOD_TIMER, dst_mod, STACK_TIMER_SAP, ilm_p);
    }
#endif
    ilm_struct *ilm_p;
    KAL_OS_TIMER *timer_tab = (KAL_OS_TIMER *)pvTimerGetTimerID(ptimer);
    KAL_OS_SCHED *kal_os_sched_ptr = (KAL_OS_SCHED *)timer_tab->sched_ptr;
    module_type dst_mod = kal_os_sched_ptr->module_id;
    unsigned char timer_id = 0;
    unsigned char timer_send_msg_flag = KAL_FALSE;

    for (timer_id = 0; timer_id < KAL_MAX_TIMER_NUM_PER_SCHED; timer_id++) {
        if (kal_os_sched_ptr->timerTab[timer_id].set == TRUE) {
            if (kal_os_sched_ptr->timerTab[timer_id].timeoutflag == FALSE) {
                if (kal_os_sched_ptr->timerTab[timer_id].cnt++ >= kal_os_sched_ptr->timerTab[timer_id].triggerCnt) {
                    kal_os_sched_ptr->timerTab[timer_id].cnt = 0;
                    kal_os_sched_ptr->timerTab[timer_id].timeoutflag = TRUE;
                    timer_send_msg_flag = KAL_TRUE;
                }
            }
        }
    }

    if (timer_send_msg_flag == KAL_TRUE) {
        ilm_p = &timer_ilm_struct;

        ilm_p->msg_id         = MSG_ID_TIMER_EXPIRY;
        ilm_p->local_para_ptr = NULL;
        ilm_p->peer_buff_ptr  = NULL;

        SEND_ILM(MOD_TIMER, dst_mod, STACK_TIMER_SAP, ilm_p);
    }

}

event_scheduler *evshed_create(kal_char *evshed_name, module_type module_id,
                               kal_uint32 allowed_time_shift, kal_uint16 allowed_time_shift_sleep_mode)
{
#if 0//Need implement
    unsigned short i = 0;
    unsigned char ret = 0;

    //find a not used scheduler, then create a OS timer
    for (i = 0; i < KAL_MAX_SCHED_NUM; i++) {
        if (kal_os_sched[i].used == FALSE) {
            break;
        }
    }

    if (i <  KAL_MAX_SCHED_NUM) {
        kal_os_sched[i].used = TRUE;
        kal_os_sched[i].module_id = module_id;
        kal_os_sched[i].scheduler.tmr = OSTmrStart(KAL_TIMER_PERIOD,
                                        OS_TMR_OPT_PERIODIC,
                                        KalTimerCallback,
                                        (void *)&kal_os_sched[i], (unsigned char *)evshed_name, &ret);
    } else {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "evshed_create:exceed KAL_MAX_SCHED_NUM\n");
        return NULL;
    }

    return (&kal_os_sched[i].scheduler);
#endif
    unsigned short i = 0;

    //find a not used scheduler, then create a OS timer
    for (i = 0; i < KAL_MAX_SCHED_NUM; i++) {
        if (kal_os_sched[i].used == FALSE) {
            break;
        }
    }

    if (i <  KAL_MAX_SCHED_NUM) {
        kal_os_sched[i].used = TRUE;
        kal_os_sched[i].module_id = module_id;
        kal_os_sched[i].scheduler.tmr = NULL;
        kal_mem_set(kal_os_sched[i].timerTab, 0, sizeof(KAL_OS_TIMER)*KAL_MAX_TIMER_NUM_PER_SCHED);
    } else {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "evshed_create:exceed KAL_MAX_SCHED_NUM\n");
        return NULL;
    }

    return (&kal_os_sched[i].scheduler);


}

eventid evshed_set_event(event_scheduler *es,
                         kal_timer_func_ptr event_hf,
                         void *event_hf_param,
                         kal_uint32 elapse_time)
{
#if 0//Need implement
    KAL_OS_SCHED *kal_os_sched_ptr = (KAL_OS_SCHED *)es;    //todo  ?
    unsigned char timer_id;
    if (kal_os_sched_ptr->module_id == MOD_WNDRV) {
        timer_id = *(unsigned char *)event_hf_param;
    } else if (kal_os_sched_ptr->module_id == MOD_HOSTAP || kal_os_sched_ptr->module_id == MOD_SUPC) {
        timer_id = (int) event_hf_param;
    } else {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "unexpected module id\n");
    }
    if ((!es) || (timer_id > KAL_MAX_TIMER_NUM_PER_SCHED)) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "evshed_set_event:exceed KAL_MAX_TIMER_NUM_PER_SCHED, timer_id = %d\n", timer_id);
        return NULL;
    }

    kal_os_sched_ptr->timerTab[timer_id].kalTimerFunc = event_hf;
    kal_os_sched_ptr->timerTab[timer_id].interval = elapse_time;
    kal_os_sched_ptr->timerTab[timer_id].triggerCnt = elapse_time / KAL_TIMER_PERIOD;
    kal_os_sched_ptr->timerTab[timer_id].cnt = 0;
    kal_os_sched_ptr->timerTab[timer_id].timeoutflag = FALSE;
    kal_os_sched_ptr->timerTab[timer_id].set = TRUE;
    KAL_PUBLIC_TRACE(TRACE_GROUP_1, "module[%d] set timer id = %d, elapse_time = %d\n", kal_os_sched_ptr->module_id , timer_id, elapse_time);
    if (kal_os_sched_ptr->module_id == MOD_WNDRV) {
        return event_hf_param;
    } else if (kal_os_sched_ptr->module_id == MOD_HOSTAP || kal_os_sched_ptr->module_id == MOD_SUPC) {
        return (eventid)timer_id;
    } else {
        return NULL;
    }
#endif
    TimerHandle_t event_timer = NULL;
    KAL_OS_SCHED *kal_os_sched_ptr = (KAL_OS_SCHED *)es;
    kal_uint8 timer_id;

    if (kal_os_sched_ptr->module_id == MOD_WNDRV) {
        timer_id = *(unsigned char *)event_hf_param;
    } else if (kal_os_sched_ptr->module_id == MOD_HOSTAP || kal_os_sched_ptr->module_id == MOD_SUPC) {
        timer_id = (int) event_hf_param;
    } else {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "unexpected module id\n");
        return NULL;
    }
    if ((!es) || (timer_id >= KAL_MAX_TIMER_NUM_PER_SCHED)) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "evshed_set_event:exceed KAL_MAX_TIMER_NUM_PER_SCHED, timer_id = %d\n", timer_id);
        return NULL;
    }

    event_timer = kal_os_sched_ptr->timerTab[timer_id].timerhandle;

    if (elapse_time == 0) {
        elapse_time = 1;
    }

    if (event_timer && (xTimerIsTimerActive(event_timer) != pdFALSE)) {
        xTimerStop(event_timer, 0);
        xTimerChangePeriod(event_timer, elapse_time / portTICK_PERIOD_MS, 0);
    } else if(event_timer && (xTimerIsTimerActive(event_timer) == pdFALSE)) {
        xTimerChangePeriod(event_timer, elapse_time / portTICK_PERIOD_MS, 0);
    } else if (event_timer == NULL) {
        kal_os_sched_ptr->timerTab[timer_id].timer_id = timer_id;

        event_timer = xTimerCreate("wifi timer",
                                   elapse_time / portTICK_PERIOD_MS,
                                   pdFALSE,
                                   (void *)&kal_os_sched_ptr->timerTab[timer_id],
                                   KalTimerCallback);
        if (event_timer == NULL) {
            KAL_PUBLIC_TRACE(TRACE_ERROR, "create timer: return NULL\n");
            return NULL;
        }
    }

    xTimerStart(event_timer, 0);
    kal_os_sched_ptr->timerTab[timer_id].kalTimerFunc = event_hf;
    kal_os_sched_ptr->timerTab[timer_id].interval = elapse_time;
    kal_os_sched_ptr->timerTab[timer_id].triggerCnt = 0;
    kal_os_sched_ptr->timerTab[timer_id].cnt = 0;
    kal_os_sched_ptr->timerTab[timer_id].timeoutflag = FALSE;
    kal_os_sched_ptr->timerTab[timer_id].set = TRUE;
    kal_os_sched_ptr->timerTab[timer_id].timerhandle = event_timer;
    kal_os_sched_ptr->timerTab[timer_id].timer_id = timer_id;
    kal_os_sched_ptr->timerTab[timer_id].sched_ptr = kal_os_sched_ptr;

    KAL_PUBLIC_TRACE(TRACE_GROUP_1, "module[%d] set timer id = %d, elapse_time = %d\n", kal_os_sched_ptr->module_id , timer_id, elapse_time);

    if (kal_os_sched_ptr->module_id == MOD_WNDRV) {
        return event_hf_param;
    } else if (kal_os_sched_ptr->module_id == MOD_HOSTAP || kal_os_sched_ptr->module_id == MOD_SUPC) {
        return (eventid)timer_id;
    } else {
        return NULL;
    }
    return NULL;

}


kal_int32 evshed_cancel_event(event_scheduler *es, eventid *eid)
{
#if 0//Need implement
    KAL_OS_SCHED *kal_os_sched_ptr = (KAL_OS_SCHED *)es;    //todo  ?
    unsigned char timer_id;
    if (kal_os_sched_ptr->module_id == MOD_WNDRV || kal_os_sched_ptr->module_id == MOD_HOSTAP) {
        timer_id = *((unsigned char *)(*eid));
    } else if (kal_os_sched_ptr->module_id == MOD_SUPC) {
        timer_id = (int)(*eid);
    }
    if ((!es) || (timer_id > KAL_MAX_TIMER_NUM_PER_SCHED)) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "evshed_cancel_event FAIL. 0x%X,%d\n", es , timer_id);
        return -1;
    }

    //cancel the selected timer
    kal_os_sched_ptr->timerTab[timer_id].set = FALSE;
    KAL_PUBLIC_TRACE(TRACE_GROUP_1, "module[%d] cancel timer id = %d\n", kal_os_sched_ptr->module_id , timer_id);

    return 0;
#endif
    KAL_OS_SCHED *kal_os_sched_ptr = (KAL_OS_SCHED *)es;    //todo  ?
    unsigned char timer_id;
    TimerHandle_t event_timer = NULL;
    if (kal_os_sched_ptr->module_id == MOD_WNDRV || kal_os_sched_ptr->module_id == MOD_HOSTAP) {
        timer_id = *((unsigned char *)(*eid));
    } else if (kal_os_sched_ptr->module_id == MOD_SUPC) {
        timer_id = (int)(*eid);
    } else {
        return -1;
    }
    if ((!es) || (timer_id > KAL_MAX_TIMER_NUM_PER_SCHED)) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "evshed_cancel_event FAIL. 0x%X,%d\n", es , timer_id);
        return -1;
    }

    //cancel the selected timer
    event_timer = kal_os_sched_ptr->timerTab[timer_id].timerhandle;
    xTimerStop(event_timer, 0);
    kal_os_sched_ptr->timerTab[timer_id].set = FALSE;
    KAL_PUBLIC_TRACE(TRACE_GROUP_1, "module[%d] cancel timer id = %d\n", kal_os_sched_ptr->module_id , timer_id);

    return 0;


}


void evshed_timer_handler(event_scheduler *es)
{
#if 0//Need implement
    KAL_OS_SCHED *kal_os_sched_ptr = (KAL_OS_SCHED *)es;    //todo  ?
    unsigned char timer_id = 0;
    if (!es) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "FAIL %s\n", __func__);
        return;
    }

    for (timer_id = 0; timer_id < KAL_MAX_TIMER_NUM_PER_SCHED; timer_id++) {
        if (kal_os_sched_ptr->timerTab[timer_id].set == TRUE) {
            if (kal_os_sched_ptr->timerTab[timer_id].timeoutflag == TRUE) {
                KAL_PUBLIC_TRACE(TRACE_GROUP_1, "module[%d] timer handle id = %d\n", kal_os_sched_ptr->module_id , timer_id);
                kal_os_sched_ptr->timerTab[timer_id].set = FALSE;

                if (kal_os_sched_ptr->module_id == MOD_WNDRV || kal_os_sched_ptr->module_id == MOD_HOSTAP) {
                    (*(kal_os_sched_ptr->timerTab[timer_id].kalTimerFunc))(&timer_id);
                } else if (kal_os_sched_ptr->module_id == MOD_SUPC) {
                    (*(kal_os_sched_ptr->timerTab[timer_id].kalTimerFunc))((void *)timer_id);
                }

                kal_os_sched_ptr->timerTab[timer_id].timeoutflag = FALSE;
            }
        }
    }
#endif
    KAL_OS_SCHED *kal_os_sched_ptr = (KAL_OS_SCHED *)es;    //todo  ?
    unsigned char timer_id = 0;
    if (!es) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "FAIL %s\n", __func__);
        return;
    }

    for (timer_id = 0; timer_id < KAL_MAX_TIMER_NUM_PER_SCHED; timer_id++) {
        if (kal_os_sched_ptr->timerTab[timer_id].set == TRUE) {
            if (kal_os_sched_ptr->timerTab[timer_id].timeoutflag == TRUE) {
                KAL_PUBLIC_TRACE(TRACE_GROUP_1, "module[%d] timer handle id = %d\n", kal_os_sched_ptr->module_id , timer_id);
                kal_os_sched_ptr->timerTab[timer_id].set = FALSE;

                if (kal_os_sched_ptr->module_id == MOD_WNDRV || kal_os_sched_ptr->module_id == MOD_HOSTAP) {
                    (*(kal_os_sched_ptr->timerTab[timer_id].kalTimerFunc))(&timer_id);
                } else if (kal_os_sched_ptr->module_id == MOD_SUPC) {
                    (*(kal_os_sched_ptr->timerTab[timer_id].kalTimerFunc))((void *)timer_id);
                }

                kal_os_sched_ptr->timerTab[timer_id].timeoutflag = FALSE;
            }
        }
    }

}


void *wifi_get_ctrl_buffer(unsigned long size, void *fun)
{
    void *ptr = NULL;

    ptr = MEM_MALLOC(size);
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[M_M_]0x%X,%d\n", ptr, size);
    EXT_ASSERT_TRACE_FUNC(ptr, fun, ptr, size);
    return (ptr);
}

void wifi_free_ctrl_buffer(void *ptr, void *fun)
{
    EXT_ASSERT_TRACE_FUNC(ptr, fun, ptr, 0);
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[M_F_]0x%X\n", ptr);
    if (ptr == NULL) {
        return;
    }
    MEM_FREE(ptr);
    ptr = NULL;
}




/*************************************************************************
* FUNCTION
*  construct_peer_buff
*
* DESCRIPTION
*   Allocates peer buffer of given size for U plane and C
*    plane messages.
*
* PARAMETERS
*  pdu_len: Size of the PDU data to allocate.
*  header_len: Size of the header space to allocate.
*  tail_len: Size of tail space to allocate.
*  direc: Tranfer direction (Uplink/Downlink/Control)
*
* RETURNS
*  Poniter to the peer_buff_struct or NULL
*
* GLOBALS AFFECTED
*
*************************************************************************/
void *construct_int_peer_buff(kal_uint16 pdu_len, kal_uint16  header_len,
                              kal_uint16 tail_len, kal_char *file_name_ptr,
                              kal_uint32 line)
{

    peer_buff_struct *peer_buff_ptr;

    EXT_ASSERT((pdu_len + header_len + tail_len) != 0, pdu_len, header_len, tail_len);

    if (pdu_len < 2048) {
        peer_buff_ptr = (peer_buff_struct *)GET_BUFFER((sizeof(*peer_buff_ptr) + header_len + 2048 + tail_len), (char *)__func__);
    } else {
        peer_buff_ptr = (peer_buff_struct *)GET_BUFFER((sizeof(*peer_buff_ptr) + header_len + pdu_len + tail_len), (char *)__func__);
    }
    ASSERT(peer_buff_ptr);
    if (peer_buff_ptr == NULL) {
        return NULL;
    }

    peer_buff_ptr->pb_resvered = 0;
    peer_buff_ptr->ref_count = 1;
    peer_buff_ptr->free_header_space = header_len;
    peer_buff_ptr->pdu_len = pdu_len;
    peer_buff_ptr->free_tail_space = tail_len;

    return (void *)peer_buff_ptr;

}

/*************************************************************************
* FUNCTION
*  construct_local_para
*
* DESCRIPTION
*   Allocates local parameters of given size for U plane and C
*  plane messages.
*
* PARAMETERS
*  local_para_size      -  Size of local parameters to be allocated.
*  trans_drctn          -  Tranfer direction (Uplink/Downlink/Control)
*
* RETURNS
*  Poniter to the local_para_struct or NULL
*
* GLOBALS AFFECTED
*
*************************************************************************/
void *construct_int_local_para(kal_uint16 local_para_size, kal_uint32 auto_reset,
                               kal_char *file_ptr, kal_uint32 line)
{

    local_para_struct *local_para;
    kal_uint8 *ptr;

    ASSERT(local_para_size != 0);

    local_para = (local_para_struct *)GET_BUFFER(local_para_size, (char *)__func__);
    ASSERT(local_para_size >= sizeof(local_para_struct));
    ASSERT(local_para);
    if (local_para == NULL) {
        return NULL;
    }



    /* reset buffer content to 0 */
    EXT_ASSERT(local_para_size >= sizeof(local_para_struct), 0, 0, 0);
    ptr = (kal_uint8 *)local_para;
    kal_mem_set(ptr + sizeof(local_para->ref_count), 0, local_para_size - sizeof(local_para->ref_count));


//   local_para->msg_len = local_para_size;
    local_para->ref_count  = 1;

    return (void *)local_para;

}


/*************************************************************************
* FUNCTION
*  allocate_ilm
*
* DESCRIPTION
*  Allocates an ilm_struct for the calling module from the pool of
*  the pool of task ilms.
*
* PARAMETERS
*  module_id      -  Module id of allocating module.
*
* RETURNS
*  Poniter to the allocated ILM (ilm_struct)
*
* GLOBALS AFFECTED
*
*************************************************************************/
ilm_struct *allocate_ilm(module_type module_id)
{
    static unsigned char ilm_array_init_flag = FALSE;
    unsigned int i = 0;
    ILM_STRUCT_ARRAY *ilm_array_ptr = NULL;
    unsigned short ilm_num_mod = 0;
    unsigned short ilm_offset_mod = 0;

    ASSERT(module_id < MOD_TOTAL_NUM);
    if (ilm_array_init_flag == FALSE) {
        ASSERT((kal_uint32)ILM_NUM_MOD_TOTAL == (kal_uint32)ILM_OFFSET_MOD_END);
        memset(&ilm_struct_array[0], 0, sizeof(ILM_STRUCT_ARRAY)*ILM_NUM_MOD_TOTAL);
        KAL_PUBLIC_TRACE(TRACE_BUFF, "Init ilm_struct_array:%d * %d\n", sizeof(ILM_STRUCT_ARRAY), ILM_NUM_MOD_TOTAL);
        ilm_array_init_flag = TRUE;
        for (i = 0; i < MOD_TOTAL_NUM; i++) {
            KAL_PUBLIC_TRACE(TRACE_BUFF, "MOD:%d,num:%d,ptr:0x%X\n", i, ilm_num_table[i], &ilm_struct_array[ilm_offset_table[i]]);
        }
        memset(&ilm_struct_mod_cnt[0], 0, sizeof(ILM_STRUCT_MOD_CNT)*MOD_TOTAL_NUM);
    }

    ilm_num_mod = ilm_num_table[module_id];
    ilm_offset_mod = ilm_offset_table[module_id];
    ilm_array_ptr = &ilm_struct_array[ilm_offset_mod];


    for (i = 0; i < ilm_num_mod; i++) {
        if (ilm_array_ptr[i].used == 0) {
            ilm_array_ptr[i].used = 1;
            ilm_array_ptr[i].mod = module_id;
            ilm_array_ptr[i].ilm.local_para_ptr = NULL;
            ilm_array_ptr[i].ilm.peer_buff_ptr = NULL;
            ilm_struct_mod_cnt[module_id].alloc_cnt++;
            if (ilm_struct_mod_cnt[module_id].alloc_cnt >= (ilm_struct_mod_cnt[module_id].free_cnt + 30)) {
                //KAL_PUBLIC_TRACE(TRACE_WARNING, "[ILM]%d,%d,%d\n",module_id, ilm_struct_mod_cnt[module_id].alloc_cnt, ilm_struct_mod_cnt[module_id].free_cnt);
            }
            return ((ilm_struct *)&ilm_array_ptr[i]);
        }
    }
    KAL_PUBLIC_TRACE(TRACE_ERROR, "FAIL: NO enough ilm_struct_array for module:%d\n", module_id);
    KAL_PUBLIC_TRACE(TRACE_ERROR, "[ILM]%d,%d,%d\n", module_id, ilm_struct_mod_cnt[module_id].alloc_cnt, ilm_struct_mod_cnt[module_id].free_cnt);
    return NULL;
}

/*************************************************************************
* FUNCTION
*  free_int_ilm
*
* DESCRIPTION
*     Frees the local parameters and peer buffer if no other module is
*  holding them, else the decrements the reference count.
*
* PARAMETERS
*  ilm_ptr     -  Pointer to the ilm_struct to be freed.
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void free_int_ilm(ilm_struct *ilm_ptr, kal_char *file_name, kal_uint32 line)
{
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[I-F]I:0x%X,L:0x%X,P:0x%X\n", ilm_ptr, ilm_ptr->local_para_ptr, ilm_ptr->peer_buff_ptr);
#if (TRACE_BUFF_DEBUG == 1)
    ILMTraceCheck(9);
    ILMTraceCancel(ilm_ptr);
#endif
    ASSERT(ilm_ptr);
    if (ilm_ptr->src_mod_id == MOD_TIMER) {
        return;
    }
    if (ilm_ptr->peer_buff_ptr != NULL) {
        if (ilm_ptr->peer_buff_ptr->ref_count == 0) {
            KAL_PUBLIC_TRACE(TRACE_ERROR, "[I-F-ALREADY]ERROR:0x%X\n", ilm_ptr->peer_buff_ptr);
        } else {

            --(ilm_ptr->peer_buff_ptr->ref_count);

            if (ilm_ptr->peer_buff_ptr->ref_count == 0) {
                FREE_BUFFER(ilm_ptr->peer_buff_ptr, (char *)__func__);
                ilm_ptr->peer_buff_ptr = NULL;
            }
        }
    }

    if (ilm_ptr->local_para_ptr != NULL) {
        if (ilm_ptr->local_para_ptr->ref_count == 0) {
            KAL_PUBLIC_TRACE(TRACE_ERROR, "[I-F-ALREADY]ERROR:0x%X\n", ilm_ptr->local_para_ptr);
        } else {

            --(ilm_ptr->local_para_ptr->ref_count);

            if (ilm_ptr->local_para_ptr->ref_count == 0) {
                FREE_BUFFER(ilm_ptr->local_para_ptr, (char *)__func__);
                ilm_ptr->local_para_ptr = NULL;
            }
        }
    }

    ASSERT((ilm_ptr->src_mod_id) == (((ILM_STRUCT_ARRAY *)ilm_ptr)->mod));
    ASSERT(((ILM_STRUCT_ARRAY *)ilm_ptr)->used == 1);
    if (ilm_ptr != NULL) {
        ilm_struct_mod_cnt[ilm_ptr->src_mod_id].free_cnt++;
        ((ILM_STRUCT_ARRAY *)ilm_ptr)->used = 0;
    }
    return;

} /* free_ilm() */

kal_uint32 get_ilm_avaliable_count(module_type module_id)
{
    kal_uint32 used_count = 0;
    kal_uint32 aval_count = 0;

    //KAL_PUBLIC_TRACE(TRACE_GROUP_10,"%d\n",((kal_uint32)1 - (kal_uint32)0xFFFFFFFE));//test overflow
    ASSERT(module_id < MOD_TOTAL_NUM);
    used_count = ilm_struct_mod_cnt[module_id].alloc_cnt - ilm_struct_mod_cnt[module_id].free_cnt;
    ASSERT(used_count < ilm_num_table[module_id]);
    aval_count = ilm_num_table[module_id] - used_count;
    return aval_count;
}

kal_uint32 get_ilm_used_count(module_type module_id)
{
    return (ilm_struct_mod_cnt[module_id].alloc_cnt - ilm_struct_mod_cnt[module_id].free_cnt);
}

/*************************************************************************
* FUNCTION
*  free_int_peer_buff
*
* DESCRIPTION
*     This fuction frees buffer used for peer to peer
*  communication.
*
* PARAMETERS
*  pdu_ptr       -> pointer to peer buffer
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void free_int_peer_buff(peer_buff_struct *pdu_ptr, kal_char *file, kal_uint32 line)
{

    if (pdu_ptr != NULL) {
        if (pdu_ptr->ref_count == 0) {
            KAL_PUBLIC_TRACE(TRACE_ERROR, "[I-F-P-ALREADY]ERROR:0x%X\n", pdu_ptr);
        }

#if 0

        --(pdu_ptr->ref_count);

        if (pdu_ptr->ref_count == 0) {
            KAL_PUBLIC_TRACE(TRACE_BUFF, "[I-F-P]pdu:0x%X\n", pdu_ptr);
            ILMTraceCancelPeerBuff(pdu_ptr);
            FREE_BUFFER(pdu_ptr, __func__);
        }
#else
        if (pdu_ptr->ref_count == 1) {
            KAL_PUBLIC_TRACE(TRACE_BUFF, "[I-F-P]0x%X\n", pdu_ptr);

#if (TRACE_BUFF_DEBUG == 1)
            ILMTraceCancelPeerBuff(pdu_ptr);
#endif
            //OSSchedLock();
            pdu_ptr->ref_count--;
            FREE_BUFFER(pdu_ptr, (char *)__func__);
            //OSSchedUnlock();
        } else {
            pdu_ptr->ref_count--;
        }
#endif
    } else {
        KAL_PUBLIC_TRACE(TRACE_WARNING, "[I-F-P-ALREADY]Warning:peer_buff is already NULL\n");
    }
    return;

}

/*************************************************************************
* FUNCTION
*  free_int_local_para
*
* DESCRIPTION
*     This function decrements local params ref count and
*  frees the local params if ref count becomes 0
*
* PARAMETERS
*  local_para_ptr    -  pointer to local parameter structure
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void free_int_local_para(local_para_struct *local_para_ptr, kal_char *file,
                         kal_uint32 line)
{

    if (local_para_ptr != NULL) {
        if (local_para_ptr->ref_count == 0) {
            KAL_PUBLIC_TRACE(TRACE_ERROR, "[I-F-L-ALREADY]ERROR:0x%X\n", local_para_ptr);
        }

        --(local_para_ptr->ref_count);

        if (local_para_ptr->ref_count == 0) {
            KAL_PUBLIC_TRACE(TRACE_BUFF, "[I-F-L]0x%X\n", local_para_ptr);
            FREE_BUFFER(local_para_ptr, (char *)__func__);
        }
    } else {
        KAL_PUBLIC_TRACE(TRACE_WARNING, "[I-F-L-ALREADY]Warning: local_para_ptr is already NULL\n");
    }

    return;

}

/*************************************************************************
* FUNCTION
*  hold_peer_buff
*
* DESCRIPTION
*     This fuction holds buffer used for peer to peer
*  communication for retransmissions.
*
* PARAMETERS
*  peer_buff_ptr       -> pointer to peer buffer
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_bool hold_peer_buff(peer_buff_struct *peer_buff_ptr)
{
    ASSERT(peer_buff_ptr);

    if (peer_buff_ptr != NULL) {

        peer_buff_ptr->ref_count++;

        return KAL_TRUE;
    }

    return KAL_FALSE;
}

/*************************************************************************
* FUNCTION
*  hold_local_para
*
* DESCRIPTION
*     This fuction increments refcount to enable holding of
*  local params
*
* PARAMETERS
*  local_para_ptr       -> pointer to local parameter structure
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_bool hold_local_para(local_para_struct *local_para_ptr)
{
    ASSERT(local_para_ptr);

    if (local_para_ptr != NULL) {

        local_para_ptr->ref_count++;

        return KAL_TRUE;
    }

    return KAL_FALSE;
}


/*************************************************************************
* FUNCTION
*  append_to_peer_buff
*
* DESCRIPTION
*   This fuction adds specified tail to end of peer data
*
* PARAMETERS
*  peer_buff_ptr   -> pointer to peer buffer
*  tail_data_ptr   -> data to be put in tail
*  tail_len        -> length of tail
*
* RETURNS
*  peer_buff_ptr
*
* GLOBALS AFFECTED
*
*************************************************************************/
void append_to_peer_buff(peer_buff_struct *peer_buff_ptr, void *tail_data_ptr,
                         kal_uint16 tail_len)
{

    void *temp_ptr;

    ASSERT(peer_buff_ptr);
    ASSERT(tail_data_ptr);
    if ((peer_buff_ptr == NULL) || (tail_len == 0) ||
            (peer_buff_ptr->free_tail_space < tail_len)) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "FAIL: %s,0x%X,%d,%d\n", __func__, peer_buff_ptr, tail_len, peer_buff_ptr->free_tail_space);
    } else {

        temp_ptr = (void *)((kal_uint8 *) peer_buff_ptr +
                            sizeof(*peer_buff_ptr) +
                            peer_buff_ptr->free_header_space +
                            peer_buff_ptr->pdu_len
                           );

        /* copying tail data in to the peer buffer */
        kal_mem_cpy(temp_ptr, tail_data_ptr, tail_len);

        /* reducing tail by indicated amount */
        peer_buff_ptr->free_tail_space -= tail_len;

        peer_buff_ptr->pdu_len += tail_len;
    }

    return;

}

/*************************************************************************
* FUNCTION
*  update_peer_buff_hdr
*
* DESCRIPTION
*
* PARAMETERS
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
void update_peer_buff_hdr(peer_buff_struct *peer_buff_ptr, kal_uint16 new_hdr_len,
                          kal_uint16 new_pdu_len, kal_uint16 new_tail_len)
{
    ASSERT(peer_buff_ptr);

    peer_buff_ptr->free_header_space = new_hdr_len;
    peer_buff_ptr->pdu_len = new_pdu_len;
    peer_buff_ptr->free_tail_space = new_tail_len;

    return;
}


/*************************************************************************
* FUNCTION
*  prepend_to_peer_buff
*
* DESCRIPTION
*  This fuction adds specified header to peer data
*
* PARAMETERS
*  peer_buff_ptr   -> pointer to peer buffer
*  header_data_ptr -> data to be put in header
*  header_len      -> length of header
*
* RETURNS
*  peer_buff_ptr
*
* GLOBALS AFFECTED
*
*************************************************************************/
void prepend_to_peer_buff(peer_buff_struct *peer_buff_ptr, void *header_data_ptr,
                          kal_uint16 header_len)
{
    void *temp_ptr;

    ASSERT(peer_buff_ptr);
    ASSERT(header_data_ptr);
    if ((peer_buff_ptr == NULL) || (header_len == 0) ||
            (peer_buff_ptr->free_header_space < header_len)) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "FAIL: %s,0x%X,%d,%d\n", __func__, peer_buff_ptr, header_len, peer_buff_ptr->free_tail_space);
    } else {

        temp_ptr = (void *)((kal_uint8 *) peer_buff_ptr +
                            sizeof(*peer_buff_ptr) +
                            (peer_buff_ptr->free_header_space -
                             header_len)
                           );

        /* copying header data in to the peer buffer */
        kal_mem_cpy(temp_ptr, header_data_ptr, header_len);

        /* header is being made part of PDU */
        peer_buff_ptr->free_header_space -= header_len;
        peer_buff_ptr->pdu_len += header_len;
    }

    return;

}

/*************************************************************************
* FUNCTION
*  get_pdu_ptr
*
* DESCRIPTION
*  This fuction adds specified header to peer data
*
* PARAMETERS
*  peer_buff_ptr  -> Pointer to the Peer-Buffer
*  length_ptr       -> Length of the PDU
*
* RETURNS
*  Pointer to PDU
*
* GLOBALS AFFECTED
*
*************************************************************************/
void *get_pdu_ptr(peer_buff_struct *peer_buff_ptr, kal_uint16 *length_ptr)
{

    ASSERT(peer_buff_ptr);
    ASSERT(length_ptr);

    *length_ptr = peer_buff_ptr->pdu_len;

    return (void *)((kal_uint8 *)peer_buff_ptr + sizeof(*peer_buff_ptr) +
                    peer_buff_ptr->free_header_space);
}

/*************************************************************************
* FUNCTION
*  get_local_para_ptr
*
* DESCRIPTION
*     This function extracts the local params from the local
*  parameters structure
*
* PARAMETERS
*  local_para_ptr      -> pointer to local parameter structure
*
* RETURNS
*  local_para_len_ptr  -> length of extracted local params
*
* GLOBALS AFFECTED
*
*************************************************************************/
void *get_local_para_ptr(local_para_struct *local_para_ptr, kal_uint16 *local_para_len_ptr)
{
    ASSERT(local_para_ptr);
    ASSERT(local_para_len_ptr);

    *local_para_len_ptr = local_para_ptr->msg_len;

    return (void *)(local_para_ptr);
}

/*************************************************************************
* FUNCTION
*  remove_hdr_of_peer_buff
*
* DESCRIPTION
*     This function removes the specified length header from pdu
*
* PARAMETERS
*  peer_buff_ptr      -> pointer to peer buffer
*  hdr_len            -> length of header to be removed
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void remove_hdr_of_peer_buff(peer_buff_struct *peer_buff_ptr, kal_uint16 hdr_len)
{
    if (peer_buff_ptr->pdu_len < hdr_len) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "FAIL: %s,pdu_len:0x%X,hdr_len:0x%X\n", __func__, peer_buff_ptr->pdu_len, hdr_len);
    }

    peer_buff_ptr->free_header_space += hdr_len;
    peer_buff_ptr->pdu_len -= hdr_len;
    return;
}

/*************************************************************************
* FUNCTION
*  remove_tail_of_peer_buff
*
* DESCRIPTION
*     This fuction removes specified length tail from pdu
*
* PARAMETERS
*  peer_buff_ptr      -> pointer to peer buffer
*  tail_len           -> length of tail to be removed
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void remove_tail_of_peer_buff(peer_buff_struct *peer_buff_ptr, kal_uint16 tail_len)
{
    if (peer_buff_ptr->pdu_len < tail_len) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "FAIL: %s, pdu_len:0x%X,tail_len:0x%X\n", __func__, peer_buff_ptr->pdu_len, tail_len);
    }

    peer_buff_ptr->free_tail_space += tail_len;
    peer_buff_ptr->pdu_len -= tail_len;
    return;
}

#if defined (WLAN_ZERO_COPY)
void *pbuf_to_flc2(struct pbuf *pbuf_ptr)
{
    peer_buff_struct *peer_buff_ptr;
    struct pbuf *pbuf_ptr_new = pbuf_ptr;

    ASSERT(pbuf_ptr);
    if ((pbuf_ptr->type != PBUF_RAM) && (pbuf_ptr->type != PBUF_POOL)) {
        KAL_PUBLIC_TRACE(TRACE_ERROR, "[F_F_P]FAIL:type%d", pbuf_ptr->type);
        return NULL;
    }
    if (pbuf_ptr->ref > 2) { //TCP TX Retransimission
        ASSERT(pbuf_ptr->len < 2048);
        pbuf_ptr_new = pbuf_alloc(PBUF_RAW, 2048, PBUF_RAM);
        ASSERT(pbuf_ptr_new);
        memcpy(pbuf_ptr_new->payload, pbuf_ptr->payload, pbuf_ptr->len);
        pbuf_ptr_new->len = pbuf_ptr->len;
        pbuf_free(pbuf_ptr);
    }

    peer_buff_ptr = (peer_buff_struct *)((kal_uint32)pbuf_ptr_new + OFFSET_OF(struct pbuf, aheadroom[0]));
    peer_buff_ptr->pb_resvered = 0;
    peer_buff_ptr->ref_count = 1;
    peer_buff_ptr->free_header_space = ((kal_uint32)pbuf_ptr_new->payload - (kal_uint32) & (pbuf_ptr_new->aheadroom[0])) - sizeof(peer_buff_struct);
    peer_buff_ptr->pdu_len = pbuf_ptr_new->len;
    peer_buff_ptr->free_tail_space = 0;
    //ASSERT((((kal_uint32)pbuf_ptr->payload) & 0x03) == 0);
    //ASSERT((((kal_uint32)pbuf_ptr_new) & 0x03) == 0);
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[F_F_P]pb:0x%X,pe:0x%X,pay:0x%X,freehead:0x%X,%d,%d,0x%X,%d\n", pbuf_ptr_new, peer_buff_ptr, pbuf_ptr_new->payload, peer_buff_ptr->free_header_space, pbuf_ptr_new->len, pbuf_ptr_new->tot_len, pbuf_ptr_new->next, pbuf_ptr_new->ref);
    return (void *)peer_buff_ptr;
}

void *flc2_to_pbuf(peer_buff_struct *peer_buff_ptr)
{
    struct pbuf *pbuf_ptr;
    unsigned char *data_p = NULL;
    kal_uint16 pdu_len = 0;

    data_p = (kal_uint8 *)(get_pdu_ptr(peer_buff_ptr, &pdu_len));
    pbuf_ptr = (struct pbuf *)((kal_uint32)peer_buff_ptr - OFFSET_OF(struct pbuf, aheadroom));
    pbuf_ptr->payload = data_p;
    pbuf_ptr->len = peer_buff_ptr->pdu_len;
    pbuf_ptr->tot_len = peer_buff_ptr->pdu_len;
    //ASSERT((((kal_uint32)pbuf_ptr->payload) & 0x03) == 0);
    KAL_PUBLIC_TRACE(TRACE_BUFF, "[F_T_P]pb:0x%X,pe:0x%X,%d\n", pbuf_ptr, peer_buff_ptr, pbuf_ptr->len);
    return (void *)pbuf_ptr;
}



void *flc2_ent_get_peer_buff(flc2_pool_id_enum pool_id, kal_uint16 pdu_len,
                             kal_uint16 header_size, kal_uint16 tail_size)
{
    peer_buff_struct *peer_buff_ptr;
    struct pbuf *pbuf_ptr;
    kal_uint16 alloc_len = 2048;

    if (pdu_len > 2048) {
        alloc_len = pdu_len;
    }

    EXT_ASSERT((pdu_len + header_size + tail_size) != 0, pdu_len, header_size, tail_size);
    EXT_ASSERT((sizeof(*peer_buff_ptr) + header_size) <= WLAN_HEADROOM_SIZE, sizeof(*peer_buff_ptr), header_size, WLAN_HEADROOM_SIZE);

    pbuf_ptr = pbuf_alloc(PBUF_RAW, alloc_len + tail_size, PBUF_RAM);
    EXT_ASSERT(pbuf_ptr, pdu_len + tail_size, 0, 0);
    ASSERT((((kal_uint32)pbuf_ptr->payload) & 0x03) == 0);
    if (pbuf_ptr == NULL) {
        return NULL;
    }
    peer_buff_ptr = (peer_buff_struct *)((kal_uint32)pbuf_ptr + OFFSET_OF(struct pbuf, aheadroom[0]));
    peer_buff_ptr->pb_resvered = 0;
    peer_buff_ptr->ref_count = 1;
    peer_buff_ptr->free_header_space = WLAN_HEADROOM_SIZE - sizeof(peer_buff_struct);
    peer_buff_ptr->pdu_len = pdu_len;
    peer_buff_ptr->free_tail_space = tail_size;

    KAL_PUBLIC_TRACE(TRACE_BUFF, "[F_M]pb:0x%X,pe:0x%X,pay:0x%X\n", pbuf_ptr, peer_buff_ptr, pbuf_ptr->payload);
    return (void *)peer_buff_ptr;

}

void flc2_ent_free_peer_buff(flc2_pool_id_enum pool_id, peer_buff_struct *peer_buff_ptr)
{
    struct pbuf *pbuf_ptr;

    ASSERT(peer_buff_ptr);
    ASSERT(peer_buff_ptr->ref_count != 0);

#if 0
    --(peer_buff_ptr->ref_count);

    if (peer_buff_ptr->ref_count == 0) {
        pbuf_ptr = (struct pbuf *)((kal_uint32)peer_buff_ptr - OFFSET_OF(struct pbuf, aheadroom));
        KAL_PUBLIC_TRACE(TRACE_BUFF, "[F_F]pbuf:0x%X,peer:0x%X\n", pbuf_ptr, peer_buff_ptr);
#if (TRACE_BUFF_DEBUG == 1)
        ILMTraceCancelPeerBuff(peer_buff_ptr);
#endif
        pbuf_free(pbuf_ptr);
    }
#else
    if (peer_buff_ptr->ref_count == 1) {
        pbuf_ptr = (struct pbuf *)((kal_uint32)peer_buff_ptr - OFFSET_OF(struct pbuf, aheadroom));
        KAL_PUBLIC_TRACE(TRACE_BUFF, "[F_F]pb:0x%X,pe:0x%X\n", pbuf_ptr, peer_buff_ptr);
#if (TRACE_BUFF_DEBUG == 1)
        ILMTraceCancelPeerBuff(peer_buff_ptr);
#endif
        //OSSchedLock();
        peer_buff_ptr->ref_count--;
        pbuf_free(pbuf_ptr);
        //OSSchedUnlock();
    } else {
        peer_buff_ptr->ref_count--;
    }
#endif
}
#endif

void hexdump(char *header, unsigned char *src, unsigned int len)
{
    int i = 0;
    KAL_PUBLIC_TRACE(TRACE_INFO, "%s %p[len=%d]:\n", header, src, len);
    for (i = 0; i < len; i++) {
        if (i % 16 == 15) {
            KAL_PUBLIC_TRACE(TRACE_INFO, "0x%02x\n", src[i]);
        } else {
            KAL_PUBLIC_TRACE(TRACE_INFO, "0x%02x ", src[i]);
        }
    }
    if (i % 16 != 0) {
        KAL_PUBLIC_TRACE(TRACE_INFO, "\n");
    }
}

void kal_get_time(kal_uint32 *ptime)
{
#if 0//Need implement
    //return ticks of clock, it's a 32 bit counter.
    *ptime = OSTimeGet();
#endif
    //os_get_time();
    *ptime = xTaskGetTickCount();
}

void kal_task_enter_critical()
{
#if ( portCRITICAL_NESTING_IN_TCB == 1 )
    vTaskEnterCritical();
#endif
}

void kal_task_exit_critical()
{
#if ( portCRITICAL_NESTING_IN_TCB == 1 )
    vTaskExitCritical();
#endif
}

os_queue_t kal_queue_create( uint32_t queue_len, uint32_t item_size)
{
    return (os_queue_t) xQueueCreate(queue_len, item_size);
}

uint32_t kal_queue_message_waiting( const os_queue_t queue )
{
    return (uint32_t)uxQueueMessagesWaiting(queue);
}
    
uint32_t kal_queue_get_space( const os_queue_t queue )
{
    return (uint32_t)uxQueueSpacesAvailable(queue);
}

int32_t kal_queue_receive(os_queue_t queue, void *buf, uint32_t wait_time)
{

    TickType_t ticks;

    if(wait_time == 0){
        ticks = portMAX_DELAY;
    }else{
        ticks = wait_time / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueReceive(queue, buf, ticks))
        return 0;

    return -1;
}

int32_t kal_queue_send(os_queue_t queue, void *item, uint32_t wait_time)
{
    TickType_t ticks;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = wait_time / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueSend(queue, item, ticks))
        return 0;

    return -1;
}


int32_t kal_queue_send_from_isr(os_queue_t queue, void *item)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(pdPASS == xQueueSendFromISR(queue, item, &xHigherPriorityTaskWoken))
        return 0;

    return -1;
}

int32_t kal_queue_send_front(os_queue_t queue, void *item, uint32_t wait_time)
{
    TickType_t ticks;

    if(wait_time == 0){
        ticks = portMAX_DELAY;
    }else{
        ticks = wait_time / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xQueueSendToFront(queue, item, ticks))
        return 0;

    return -1;
}

int32_t kal_queue_send_front_from_isr(os_queue_t queue, void *item)
{

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(pdPASS == xQueueSendToFrontFromISR(queue, item, &xHigherPriorityTaskWoken))
        return 0;

    return -1;
}


