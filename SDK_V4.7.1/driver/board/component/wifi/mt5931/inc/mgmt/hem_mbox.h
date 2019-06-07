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
** $Id: //Department/DaVinci/TRUNK/MT6620_5931_WiFi_Driver/include/mgmt/hem_mbox.h#15 $
*/

/*! \file   hem_mbox.h
    \brief

*/




#ifndef _HEM_MBOX_H
#define _HEM_MBOX_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/* Message IDs */
typedef enum _ENUM_MSG_ID_T {
    MID_MNY_CNM_CH_REQ,                 /* MANY notify CNM to obtain channel privilege */
    MID_MNY_CNM_CH_ABORT,               /* MANY notify CNM to abort/release channel privilege */

    MID_CNM_AIS_CH_GRANT,               /* CNM notify AIS for indicating channel granted */
    MID_CNM_P2P_CH_GRANT,               /* CNM notify P2P for indicating channel granted */
    MID_CNM_BOW_CH_GRANT,               /* CNM notify BOW for indicating channel granted */

    /*--------------------------------------------------*/
    /* SCN Module Mailbox Messages                      */
    /*--------------------------------------------------*/
    MID_AIS_SCN_SCAN_REQ,               /* AIS notify SCN for starting scan */
    MID_AIS_SCN_SCAN_CANCEL,            /* AIS notify SCN for cancelling scan */
    MID_P2P_SCN_SCAN_REQ,               /* P2P notify SCN for starting scan */
    MID_P2P_SCN_SCAN_CANCEL,            /* P2P notify SCN for cancelling scan */
    MID_BOW_SCN_SCAN_REQ,               /* BOW notify SCN for starting scan */
    MID_BOW_SCN_SCAN_CANCEL,            /* BOW notify SCN for cancelling scan */
    MID_RLM_SCN_SCAN_REQ,               /* RLM notify SCN for starting scan (OBSS-SCAN) */
    MID_RLM_SCN_SCAN_CANCEL,            /* RLM notify SCN for cancelling scan (OBSS-SCAN)*/
    MID_SCN_AIS_SCAN_DONE,              /* SCN notify AIS for scan completion */
    MID_SCN_P2P_SCAN_DONE,              /* SCN notify P2P for scan completion */
    MID_SCN_BOW_SCAN_DONE,              /* SCN notify BOW for scan completion */
    MID_SCN_RLM_SCAN_DONE,              /* SCN notify RLM for scan completion (OBSS-SCAN) */

    /*--------------------------------------------------*/
    /* AIS Module Mailbox Messages                      */
    /*--------------------------------------------------*/
    MID_OID_AIS_FSM_JOIN_REQ,           /* OID/IOCTL notify AIS for join */
    MID_OID_AIS_FSM_ABORT,              /* OID/IOCTL notify AIS for abort */
    MID_AIS_SAA_FSM_START,              /* AIS notify SAA for Starting authentication/association fsm */
    MID_AIS_SAA_FSM_ABORT,              /* AIS notify SAA for Aborting authentication/association fsm */
    MID_SAA_AIS_JOIN_COMPLETE,          /* SAA notify AIS for indicating join complete */

#if CFG_ENABLE_BT_OVER_WIFI
    /*--------------------------------------------------*/
    /* BOW Module Mailbox Messages                      */
    /*--------------------------------------------------*/
    MID_BOW_SAA_FSM_START,              /* BOW notify SAA for Starting authentication/association fsm */
    MID_BOW_SAA_FSM_ABORT,              /* BOW notify SAA for Aborting authentication/association fsm */
    MID_SAA_BOW_JOIN_COMPLETE,          /* SAA notify BOW for indicating join complete */
#endif

#if CFG_ENABLE_WIFI_DIRECT
    /*--------------------------------------------------*/
    /* P2P Module Mailbox Messages                      */
    /*--------------------------------------------------*/
    MID_P2P_SAA_FSM_START,              /* P2P notify SAA for Starting authentication/association fsm */
    MID_P2P_SAA_FSM_ABORT,              /* P2P notify SAA for Aborting authentication/association fsm */
    MID_SAA_P2P_JOIN_COMPLETE,          /* SAA notify P2P for indicating join complete */

    MID_MNY_P2P_FUN_SWITCH,             /* Enable P2P FSM. */
    MID_MNY_P2P_DEVICE_DISCOVERY,       /* Start device discovery. */
    MID_MNY_P2P_DISCOVERY_ABORT,        /* Start device discovery abort. */
    MID_MNY_P2P_CONNECTION_REQ,         /* Connection request. */
    MID_MNY_P2P_CONNECTION_ABORT,       /* Abort connection request, P2P FSM return to IDLE. */
    MID_MNY_P2P_CONNECTION_TRIGGER,     /* Only if connection request is valid. Trigger P2P FSM to connection target AP. */
    MID_MNY_P2P_CONNECTION_PAUSE,       /* Disconnect from target AP, connection is not complete yet. */
    MID_MNY_P2P_PROVISION_COMPLETE,     /* Provisioning complete. */
    MID_MNY_P2P_INVITATION_REQ,
    MID_MNY_P2P_INVITATION_ABORT,
#endif

#if CFG_SUPPORT_ADHOC
    MID_SCN_AIS_FOUND_IBSS,             /* SCN notify AIS that an IBSS Peer has been found and can merge into */
#endif /* CFG_SUPPORT_ADHOC */

    MID_SAA_AIS_FSM_ABORT,              /* SAA notify AIS for indicating deauthentication/disassociation */

    MID_TOTAL_NUM
} ENUM_MSG_ID_T, *P_ENUM_MSG_ID_T;

/* Message header of inter-components */
struct _MSG_HDR_T {
    LINK_ENTRY_T    rLinkEntry;
    ENUM_MSG_ID_T   eMsgId;
};

typedef VOID (*PFN_MSG_HNDL_FUNC)(P_ADAPTER_T, P_MSG_HDR_T);

typedef struct _MSG_HNDL_ENTRY {
    ENUM_MSG_ID_T       eMsgId;
    PFN_MSG_HNDL_FUNC   pfMsgHndl;
} MSG_HNDL_ENTRY_T, *P_MSG_HNDL_ENTRY_T;

typedef enum _EUNM_MSG_SEND_METHOD_T {
    MSG_SEND_METHOD_BUF = 0,    /* Message is put in the queue and will be
                                   executed when mailbox is checked. */
    MSG_SEND_METHOD_UNBUF       /* The handler function is called immediately
                                   in the same context of the sender */
} EUNM_MSG_SEND_METHOD_T, *P_EUNM_MSG_SEND_METHOD_T;


typedef enum _ENUM_MBOX_ID_T {
    MBOX_ID_0 = 0,
    MBOX_ID_TOTAL_NUM
} ENUM_MBOX_ID_T, *P_ENUM_MBOX_ID_T;

/* Define Mailbox structure */
typedef struct _MBOX_T {
    LINK_T          rLinkHead;
} MBOX_T, *P_MBOX_T;

typedef struct _MSG_SAA_FSM_START_T {
    MSG_HDR_T       rMsgHdr;    /* Must be the first member */
    UINT_8          ucSeqNum;
    P_STA_RECORD_T  prStaRec;
} MSG_SAA_FSM_START_T, *P_MSG_SAA_FSM_START_T;

typedef struct _MSG_SAA_FSM_COMP_T {
    MSG_HDR_T       rMsgHdr;        /* Must be the first member */
    UINT_8          ucSeqNum;
    WLAN_STATUS     rJoinStatus;
    P_STA_RECORD_T  prStaRec;
    P_SW_RFB_T      prSwRfb;
} MSG_SAA_FSM_COMP_T, *P_MSG_SAA_FSM_COMP_T;

typedef struct _MSG_SAA_FSM_ABORT_T {
    MSG_HDR_T       rMsgHdr;    /* Must be the first member */
    UINT_8          ucSeqNum;
    P_STA_RECORD_T  prStaRec;
} MSG_SAA_FSM_ABORT_T, *P_MSG_SAA_FSM_ABORT_T;

typedef struct _MSG_CONNECTION_ABORT_T {
    MSG_HDR_T       rMsgHdr;    /* Must be the first member */
    UINT_8          ucNetTypeIndex;
} MSG_CONNECTION_ABORT_T, *P_MSG_CONNECTION_ABORT_T;



/* specific message data types */
typedef MSG_SAA_FSM_START_T MSG_JOIN_REQ_T, *P_MSG_JOIN_REQ_T;
typedef MSG_SAA_FSM_COMP_T MSG_JOIN_COMP_T, *P_MSG_JOIN_COMP_T;
typedef MSG_SAA_FSM_ABORT_T MSG_JOIN_ABORT_T, *P_MSG_JOIN_ABORT_T;


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
VOID
mboxSetup(
    IN P_ADAPTER_T  prAdapter,
    IN ENUM_MBOX_ID_T  eMboxId
);


VOID
mboxSendMsg(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_MBOX_ID_T eMboxId,
    IN P_MSG_HDR_T prMsg,
    IN EUNM_MSG_SEND_METHOD_T eMethod
);

VOID
mboxRcvAllMsg(
    IN P_ADAPTER_T prAdapter,
    IN ENUM_MBOX_ID_T eMboxId
);

VOID
mboxInitialize(
    IN P_ADAPTER_T prAdapter
);

VOID
mboxDestroy(
    IN P_ADAPTER_T prAdapter
);

VOID
mboxDummy(
    IN P_ADAPTER_T prAdapter,
    P_MSG_HDR_T prMsgHdr
);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _HEM_MBOX_H */


