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

FUNC(void,CANTP) CanTp_Init(CONSTP2VAR(CanTp_ConfigType, CANTP, AUTOMATIC) CfgPtr);

FUNC(void,CANTP) CanTp_Shutdown(void);

FUNC(Std_ReturnType,CANTP) CanTp_Transmit( VAR(PduIdType,AUTOMATIC) TxPduId,
		                                          CONSTP2VAR(PduInfoType, CANTP, AUTOMATIC) PduInfoPtr);
FUNC(void,CANTP) CanTp_MainFunction(void);



#endif /* CANTP_H_ */
