
/*******************************************************
 *
 * File Name: Com.h
 *
 * Author: AUTOSAR COM Team 
 * 
 * Date Created: 6 March 2019
 * 
 * Version  : 01
 * 
 ********************************************************/

#ifndef COM_H_
#define COM_H_

#include "Std_Types.h"
#include "Com_Cfg.h"
#include "ComStack_Types.h"
#include "Com_Types.h"


/************************************************************************
 *                      Functions Prototypes                            *
 ************************************************************************/

/*initializes internal and external interfaces and variables of the COM module */
void Com_Init( const ComConfig_type* config);

/* Processing of the AUTOSAR COM module's receive activities (PDU To Signal) */
void Com_MainFunctionRx(void);

/* Processing of the AUTOSAR COM module's transmission activities (Signal To PDU)*/
void Com_MainFunctionTx(void);

/* Updates the signal object identified by SignalId with the signal referenced by the SignalDataPtr parameter */
uint8 Com_SendSignal( Com_SignalIdType SignalId, const void* SignalDataPtr );

/*The service Com_ReceiveSignalGroup copies the received signal group from the I-PDU to the shadow buffer.*/
uint8 Com_ReceiveSignalGroup( Com_SignalGroupIdType SignalGroupId );

/*The service Com_SendSignalGroup copies the content of the associated shadow buffer to the associated I-PDU.*/
uint8 Com_SendSignalGroup( Com_SignalGroupIdType SignalGroupId );

/* Copies the data of the signal identified by SignalId to the location specified by SignalDataPtr */
uint8 Com_ReceiveSignal( Com_SignalIdType SignalId, void* SignalDataPtr );

/* the I-PDU with the given ID is triggered for transmission */
Std_ReturnType Com_TriggerIPDUSend( PduIdType PduId );


#endif

