 /*******************************************************************************************
 *                                                                                          *
 * File Name   : CanTp_Cbk.h                                                                *
 *                                                                                          *
 * Author      : ITI AUTOSAR CanTP Team                                                     *
 *                                                                                          *
 * Platform    : TivaC                                                                      *
 *                                                                                          *
 * Date        : 6 Jun 2020                                                                 *
 *                                                                                          *
 * Version     : 4.0.0                                                                      *
 *                                                                                          *
 * Description : header file contain CanTp callback function Release 4.3.1                  *
 *                                                                                          *
 ********************************************************************************************/


#ifndef CANTP_CBK_H_
#define CANTP_CBK_H_

FUNC(void,CANTP) CanTp_TxConfirmation( VAR(PduIdType,AUTOMATIC) TxPduId, VAR(Std_ReturnType,AUTOMATIC) result );
FUNC(void,CANTP) CanTp_RxIndication( VAR(PduIdType,AUTOMATIC) RxPduId, CONSTP2VAR(PduInfoType, CANTP, AUTOMATIC) PduInfoPtr);


#endif /* CANTP_CBK_H_ */
