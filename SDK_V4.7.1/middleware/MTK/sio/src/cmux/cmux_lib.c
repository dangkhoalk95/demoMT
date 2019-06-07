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

#include "cmux_lib.h"

#define LOGE(fmt,arg...)        LOG_E(cmux, "[cmux_lib.c] "fmt,##arg)
#define LOGW(fmt,arg...)        LOG_W(cmux, "[cmux_lib.c] "fmt,##arg)
#define LOGI(fmt,arg...)        LOG_I(cmux, "[cmux_lib.c] "fmt,##arg)

void cmux_initBuffer ( CmuxRingBufferT *pBuff, kal_uint8 *pDataBuff,
                       kal_uint32 size )
{
    pBuff->pDataBuffer = pDataBuff;
    pBuff->pWrite = pBuff->pDataBuffer;
    pBuff->pRead = pBuff->pDataBuffer;
    pBuff->u4BufferSize = size;
}

void cmux_clearBuffer ( CmuxRingBufferT *pBuff )
{
    pBuff->pRead = pBuff->pWrite = pBuff->pDataBuffer;
}

void cmux_readBuffer ( CmuxRingBufferT *pBuff, kal_uint8 *pDst,
                       kal_uint32 length )
{
    kal_uint8 *pRead = pBuff->pRead;
    kal_uint8 *pWrite = pBuff->pWrite;

    if ( length > cmux_getBufferDataAvail ( pBuff ) ) {
        configASSERT ( 0 );
    }

    if ( pWrite < pRead ) {
        /* case 2 */
        kal_uint32 rearData = pBuff->u4BufferSize - (pRead - pBuff->pDataBuffer);

        if ( rearData >= length ) {
            /* rear space is enough */
            memcpy ( pDst, pRead, length );
            pRead += length;

            if ( pRead >= ( pBuff->pDataBuffer + pBuff->u4BufferSize ) ) {
                pBuff->pRead = pBuff->pDataBuffer;
            } else {
                pBuff->pRead = pRead;
            }

            return;
        } else {
            memcpy ( pDst, pRead, rearData );

            pDst += rearData;
            length -= rearData;

            pRead = pBuff->pDataBuffer;
        }
    }

    /* case 1 */
    memcpy ( pDst, pRead, length );
    pRead += length;

    pBuff->pRead = pRead;
}

void cmux_discardDataInBuffer ( CmuxRingBufferT *pBuff, kal_uint32 length )
{
    /* Use auto variables instead of globals to avoid race conditions */
    kal_uint8 *pRead = pBuff->pRead;
    kal_uint8 *pWrite = pBuff->pWrite;

    LOGW("pBuff = 0x%x, length = %d", pBuff, length);

    if ( pWrite < pRead ) {
        /* case 2 */
        kal_uint32 rearData = pBuff->u4BufferSize - ( pRead - pBuff->pDataBuffer );

        if ( rearData >= length ) {
            pRead += length;

            if ( pRead >= ( pBuff->pDataBuffer + pBuff->u4BufferSize ) ) {
                pBuff->pRead = pBuff->pDataBuffer;
            } else {
                pBuff->pRead = pRead;
            }

            return;
        } else {
            pRead = pBuff->pDataBuffer;
            length -= rearData;
        }
    }

    /* case 1 */
    pRead += length;

    if ( pWrite >= pRead ) {
        pBuff->pRead = pRead;
    } else {
        pBuff->pRead = pWrite;
    }
}

void cmux_writeBuffer ( CmuxRingBufferT *pBuff, kal_uint8 *pSrc,
                        kal_uint32 length )
{
    kal_uint8 *pRead = pBuff->pRead;
    kal_uint8 *pWrite = pBuff->pWrite;

    if ( length > cmux_getBufferSpaceAvail ( pBuff ) ) {
        configASSERT ( 0 );
    }

    if ( pWrite >= pRead ) {
        /* case 1 */
        kal_uint32 rearSpace = pBuff->u4BufferSize - ( pWrite - pBuff->pDataBuffer );

        if ( rearSpace >= length ) {
            /* rear space is enough */
            memcpy ( pWrite, pSrc, length );
            pWrite += length;

            if ( pWrite >= ( pBuff->pDataBuffer + pBuff->u4BufferSize ) ) {
                pBuff->pWrite = pBuff->pDataBuffer;
            } else {
                pBuff->pWrite = pWrite;
            }

            return;
        } else {
            memcpy ( pWrite, pSrc, rearSpace );

            pSrc += rearSpace;
            length -= rearSpace;

            pWrite = pBuff->pDataBuffer;
        }
    }

    /* case 2 */
    memcpy ( pWrite, pSrc, length );
    pWrite += length;

    pBuff->pWrite = pWrite;
}

kal_uint8 *cmux_reserveBufferSpace ( CmuxRingBufferT *pBuff,
                                     kal_uint32 *pLength )
{
    *pLength = cmux_getBufferSpaceAvail ( pBuff );

    return pBuff->pWrite;
}

void  cmux_admitReservedBuffer( CmuxRingBufferT *pBuff, kal_uint8 *pReserve )
{
    // add by mtk02285 on 20080815
    // fix to cmux_writeReservedBuffer() write pointer error
    if (pBuff->pRead > pBuff->pWrite) { //modified on 20081021
        if ((pReserve < pBuff->pWrite) || (pReserve >= pBuff->pRead)) {
            configASSERT(0);// pReserve: corrupt data to read
        }
    } else if (pBuff->pRead < pBuff->pWrite) { //modified on 20081021
        if ((pReserve < pBuff->pWrite) && (pReserve >= pBuff->pRead)) {
            configASSERT(0);// pReserve: corrupt data to read
        }
    } else if ((pReserve > (pBuff->pDataBuffer + pBuff->u4BufferSize)) ||
               (pReserve < pBuff->pDataBuffer) || (pReserve == NULL)) {
        configASSERT(0); // make sure pReserve is correctly inited in cmux_getBufferSpaceAvail()
    }

    pBuff->pWrite = pReserve;
}

kal_uint8 *cmux_writeReservedBuffer ( CmuxRingBufferT *pBuff,
                                      kal_uint8 *pRev, kal_uint8 *pSrc, kal_uint32 length )
{
    kal_uint8 *pRead = pBuff->pRead;

    kal_uint32 freeSpace;

    if ((pRev >= ( pBuff->pDataBuffer + pBuff->u4BufferSize )) ||
            (pRev < pBuff->pDataBuffer) || (pRev == NULL)) {
        configASSERT(0); // make sure pReserve is correctly inited in cmux_getBufferSpaceAvail()
    }

    // Get free space
    if ( pRev >= pRead ) {
        freeSpace = pBuff->u4BufferSize - (pRev - pRead) - 1;
    } else {
        freeSpace = ( pRead - pRev - 1 );
    }

    // Free Space is not enough
    if ( length > freeSpace ) {
        length = freeSpace;
    }

    if ( pRev >= pRead ) {
        /* case 1 */
        kal_uint32 rearSpace = pBuff->u4BufferSize - ( pRev - pBuff->pDataBuffer );

        if ( rearSpace >= length ) {
            /* space enough */
            memcpy ( pRev, pSrc, length );
            pRev += length;

            if ( pRev >= ( pBuff->pDataBuffer + pBuff->u4BufferSize ) ) {
                return pBuff->pDataBuffer;
            } else {
                return pRev;
            }
        } else {
            memcpy ( pRev, pSrc, rearSpace );

            pSrc += rearSpace;
            length -= rearSpace;

            pRev = pBuff->pDataBuffer;
        }
    }

    /* case 2 */
    memcpy ( pRev, pSrc, length );
    pRev += length;

    return pRev;
}

kal_bool cmux_isBufferEmpty ( CmuxRingBufferT *pBuff )
{
    if (pBuff->pWrite == pBuff->pRead) {
        return KAL_TRUE;
    }

    return KAL_FALSE;
}

kal_uint32 cmux_getBufferDataAvail ( CmuxRingBufferT *pBuff )
{
    /* Use auto variables instead of globals to avoid race conditions */
    kal_uint8 *pWrite = pBuff->pWrite;
    kal_uint8 *pRead = pBuff->pRead;

    if (pWrite >= pRead) {
        /* case 1 */
        return pWrite - pRead;
    } else {
        /* case 2 */
        return pBuff->u4BufferSize - (pRead - pWrite);
    }
}

kal_uint32 cmux_getBufferSpaceAvail ( CmuxRingBufferT *pBuff )
{
    /* Use auto variables instead of globals to avoid race conditions */
    kal_uint8 *pWrite = pBuff->pWrite;
    kal_uint8 *pRead = pBuff->pRead;

    if ( pBuff->u4BufferSize == 0 ) {
        return 0;
    }

    if (pWrite >= pRead) {
        /* case 1 */
        return pBuff->u4BufferSize - (pWrite - pRead) - 1;
    } else {
        /* case 2 */
        return pRead - pWrite - 1;
    }
}

