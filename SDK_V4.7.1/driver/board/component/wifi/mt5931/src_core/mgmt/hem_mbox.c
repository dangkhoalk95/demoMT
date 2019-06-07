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

/*
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/mgmt/hem_mbox.c#36 $
*/

/*! \file   "hem_mbox.c"
    \brief

*/




/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
#if DBG
/*lint -save -e64 Type mismatch */
static PUINT_8 apucDebugMsg[] = {
    (PUINT_8)DISP_STRING("MID_MNY_CNM_CH_REQ"),
    (PUINT_8)DISP_STRING("MID_MNY_CNM_CH_ABORT"),
    (PUINT_8)DISP_STRING("MID_CNM_AIS_CH_GRANT"),
    (PUINT_8)DISP_STRING("MID_CNM_P2P_CH_GRANT"),
    (PUINT_8)DISP_STRING("MID_CNM_BOW_CH_GRANT"),

    (PUINT_8)DISP_STRING("MID_AIS_SCN_SCAN_REQ"),
    (PUINT_8)DISP_STRING("MID_AIS_SCN_SCAN_CANCEL"),
    (PUINT_8)DISP_STRING("MID_P2P_SCN_SCAN_REQ"),
    (PUINT_8)DISP_STRING("MID_P2P_SCN_SCAN_CANCEL"),
    (PUINT_8)DISP_STRING("MID_BOW_SCN_SCAN_REQ"),
    (PUINT_8)DISP_STRING("MID_BOW_SCN_SCAN_CANCEL"),
    (PUINT_8)DISP_STRING("MID_RLM_SCN_SCAN_REQ"),
    (PUINT_8)DISP_STRING("MID_RLM_SCN_SCAN_CANCEL"),
    (PUINT_8)DISP_STRING("MID_SCN_AIS_SCAN_DONE"),
    (PUINT_8)DISP_STRING("MID_SCN_P2P_SCAN_DONE"),
    (PUINT_8)DISP_STRING("MID_SCN_BOW_SCAN_DONE"),
    (PUINT_8)DISP_STRING("MID_SCN_RLM_SCAN_DONE"),

    (PUINT_8)DISP_STRING("MID_OID_AIS_FSM_JOIN_REQ"),
    (PUINT_8)DISP_STRING("MID_OID_AIS_FSM_ABORT"),
    (PUINT_8)DISP_STRING("MID_AIS_SAA_FSM_START"),
    (PUINT_8)DISP_STRING("MID_AIS_SAA_FSM_ABORT"),
    (PUINT_8)DISP_STRING("MID_SAA_AIS_JOIN_COMPLETE"),

#if CFG_ENABLE_BT_OVER_WIFI
    (PUINT_8)DISP_STRING("MID_BOW_SAA_FSM_START"),
    (PUINT_8)DISP_STRING("MID_BOW_SAA_FSM_ABORT"),
    (PUINT_8)DISP_STRING("MID_SAA_BOW_JOIN_COMPLETE"),
#endif

#if CFG_ENABLE_WIFI_DIRECT
    (PUINT_8)DISP_STRING("MID_P2P_SAA_FSM_START"),
    (PUINT_8)DISP_STRING("MID_P2P_SAA_FSM_ABORT"),
    (PUINT_8)DISP_STRING("MID_SAA_P2P_JOIN_COMPLETE"),

    (PUINT_8)DISP_STRING("MID_MNY_P2P_FUN_SWITCH"),
    (PUINT_8)DISP_STRING("MID_MNY_P2P_DEVICE_DISCOVERY"),
    (PUINT_8)DISP_STRING("MID_MNY_P2P_DISCOVERY_ABORT"),
    (PUINT_8)DISP_STRING("MID_MNY_P2P_CONNECTION_REQ"),
    (PUINT_8)DISP_STRING("MID_MNY_P2P_CONNECTION_ABORT"),
    (PUINT_8)DISP_STRING("MID_MNY_P2P_CONNECTION_TRIGGER"),
    (PUINT_8)DISP_STRING("MID_MNY_P2P_CONNECTION_PAUSE"),
    (PUINT_8)DISP_STRING("MID_MNY_P2P_PROVISION_COMPLETE"),
#endif

#if CFG_SUPPORT_ADHOC
    //(PUINT_8)DISP_STRING("MID_AIS_CNM_CREATE_IBSS_REQ"),
    //(PUINT_8)DISP_STRING("MID_CNM_AIS_CREATE_IBSS_GRANT"),
    //(PUINT_8)DISP_STRING("MID_AIS_CNM_MERGE_IBSS_REQ"),
    //(PUINT_8)DISP_STRING("MID_CNM_AIS_MERGE_IBSS_GRANT"),
    (PUINT_8)DISP_STRING("MID_SCN_AIS_FOUND_IBSS"),
#endif /* CFG_SUPPORT_ADHOC */

    (PUINT_8)DISP_STRING("MID_SAA_AIS_FSM_ABORT")
};
/*lint -restore */
#endif /* DBG */

/* This message entry will be re-ordered based on the message ID order
 * by invoking mboxInitMsgMap()
 */
static MSG_HNDL_ENTRY_T arMsgMapTable[] = {
    { MID_MNY_CNM_CH_REQ,           cnmChMngrRequestPrivilege               },
    { MID_MNY_CNM_CH_ABORT,         cnmChMngrAbortPrivilege                 },
    { MID_CNM_AIS_CH_GRANT,         aisFsmRunEventChGrant                   },
#if CFG_ENABLE_WIFI_TETHERING
#if 1
    { MID_CNM_P2P_CH_GRANT,         wtRunEventChGrant                       }, /*set in gl_p2p_init.c*/
#else
    { MID_CNM_P2P_CH_GRANT,         p2pFsmRunEventChGrant                   }, /*set in gl_p2p_init.c*/
#endif
#else
    { MID_CNM_P2P_CH_GRANT,         mboxDummy                               }, /*set in gl_p2p_init.c*/
#endif
#if CFG_ENABLE_BT_OVER_WIFI
    { MID_CNM_BOW_CH_GRANT,         bowRunEventChGrant                             },
#else
    { MID_CNM_BOW_CH_GRANT,         mboxDummy                               },
#endif

    /*--------------------------------------------------*/
    /* SCN Module Mailbox Messages                      */
    /*--------------------------------------------------*/
    { MID_AIS_SCN_SCAN_REQ,         scnFsmMsgStart                          },
    { MID_AIS_SCN_SCAN_CANCEL,      scnFsmMsgAbort                          },
    { MID_P2P_SCN_SCAN_REQ,         scnFsmMsgStart                          },
    { MID_P2P_SCN_SCAN_CANCEL,      scnFsmMsgAbort                          },
    { MID_BOW_SCN_SCAN_REQ,         scnFsmMsgStart                          },
    { MID_BOW_SCN_SCAN_CANCEL,      scnFsmMsgAbort                          },
    { MID_RLM_SCN_SCAN_REQ,         scnFsmMsgStart                          },
    { MID_RLM_SCN_SCAN_CANCEL,      scnFsmMsgAbort                          },
    { MID_SCN_AIS_SCAN_DONE,        aisFsmRunEventScanDone                  },
    { MID_SCN_P2P_SCAN_DONE,        mboxDummy                               }, /*set in gl_p2p_init.c*/

#if CFG_ENABLE_BT_OVER_WIFI
    { MID_SCN_BOW_SCAN_DONE,        bowResponderScanDone                               },
#else
    { MID_SCN_BOW_SCAN_DONE,        mboxDummy                               },
#endif
    { MID_SCN_RLM_SCAN_DONE,        rlmObssScanDone                         },

    /*--------------------------------------------------*/
    /* AIS Module Mailbox Messages                      */
    /*--------------------------------------------------*/
    { MID_OID_AIS_FSM_JOIN_REQ,     aisFsmRunEventAbort                     },
    { MID_OID_AIS_FSM_ABORT,        aisFsmRunEventAbort                     },
    { MID_AIS_SAA_FSM_START,        saaFsmRunEventStart                     },
    { MID_AIS_SAA_FSM_ABORT,        saaFsmRunEventAbort                     },
    { MID_SAA_AIS_JOIN_COMPLETE,    aisFsmRunEventJoinComplete              },

#if CFG_ENABLE_BT_OVER_WIFI
    /*--------------------------------------------------*/
    /* BOW Module Mailbox Messages                      */
    /*--------------------------------------------------*/
    { MID_BOW_SAA_FSM_START,        saaFsmRunEventStart                     },
    { MID_BOW_SAA_FSM_ABORT,        saaFsmRunEventAbort                     },
    { MID_SAA_BOW_JOIN_COMPLETE,    bowFsmRunEventJoinComplete              },
#endif

#if CFG_ENABLE_WIFI_DIRECT  /*set in gl_p2p_init.c*/
    { MID_P2P_SAA_FSM_START,        mboxDummy                               },
    { MID_P2P_SAA_FSM_ABORT,        mboxDummy                               },
    { MID_SAA_P2P_JOIN_COMPLETE,    mboxDummy                               },// TODO: p2pFsmRunEventJoinComplete

    { MID_MNY_P2P_FUN_SWITCH,       mboxDummy                               },
    { MID_MNY_P2P_DEVICE_DISCOVERY, mboxDummy                               },
    { MID_MNY_P2P_DISCOVERY_ABORT,  mboxDummy                               },
    { MID_MNY_P2P_CONNECTION_REQ,   mboxDummy                               },
    { MID_MNY_P2P_CONNECTION_ABORT, mboxDummy                               },
    { MID_MNY_P2P_CONNECTION_TRIGGER, mboxDummy                              },
    { MID_MNY_P2P_CONNECTION_PAUSE, mboxDummy                               },
    { MID_MNY_P2P_PROVISION_COMPLETE, mboxDummy                              },
    { MID_MNY_P2P_INVITATION_REQ, mboxDummy                              },
    { MID_MNY_P2P_INVITATION_ABORT, mboxDummy                              },
#endif

#if CFG_SUPPORT_ADHOC
    { MID_SCN_AIS_FOUND_IBSS,       aisFsmRunEventFoundIBSSPeer             },
#endif /* CFG_SUPPORT_ADHOC */

    { MID_SAA_AIS_FSM_ABORT,        aisFsmRunEventAbort                     }
};

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
#define MBOX_HNDL_MSG(prAdapter, prMsg) do { \
        DBGLOG(CNM, LOUD, ("DO MSG [%d: %s]\n", prMsg->eMsgId, apucDebugMsg[prMsg->eMsgId])); \
        arMsgMapTable[prMsg->eMsgId].pfMsgHndl(prAdapter, prMsg); \
} while (0)

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/


/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mboxInitMsgMap(
    VOID
)
{
    UINT_32             i, idx;
    MSG_HNDL_ENTRY_T    rTempEntry;

    ASSERT((sizeof(arMsgMapTable) / sizeof(MSG_HNDL_ENTRY_T)) == MID_TOTAL_NUM);

    for (i = 0; i < MID_TOTAL_NUM; i++) {
        if (arMsgMapTable[i].eMsgId == (ENUM_MSG_ID_T) i) {
            continue;
        }
        for (idx = i + 1; idx < MID_TOTAL_NUM; idx++) {
            if (arMsgMapTable[idx].eMsgId == (ENUM_MSG_ID_T) i) {
                break;
            }
        }
        ASSERT(idx < MID_TOTAL_NUM);
        if (idx >= MID_TOTAL_NUM) {
            continue;
        }

        /* Swap target entry and current entry */
        rTempEntry.eMsgId = arMsgMapTable[idx].eMsgId;
        rTempEntry.pfMsgHndl = arMsgMapTable[idx].pfMsgHndl;

        arMsgMapTable[idx].eMsgId = arMsgMapTable[i].eMsgId;
        arMsgMapTable[idx].pfMsgHndl = arMsgMapTable[i].pfMsgHndl;

        arMsgMapTable[i].eMsgId = rTempEntry.eMsgId;
        arMsgMapTable[i].pfMsgHndl = rTempEntry.pfMsgHndl;
    }

    /* Verify the correctness of final message map */
    for (i = 0; i < MID_TOTAL_NUM; i++) {
        ASSERT(arMsgMapTable[i].eMsgId == (ENUM_MSG_ID_T) i);
        while (arMsgMapTable[i].eMsgId != (ENUM_MSG_ID_T) i);
    }

}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mboxSetup(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_MBOX_ID_T eMboxId
)
{
    P_MBOX_T prMbox;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(eMboxId < MBOX_ID_TOTAL_NUM);
    ASSERT(prAdapter);

    prMbox = &(prAdapter->arMbox[eMboxId]);

    KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_MAILBOX);
    LINK_INITIALIZE(&prMbox->rLinkHead);
    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_MAILBOX);
}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mboxSendMsg(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_MBOX_ID_T eMboxId,
    IN P_MSG_HDR_T prMsg,
    IN EUNM_MSG_SEND_METHOD_T eMethod
)
{
    P_MBOX_T    prMbox;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(eMboxId < MBOX_ID_TOTAL_NUM);
    ASSERT(prMsg);
    ASSERT(prAdapter);

    prMbox = &(prAdapter->arMbox[eMboxId]);

    switch (eMethod) {
        case MSG_SEND_METHOD_BUF:
            KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_MAILBOX);
            LINK_INSERT_TAIL(&prMbox->rLinkHead, &prMsg->rLinkEntry);
            KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_MAILBOX);

            // to wake up main service thread
            GLUE_SET_EVENT(prAdapter->prGlueInfo);

            break;

        case MSG_SEND_METHOD_UNBUF:
            MBOX_HNDL_MSG(prAdapter, prMsg);
            break;

        default:
            ASSERT(0);
            break;
    }
}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mboxRcvAllMsg(
    IN P_ADAPTER_T prAdapter,
    ENUM_MBOX_ID_T eMboxId
)
{
    P_MBOX_T        prMbox;
    P_MSG_HDR_T     prMsg;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(eMboxId < MBOX_ID_TOTAL_NUM);
    ASSERT(prAdapter);

    prMbox = &(prAdapter->arMbox[eMboxId]);

    while (!LINK_IS_EMPTY(&prMbox->rLinkHead)) {
        KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_MAILBOX);
        LINK_REMOVE_HEAD(&prMbox->rLinkHead, prMsg, P_MSG_HDR_T);
        KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_MAILBOX);

        ASSERT(prMsg);
        MBOX_HNDL_MSG(prAdapter, prMsg);
    }

}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mboxInitialize(
    IN P_ADAPTER_T prAdapter
)
{
    UINT_32     i;

    ASSERT(prAdapter);

    /* Initialize Mailbox */
    mboxInitMsgMap();

    /* Setup/initialize each mailbox */
    for (i = 0; i < MBOX_ID_TOTAL_NUM; i++) {
        mboxSetup(prAdapter, (ENUM_MBOX_ID_T)i);
    }

}

/*----------------------------------------------------------------------------*/
/*!
* \brief
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mboxDestroy(
    IN P_ADAPTER_T prAdapter
)
{
    P_MBOX_T        prMbox;
    P_MSG_HDR_T     prMsg;
    UINT_8          i;
    KAL_SPIN_LOCK_DECLARATION();

    ASSERT(prAdapter);

    for (i = 0; i < MBOX_ID_TOTAL_NUM; i++) {
        prMbox = &(prAdapter->arMbox[i]);

        while (!LINK_IS_EMPTY(&prMbox->rLinkHead)) {
            KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_MAILBOX);
            LINK_REMOVE_HEAD(&prMbox->rLinkHead, prMsg, P_MSG_HDR_T);
            KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_MAILBOX);

            ASSERT(prMsg);
            cnmMemFree(prAdapter, prMsg);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*!
* \brief This is dummy function to prevent empty arMsgMapTable[] for compiling.
*
* \param[in]
*
* \return none
*/
/*----------------------------------------------------------------------------*/
VOID
mboxDummy(
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
)
{
    ASSERT(prAdapter);

    cnmMemFree(prAdapter, prMsgHdr);

    return;
}


