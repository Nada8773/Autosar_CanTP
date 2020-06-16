 /*******************************************************************************************
 *                                                                                          *
 * File Name   : CanTp_Cfg.h                                                                *
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

#define CANTP_PADDING_BYTE                         0U
#define DET_ERROR_STATUS                           STD_ON

#define CANTP_USE_STANDARD_CAN                     STD_ON
#define CANTP_USE_CAN_FD                           STD_OFF


#define CANTP_USE_PHYSICAL_REQUEST_ADDRESSING      STD_ON
#define CANTP_USE_FUNCTIONAL_REQUEST_ADDRESSING    STD_OFF

#if     (CANTP_USE_CAN_FD == STD_OFF)
#define MAX_SEGMENT_DATA_SIZE       8U
#else   
#define MAX_SEGMENT_DATA_SIZE       64U
#endif



#define CANTP_NSDU_CONFIG_LIST_SIZE_RX  2
#define CANTP_NSDU_CONFIG_LIST_SIZE_TX  2

/* Extern Array */
extern const CanTpTxNSdu_s CanTpTxNSdu[];
extern const CanTpRxNSdu_s CanTpRxNSdu[];



#endif /* CANTP_CFG_H_ */
