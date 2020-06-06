 /*******************************************************************************************
 *                                                                                          *
 * File Name   : CanTp.c                                                                    *
 *                                                                                          *
 * Author      : ITI AUTOSAR CanTP Team                                                     *
 *                                                                                          *
 * Platform    : TivaC                                                                      *
 *                                                                                          *
 * Date        : 6 Jun 2020                                                                 *
 *                                                                                          *
 * Version     : 4.0.0                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR  Basic Software module CAN Transport Layer           *
 *               source file Release 4.3.1                                                  *
 *                                                                                          *
 ********************************************************************************************/

#include "CanTp.h"
#include "CanTp_Cfg.h"
#include "CanTp_Cbk.h"
#include "SchM_CanTp.h"
#include "MemMap.h"
#include "Det.h"


/*******************************************************************************************
 * Service name      : CanTp_Init                                                          *
 * Service ID        : 0x01                                                                *
 * Sync/Async        : Synchronous                                                         *
 * Reentrancy        : Non Reentrant                                                       *
 * Parameters (in)   : CfgPtr Pointer to the CanTp post-build configuration data.          *
 * Parameters (inout): None                                                                *
 * Parameters (out)  : None                                                                *
 * Return value      : None                                                                *
 * Description       : This function initializes the CanTp module.                         *
 ********************************************************************************************/
void CanTp_Init(const CanTp_ConfigType* CfgPtr)
{

}

/*************************************************************************************************
 * Service name      : CanTp_Transmit                                                            *
 * Service ID        : 0x49                                                                      *
 * Sync/Async        : Synchronous                                                               *
 * Reentrancy        : Reentrant for different PduIds. Non reentrant for the same PduId.         *
 * Parameters (in)   : xPduId Identifier of the PDU to be transmitted                            *
 *                     PduInfoPtr Length of and pointer to the PDU data and pointer to MetaData. *
 * Parameters (inout): None                                                                      *
 * Parameters (out)  : None                                                                      *
 * Return value      : Std_ReturnType E_OK: Transmit request has been accepted.                  *
 *                                    E_NOT_OK: Transmit request has not been accepted.          *                                                   *
 * Description       : Requests transmission of a PDU.                                           *
 *************************************************************************************************/
Std_ReturnType CanTp_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr)
{

}


/**********************************************************************
 * Service name      : CanTp_MainFunction                             *
 * Service ID        : 0x06                                           *
 * Parameters (in)   : None                                           *
 * Parameters (inout): None                                           *
 * Parameters (out)  : None                                           *
 * Return value      : None                                           *
 * Description       : The main function for scheduling the CAN TP.   *
 **********************************************************************/
void CanTp_MainFunction(void)
{
}


/********************************************************************************************** *
 * Service name      : CanTp_Transmit                                                           *
 * Service ID        : 0x40                                                                     *
 * Sync/Async        : Synchronous                                                              *
 * Reentrancy        : Reentrant for different PduIds. Non reentrant for the same PduId.        *
 * Parameters (in)   : TxPduId ID of the PDU that has been transmitted.                         *
 *                     result E_OK: The PDU was transmitted.                                    *
 *                            E_NOT_OK: Transmission of the PDU failed                          *
 * Parameters (inout): None                                                                     *
 * Parameters (out)  : None                                                                     *
 * Return value      : None                                                                     *
 * Description       : The lower layer communication interface module confirms the transmission *
 *                     of a PDU, or the failure to transmit a PDU.                              *
 ************************************************************************************************/

void CanTp_TxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{

}


/*****************************************************************************************************
 * Service name      : CanTp_Transmit                                                                *
 * Service ID        : 0x42                                                                          *
 * Sync/Async        : Synchronous                                                                   *
 * Reentrancy        : Reentrant for different PduIds. Non reentrant for the same PduId.             *
 * Parameters (in)   : RxPduId ID of the received PDU.                                               *
 *                     PduInfoPtr Contains the length (SduLength) of the received PDU,               *
 *                     a pointer to a buffer (SduDataPtr) containing the PDU, and the MetaData       *
 *                     related to this PDU.                                                          *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : Std_ReturnType E_OK: Transmit request has been accepted.                      *
 *                                    E_NOT_OK: Transmit request has not been accepted.              *
 * Description       :Indication of a received PDU from a lower layer communication interface module *
 *****************************************************************************************************/
void CanTp_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{

}


