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


void CanTp_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
void CanTp_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);

#endif /* CANTP_CBK_H_ */
