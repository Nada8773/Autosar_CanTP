 /*******************************************************************************************
 *                                                                                          *
 * File Name   : CanTp_Cfg.h                                                           *
 *                                                                                          *
 * Author      : ITI AUTOSAR CanTP Team                                                     *
 *                                                                                          *
 * Platform    : TivaC                                                                      *
 *                                                                                          *
 * Date        : 6 Jun 2020                                                                 *
 *                                                                                          *
 * Version     : 4.0.0                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR canTp Configuration header file Release 4.3.1        *
 *                                                                                          *
 ********************************************************************************************/

#ifndef CANTP_CFG_H_
#define CANTP_CFG_H_

#define CANTP_USE_STANDARD_CAN                     STD_ON
#define CANTP_USE_CAN_FD                           STD_OFF

#define CANTP_USE_STANDARD_ADDRESSING              STD_ON
#define CANTP_USE_EXTENDED_ADDRESSING              STD_OFF
#define CANTP_USE_NORMAL_FIXED_ADDRESSING          STD_OFF
#define CANTP_USE_MIXED_ADDRESSING                 STD_OFF
#define CANTP_USE_MIXED29BIT_ADDRESSING            STD_OFF

#define CANTP_USE_PHYSICAL_REQUEST_ADDRESSING      STD_ON
#define CANTP_USE_FUNCTIONAL_REQUEST_ADDRESSING    STD_OFF

#if     (CANTP_USE_CAN_FD == STD_OFF)
#define MAX_SEGMENT_DATA_SIZE       8U
#else   
#define MAX_SEGMENT_DATA_SIZE       64U
#endif

#if     (CANTP_USE_EXTENDED_ADDRESSING == STD_OFF)   //The old were MAX_PAYLOAD_SF_STD_ADDR,MAX_PAYLOAD_SF_EXT_ADDR
#define MAX_PAYLOAD_SF              7U
#define MAX_PAYLOAD_FF              6U
#define MAX_PAYLOAD_CF              7U
#else
#define MAX_PAYLOAD_SF              6U
#define MAX_PAYLOAD_FF              5U
#define MAX_PAYLOAD_CF              6U
#endif

uint8 MaxSegSize = MAX_SEGMENT_DATA_SIZE;

#define CANTP_NSDU_CONFIG_LIST_SIZE_RX	2
#define CANTP_NSDU_CONFIG_LIST_SIZE_TX	2


extern const CanTp_TxNSduType CanTpTxNSdu[];
extern const CanTp_RxNSduType CanTpRxNSdu[];

#endif /* CANTP_CFG_H_ */
