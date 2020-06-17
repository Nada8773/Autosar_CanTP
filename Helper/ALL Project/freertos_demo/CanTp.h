 /*******************************************************************************************
 *                                                                                          *
 * File Name   : CanTp.h                                                                    *
 *                                                                                          *
 * Author      : ITI AUTOSAR CanTP Team                                                     *
 *                                                                                          *
 * Platform    : TivaC                                                                      *
 *                                                                                          *
 * Date        : 6 Jun 2020                                                                 *
 *                                                                                          *
 * Version     : 4.0.0                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR Basic Software module CAN Transport Layer            *
 *               header file Release 4.3.1                                                  *
 *                                                                                          *
 ********************************************************************************************/


#ifndef CANTP_H_
#define CANTP_H_

#include "ComStack_Types.h"


typedef struct
{

}CanTp_ConfigType;

/**************************** AUTOSAR-Compliant APIs' Prototypes ******************************************/

void CanTp_Init(const CanTp_ConfigType* CfgPtr);
void CanTp_GetVersionInfo(Std_VersionInfoType* versioninfo);
void CanTp_Shutdown(void);
Std_ReturnType CanTp_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
Std_ReturnType CanTp_CancelTransmit(PduIdType TxPduId);
Std_ReturnType CanTp_CancelReceive(PduIdType RxPduId);
Std_ReturnType CanTp_ChangeParameter(PduIdType id, TPParameterType parameter, uint16 value);
Std_ReturnType CanTp_ReadParameter(PduIdType id, TPParameterType parameter, uint16* value);
void CanTp_MainFunction(void);


#endif /* CANTP_H_ */
