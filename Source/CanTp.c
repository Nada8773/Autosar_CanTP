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
#include "Platform_Types.h"
#include "Compiler.h"
#include "Std_Types.h"
#include "ComStack_Types.h"

#include "CanTp_Types.h"
#include "CanTp.h"
#include "CanTp_Cfg.h"
#include "CanTp_LCfg.h"
#include "CanTp_Cbk.h"
#include "CanTp_Det.h"
#include "Det.h"
#include "PduR.h"
#include "CanIf.h"
#include "CanTp_Macros.h"
/* TODO Do it Manual */
#include "CanTp_CanIf.h"
#include "PduR_CanTp.h"


/* Global Variable TODO #if test */
uint8 g_DataCanIf[8] ;


/* Array index is CanTp Npdu Id
 * Data is CanIF Pdu Id   */
uint8 CanTpNPduID_ToCanIfPduId[NUMBER_OF_TXNPDU] = CANTPNPDUID_TOCANTFPDUID_MAPPING ;

/* Array index is CanTp Npdu Id
 * Data is Pdur Ipdu Id   */
uint8 CanTpNPduID_ToPdurIPduId[NUMBER_OF_RXNPDU] = CANTPNPDUID_TOPDURPDUID_MAPPING ;

/* TODO Replace it */
uint8 canif_ids[]={cantp_canif_tx};
uint8 cantp_com_copy_tx[]={CanTp_copy_cantp};
uint8 cantp_com_rx_or_copy[]= {cantp_com_rx};
uint8 cantp_cainf_FC[] ={CanTpTxFcNPdu_canif};

/* Static Function Prototypes */
static void PadFrame(PduInfoType *PduInfoPtr);
static void StartNewTransmission(RunTimeInfo_s *txRuntimeParam);
static void StartNewReception(RunTimeInfo_s *rxRuntimeParam);
static void ReceiveSingleFrame(const CanTpRxNSdu_s *rxConfig, RunTimeInfo_s *rxRuntime, const PduInfoType *rxPduData);
static void ReceiveFirstFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, const PduInfoType *CanTpPduData);
static void SendFlowControlFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, BufReq_ReturnType flowControlStatus);
static void ReceiveConsecutiveFrame(const CanTpRxNSdu_s *rxConfig, RunTimeInfo_s *rxRuntime, const PduInfoType *rxPduData);
static void ReceiveFlowControlFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo_s *txRuntime, const PduInfoType *PduData);
static void HandleReceivedFrame(PduIdType RxPduId, const PduInfoType *CanTpPduData);
static void HandleNextTxFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo_s *txRuntime);
static BufReq_ReturnType SendNextTxFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo_s *txRuntime);


/* Global Runtime Object */
RunTimeInfo_s CanTpRunTimeTxData[CANTP_NSDU_CONFIG_LIST_SIZE_TX];
RunTimeInfo_s CanTpRunTimeRxData[CANTP_NSDU_CONFIG_LIST_SIZE_RX];

/* [SWS_CanTp_00027] The CanTp module shall have two internal states, CANTP_OFF and CANTP_ON. */
/* [SWS_CanTp_00168] The CanTp module shall be in the CANTP_OFF state after power up. */


/* Static Global Variable to hold CanTp Internal State {CANTP_OFF,CANTP_ON}*/
CanTp_InternalStateType InternalState = (CanTp_InternalStateType)CANTP_OFF;


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

	PduIdType ChannelCounter = 0 ;

	/* [SWS_CanTp_00030] The function CanTp_Init shall initialize all global variables of the module and sets
	 * all transport protocol connections in a sub-state of CANTP_ON, in which neither segmented transmission
	 * nor segmented reception are in progress (Rx thread in state CANTP_RX_WAIT and Tx thread in state CANTP_TX_WAIT). (SRS_Can_01075)*/

	for( ChannelCounter=0; ChannelCounter < MAX_CHANNEL_COUNT; ChannelCounter++ ) //TODO: replace for loop if possible
	{
		StartNewTransmission(&CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx]);
		StartNewReception(&CanTpRunTimeRxData[ChannelInfo[ChannelCounter].StIdx]);
	}

	/* [ SWS_CanTp_00170] The CanTp module shall change to the internal state CANTP_ON
	 * when the CanTp has been successfully initialized with CanTp_Init(). (SRS_Can_01075)*/

	/* Putting CanTp Module in the ON state to let other functions work */
	InternalState = (CanTp_InternalStateType)CANTP_ON;



}

/*******************************************************************************************
 * Service name      : CanTp_Shutdown                                                      *
 * Service ID        : 0x02                                                                *
 * Sync/Async        : Synchronous                                                         *
 * Reentrancy        : Non Reentrant                                                       *
 * Parameters (in)   : None                                                                *
 * Parameters (inout): None                                                                *
 * Parameters (out)  : None                                                                *
 * Return value      : None                                                                *
 * Description       : This function is called to shutdown the CanTp module.               *
 *******************************************************************************************/
void CanTp_Shutdown(void)
{
	/* [SWS_CanTp_00202] The function CanTp_Shutdown shall close all pending transport protocol connections,
	 * free all resources and set the CanTp module into the CANTP_OFF state. */

	/*[SWS_CanTp_00010] The function CanTp_Shutdown shall stop the CanTp module properly.(SRS_BSW_00336)*/

	/* Putting CanTp Module in the OFF state to stop module safely */
	InternalState = (CanTp_InternalStateType)CANTP_OFF;
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

	Std_ReturnType      TransmitRequest_Status = E_NOT_OK;
	//TODO: remove the 2 pointers
	const CanTpTxNSdu_s *txConfig  = NULL_PTR;
	RunTimeInfo_s       *txRuntime = NULL_PTR;
	uint8 MaxPayload ;
	FrameType Frame ;

	/* [SWS_CanTp_00225] For specific connections that do not use MetaData, the
                         function CanTp_Transmit shall only use the full SduLength
                         information and shall not use the available N-SDU data buffer
                         in order to prepare Single Frame or First Frame PCI */

	txConfig    =  &CanTpTxNSdu[TxPduId] ;
	txRuntime   =  &CanTpRunTimeTxData[TxPduId] ;



	txRuntime->IfBuffer.IFdataPtr = g_DataCanIf ;


	if( InternalState == CANTP_OFF )
	{
#if DET_ERROR_STATUS == STD_ON
		/* API service used without module initialization : On any API call except CanTp_Init() and
               CanTp_GetVersionInfo() if CanTp is in state CANTP_OFF" */
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID,  CANTP_TRANSMIT_SERVICE_ID, CANTP_E_UNINIT);
#endif
	}


	/* API service called with a NULL pointer. In case of this error, the API service
      shall return immediately without any further action, besides reporting this
      development error. */
	else if( PduInfoPtr == NULL_PTR )
	{
#if DET_ERROR_STATUS == STD_ON
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_PARAM_POINTER);
#endif
	}

	/* TODO Check ID */
	/*CANTP_E_INVALID_TX_ID
      Invalid Transmit PDU identifier (e.g. a service is called withan inexistent Tx PDU identifier) */
	else if(TxPduId >= CANTP_NSDU_CONFIG_LIST_SIZE_TX)
	{
#if DET_ERROR_STATUS == STD_ON
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_INVALID_TX_ID);
#endif
	}

	/* TODO CANTP_E_PARAM_CONFIG & CANTP_E_PARAM_ID
            API service called with wrong parameter(s): When CanTp_Transmit is called for a none
            configured PDU identifier or with an identifier for a received PDU */


	/* [SWS_CanTp_00123] If the configured transmit connection channel is in use (state CANTP_TX_PROCESSING),
                         the CanTp module shall reject new transmission requests linked to this channel.
                         To reject a transmission, CanTp returns E_NOT_OK when the
                         upper layer asks for a transmission with the CanTp_Transmit() function */
	else
	{
		if (CanTpRunTimeTxData[TxPduId].state == IDLE)
		{
			CanTpRunTimeTxData[TxPduId].transferTotal = PduInfoPtr->SduLength;

			CanTpRunTimeTxData[TxPduId].mode          = CANTP_TX_PROCESSING;

			CanTpRunTimeTxData[TxPduId].IfBuffer.IFByteCount   = 0;

			if ( txConfig->CanTpTxAddressingFormat == CANTP_STANDARD )
			{
				MaxPayload = MAX_PAYLOAD_STANDRAD_SF ;
			}
			else if ( txConfig->CanTpTxAddressingFormat == CANTP_EXTENDED )
			{
				MaxPayload = MAX_PAYLOAD_EXTENDED_SF ;
			}
			else
			{
				/* Do Nothing */
			}

			if( CanTpRunTimeTxData[TxPduId].transferTotal <= MaxPayload ) /* Single Frame */
			{
				Frame = SINGLE_FRAME ;
			}
			else /* First Frame */
			{
				Frame = FIRST_FRAME ;
			}
			switch(Frame)
			{
			case SINGLE_FRAME :
				CanTpRunTimeTxData[TxPduId].IfBuffer.IFdataPtr[CanTpRunTimeTxData[TxPduId].IfBuffer.IFByteCount++] = ISO15765_TPCI_SF | (CanTpRunTimeTxData[TxPduId].transferTotal);
				CanTpRunTimeTxData[TxPduId].pdurBuffer.SduLength             = CanTpRunTimeTxData[TxPduId].transferTotal ;
				CanTpRunTimeTxData[TxPduId].state                            = TX_WAIT_TRANSMIT;
				TransmitRequest_Status                      = E_OK ;
				break;

			case  FIRST_FRAME :
				if ( txConfig->CanTpTxAddressingFormat == CANTP_STANDARD )
				{
					MaxPayload = MAX_PAYLOAD_STANDRAD_FF ;
				}
				else if ( txConfig->CanTpTxAddressingFormat == CANTP_EXTENDED )
				{
					MaxPayload = MAX_PAYLOAD_EXTENDED_FF ;
				}
				else
				{
					/* Nothing */
				}


				CanTpRunTimeTxData[TxPduId].IfBuffer.IFdataPtr[CanTpRunTimeTxData[TxPduId].IfBuffer.IFByteCount++] = ISO15765_TPCI_FF | (uint8)((CanTpRunTimeTxData[TxPduId].transferTotal & 0xf00) >> 8);
				CanTpRunTimeTxData[TxPduId].IfBuffer.IFdataPtr[CanTpRunTimeTxData[TxPduId].IfBuffer.IFByteCount++] = (uint8)(CanTpRunTimeTxData[TxPduId].transferTotal & 0xff);
				CanTpRunTimeTxData[TxPduId].pdurBuffer.SduLength             =  MaxPayload    ;
				CanTpRunTimeTxData[TxPduId].state                            = TX_WAIT_TRANSMIT;
				TransmitRequest_Status                      = E_OK ;
				break;
			default:
				TransmitRequest_Status                     = E_NOT_OK ;
				break;

			}
		}

		else if(CanTpRunTimeTxData[TxPduId].state == CANTP_TX_PROCESSING)
		{
			TransmitRequest_Status = E_NOT_OK ;
		}
		else
		{
			/* Nothing */
		}
	}
	return TransmitRequest_Status;
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
	PduIdType ChannelCounter ;

	static uint8          FCWaitCount     = 0;

	const  CanTpTxNSdu_s *txConfig        = NULL_PTR;
	const  CanTpRxNSdu_s *rxConfig        = NULL_PTR;

	RunTimeInfo_s *txRuntime       = NULL_PTR;
	RunTimeInfo_s *rxRuntime       = NULL_PTR;

	PduInfoType   *NextBlock       = NULL_PTR;
	PduInfoType   *Request         = NULL_PTR;
	PduLengthType  RemainingBytes  = 0;

	BufReq_ReturnType ret          = BUFREQ_OK;


	/* [SWS_CanTp_00238] The CanTp module shall perform segmentation and reassembly tasks only when
	 * the CanTp is in the CANTP_ON state. */

	/* Checking that CanTp is in the CANTP_ON state which means that's initialized before */
	if( InternalState != CANTP_ON)
	{
		/* [SWS_CanTp_00031] If development error detection for the CanTp module is enabled the CanTp module
		 * shall raise an error (CANTP_E_UNINIT) when the PDU Router or CAN Interface tries to use any function
		 * (except CanTp_GetVersionInfo) before the function CanTp_Init has been called.(SRS_Can_01076)*/

		/* Det_Reporting with uninitialized error */
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID,  CANTP_MAINFUNCTION_SERVICE_ID, CANTP_E_UNINIT);

	}
	else
	{

		/* [SWS_CanTp_00164] The main function for scheduling the CAN TP (Entry point for scheduling)
		 * The main function will be called by the Schedule Manager or by the Free Running Timer
		 * module according of the call period needed. CanTp_MainFunction is involved in handling
		 * of CAN TP timeouts N_As, N_Bs, N_Cs, N_Ar, N_Br,N_Cr and STMmin.(SRS_BSW_00424, SRS_BSW_00373)*/

		/*------------------------------------------------Sender Side--------------------------------------------------------*/

		/* Handling Sender Side timeouts N_As, N_Bs, N_Cs and STmin */

		/* for each Channel ID */
		for( ChannelCounter=0; ChannelCounter < MAX_CHANNEL_COUNT; ChannelCounter++ )
		{
			txConfig  = (CanTpTxNSdu_s*)&CanTpTxNSdu[ChannelInfo[ChannelCounter].StIdx];
			rxConfig  = (CanTpRxNSdu_s*)&CanTpRxNSdu[ChannelInfo[ChannelCounter].StIdx];

			txRuntime = (RunTimeInfo_s*)&CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx];
			rxRuntime = (RunTimeInfo_s*)&CanTpRunTimeRxData[ChannelInfo[ChannelCounter].StIdx];

			switch (CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx].state)
			{

			case TX_WAIT_STMIN:

				/* Make sure that STmin timer has expired */
				TIMER_DECREMENT(CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx].stateTimeoutCount);

				if (CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx].stateTimeoutCount != 0)
				{
					break;
				}
				CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx].state = TX_WAIT_TRANSMIT;                /* when stateTimeoutCount = 0 */

				/* Update state timeout with the remaining time of N_Cr after ST has passed, as shown in ISO 15765-2 */
				CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx].stateTimeoutCount = (rxConfig->CanTpNcr) - (CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx].STmin);

				/* No need for break here as we want to go for next case directly not to be delayed for main period time */
			case TX_WAIT_TRANSMIT:
			{
				ret = SendNextTxFrame(txConfig, &CanTpRunTimeTxData[ChannelInfo[ChannelCounter].StIdx]);

				//TODO: NOK then BUSY then OK
				if ( ret == BUFREQ_OK )
				{
					/* successfully sent frame, wait for tx confirm */
				}

				else if( ret == BUFREQ_E_BUSY )
				{
					/* Decrement to check N_Cs timeout */
					TIMER_DECREMENT(txRuntime->stateTimeoutCount);
					if (txRuntime->stateTimeoutCount == 0)
					{
						txRuntime->state = IDLE;
						txRuntime->mode  = CANTP_TX_WAIT;

						/* [SWS_CanTp_00204] The CanTp module shall notify the upper layer by calling
						 * the PduR_CanTpTxConfirmation callback when the transmit request has been completed.*/
						PduR_CanTpTxConfirmation(cantp_com_copy_tx[txConfig->CanTpTxNSduId], E_NOT_OK);
					}
					else
					{
						/* [SWS_CanTp_00184] If the PduR_CanTpCopyTxData() function returns BUFREQ_E_BUSY,
						 * the CanTp module shall later (implementation specific) retry to copy the data.*/

						/* if N_Cs is still active and BUFREQ_E_BUSY, exit main and try re-copying data
						 * the next time entering main */
					}
				}

				else /* ret == BUFREQ_E_NOT_OK */
				{
					txRuntime->state = IDLE;
					txRuntime->mode  = CANTP_TX_WAIT;

					/* As per [SWS_CanTp_00204] mentioned above */
					PduR_CanTpTxConfirmation(cantp_com_copy_tx[txConfig->CanTpTxNSduId], E_NOT_OK);
				}

				break;
			}

			case TX_WAIT_FLOW_CONTROL:
			{
				/* Decrement to check N_Bs timeout */
				TIMER_DECREMENT(txRuntime->stateTimeoutCount);

				/* [SWS_CanTp_00316] In case of N_Bs timeout occurrence the CanTp module shall abort transmission
				 * of this message and notify the upper layer by calling the callback function PduR_CanTpTxConfirmation()
				 * with the result E_NOT_OK.*/
				if (txRuntime->stateTimeoutCount == 0)
				{
					/* [SWS_CanTp_00205] The CanTp module shall abort the transmit request and call the PduR_CanTpTxConfirmation
					 * callback function with the appropriate error result value if an error occurred (over flow, N_As timeout,
					 *  N_Bs timeout and so on).*/

					txRuntime->state = IDLE;
					txRuntime->mode  = CANTP_TX_WAIT;
					PduR_CanTpTxConfirmation(cantp_com_copy_tx[txConfig->CanTpTxNSduId], E_NOT_OK);
				}

				break;
			}

			case TX_WAIT_TX_CONFIRMATION:
			{
				/* Decrement to check N_As timeout */
				TIMER_DECREMENT(txRuntime->stateTimeoutCount);

				if (txRuntime->stateTimeoutCount == 0)
				{
					/* [SWS_CanTp_00075] When the transmit confirmation is not received after a maximum time (equal to N_As), the CanTp module shall abort the corresponding session.
					 * The N-PDU remains unavailable to other concurrent sessions until the TxConfirmation is received, successful or not.*/

					txRuntime->state = IDLE;
					txRuntime->mode  = CANTP_TX_WAIT;

					/* As per [SWS_CanTp_00204] mentioned above */
					PduR_CanTpTxConfirmation(cantp_com_copy_tx[txConfig->CanTpTxNSduId], E_NOT_OK);
				}

				break;
			}

			default:
				break;
			}

			/*-----------------------------------------------Receiver Side-------------------------------------------------------*/

			/* Handling Receiver Side timeouts N_Ar,N_Br and N_Cr */

			switch (rxRuntime->state)
			{
			case RX_WAIT_CONSECUTIVE_FRAME:
			{
				/* Decrement to check N_Cr timeout */
				TIMER_DECREMENT (rxRuntime->stateTimeoutCount);

				if (rxRuntime->stateTimeoutCount == 0)
				{
					/* [SWS_CanTp_00313] In case of N_Cr timeout occurrence the CanTp module shall abort reception
					 * and notify the upper layer of this failure by calling the indication function PduR_CanTpRxIndication()
					 * with the result E_NOT_OK. */

					rxRuntime->state = IDLE;
					rxRuntime->mode  = CANTP_RX_WAIT;
					PduR_CanTpRxIndication(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], E_NOT_OK);
				}
				break;
			}
			case RX_WAIT_TX_CONFIRMATION:
			{
				/* Decrement to check N_Ar timeout */
				TIMER_DECREMENT (rxRuntime->stateTimeoutCount);

				if (rxRuntime->stateTimeoutCount == 0)
				{
					/* [SWS_CanTp_00311] In case of N_Ar timeout occurrence (no confirmation from CAN driver for any of the FC frame sent)
					 * the CanTp module shall abort reception and notify the upper layer of this failure by calling the indication function
					 * PduR_CanTpRxIndication() with the result E_NOT_OK. */

					rxRuntime->state = IDLE;
					rxRuntime->mode  = CANTP_RX_WAIT;
					PduR_CanTpRxIndication(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], E_NOT_OK);
				}

				break;
			}

			case RX_WAIT_SDU_BUFFER:
			{
				/* Decrement to check N_Br timeout */
				TIMER_DECREMENT (rxRuntime->stateTimeoutCount);

				if (rxRuntime->stateTimeoutCount == 0) /* Check N_Br Timeout */
				{

					/* [SWS_CanTp_00223] The CanTp module shall send a maximum of WFTmax consecutive FC(WAIT) N-PDU. If this number is reached,
					 * the CanTp module shall abort the reception of this N-SDU (the receiver did not send any FC N-PDU, so the N_Bs timer expires
					 * on the sender side and then the transmission is aborted) and a receiving indication with E_NOT_OK occurs. */

					if( FCWaitCount <= (rxConfig->CanTpRxWftMax) )
					{
						FCWaitCount++;

						/*[SWS_CanTp_00341]If the N_Br timer expires and the available buffer size is still not big enough,
						 * the CanTp module shall send a new FC(WAIT) to suspend the N-SDU reception and reload the N_Br timer.*/

						SendFlowControlFrame(rxConfig, rxRuntime, FLOW_CONTROL_WAIT_FRAME);
						rxRuntime->stateTimeoutCount = rxConfig->CanTpNbr;

					}
					else
					{
						/* as per [SWS_CanTp_00223] mentioned above */

						/* Receive process is ended with failure */
						PduR_CanTpRxIndication(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], E_NOT_OK);
						rxRuntime->state = IDLE;
						rxRuntime->mode  = CANTP_RX_WAIT;

						/* Reset this variable for the next time entering this state */
						FCWaitCount      = 0;
					}

				}
				else            /*  stateTimeoutCount != 0, N_Br Timer is active */
				{

					/* [SWS_CanTp_00222]  While the timer N_Br is active, the CanTp module shall call the service PduR_CanTpCopyRxData()
					 * with a data length 0 (zero) and NULL_PTR as data buffer during each processing of the MainFunction.*/

					Request->SduDataPtr   = NULL_PTR;
					Request->SduLength    = 0;

					PduR_CanTpCopyRxData(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], Request, &rxRuntime->Buffersize);

					/* Assign 'NextBlock' with the next block information which in stored IFdata, IFByteCount*/
					NextBlock->SduDataPtr = rxRuntime->IfBuffer.IFdataPtr;
					NextBlock->SduLength  = rxRuntime->IfBuffer.IFByteCount;

					/* Check if the available PduR buffer size is sufficient for next block or not */
					if( (rxRuntime->Buffersize) >= (NextBlock->SduLength) )
					{
						/* [SWS_CanTp_00224] When the Rx buffer is large enough for the next block (directly after the First Frame
						 * or the last Consecutive Frame of a block, or after repeated calls to PduR_CanTpCopyRxData() according to
						 * SWS_CanTp_00222), the CanTp module shall send a Flow Control N-PDU with ClearToSend status (FC(CTS))
						 * and shall then expect the reception of Consecutive Frame N-PDUs.*/

						ret = PduR_CanTpCopyRxData(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], NextBlock, &rxRuntime->Buffersize);
						switch(ret)
						{
						case BUFREQ_OK:
						{
							RemainingBytes = (rxRuntime->transferTotal) - (rxRuntime->transferCount); //TODO: make sure it's updated by receive functions
							if(RemainingBytes > 0) /* Reception is finished and waiting for next CF */
							{
								/* as per [SWS_CanTp_00224] mentioned above */
								SendFlowControlFrame( rxConfig, rxRuntime, FLOW_CONTROL_CTS_FRAME);

								/* [SWS_CanTp_00312] The CanTp module shall start a time-out N_Cr at each indication of
								 * CF reception (except the last one in a block) and at each confirmation of a FC transmission
								 * that initiate a CF transmission on the sender side (FC with FS=CTS). */
								rxRuntime->stateTimeoutCount = (rxConfig->CanTpNcr);
								rxRuntime->state             = RX_WAIT_CONSECUTIVE_FRAME;
							}
							else /* Receive process is ended successfully */
							{
								PduR_CanTpRxIndication(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], E_OK);              /* CanTp_00214 */
								rxRuntime->state = IDLE;
								rxRuntime->mode  = CANTP_RX_WAIT;
							}

							break;
						}

						case BUFREQ_E_NOT_OK:
						{
							/* Receive process is ended with failure */

							PduR_CanTpRxIndication(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], E_NOT_OK);        /* CanTp_00214 */
							rxRuntime->state = IDLE;
							rxRuntime->mode  = CANTP_RX_WAIT;

							break;
						}

						default:
							break;
						}
					}

					else
					{
						/* If the available buffer size is not sufficient, do nothing as it will end up with break
						 * Until entering main function the next time */
					}

				}//end of else
				break;
			}//end of SDU_WAIT case
			default:
				break;
			}  // end of switch case

		} // end of 2nd for loop

	}//end of if-condition

}//end of main function


/********************************************************************************************** *
 * Service name      : CanTp_TxConfirmation                                                     *
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
s
void CanTp_TxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{
	FrameType IDtype;
	RunTimeInfo_s *txRuntime ;
	RunTimeInfo_s *rxRuntime ;

	const CanTpTxNSdu_s *txConfig = NULL_PTR;


	txRuntime   = &CanTpRunTimeTxData[TxPduId] ;
	rxRuntime   = &CanTpRunTimeRxData[TxPduId];

	txConfig    = &CanTpTxNSdu[TxPduId] ;

	/* [SWS_CanTp_00111] If called when the CanTp module is in the global state CANTP_ON,
                             the function CanTp_Init shall return the module to state Idle
                             (state = CANTP_ON, but neither transmission nor reception are in progress */

	/* [SWS_CanTp_00238] The CanTp module shall perform segmentation and reassembly tasks only when
	 * the CanTp is in the CANTP_ON state. */

	if( InternalState == CANTP_OFF )
	{
#if DET_ERROR_STATUS == STD_ON
		/* API service used without module initialization : On any API call except CanTp_Init() and
               CanTp_GetVersionInfo() if CanTp is in state CANTP_OFF" */
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TXCONFIRMATION_SERVICE_ID, CANTP_E_UNINIT);
#endif
	}

	/* CANTP_E_INVALID_RX_ID Invalid Receive PDU identifier (e.g. a service is called with
                              an inexistent Rx PDU identifier) */
	else if (TxPduId < CANTP_NSDU_CONFIG_LIST_SIZE_TX)
	{
		/*[SWS_CanTp_00355]  CanTp shall abort the corrensponding session, when
                         CanTp_TxConfirmation() is called with the result E_NOT_OK */

		switch(result)
		{
		case E_NOT_OK:
			txRuntime->state = IDLE;
			txRuntime->mode  = CANTP_TX_WAIT;
			break;
		case E_OK:
			if(txRuntime->state == TX_WAIT_TX_CONFIRMATION)
			{
				/* Get The Frame ID */
				IDtype = CanTpRunTimeTxData[TxPduId].IfBuffer.IFdataPtr[0] & ISO15765_TPCI_MASK;
				switch(IDtype)
				{
				case ISO15765_TPCI_SF:
					txRuntime->state = IDLE;
					txRuntime->mode  = CANTP_TX_WAIT;
					break;
				case ISO15765_TPCI_FF:
					txRuntime->stateTimeoutCount = txConfig->CanTpNbs;
					txRuntime->state             = TX_WAIT_FLOW_CONTROL;
					break;
				case ISO15765_TPCI_CF:
					HandleNextTxFrame(txConfig, txRuntime);
					break;
				default:
					break;
				}
			}
			else if(rxRuntime->state == RX_WAIT_TX_CONFIRMATION)
			{
				rxRuntime->state = RX_WAIT_CONSECUTIVE_FRAME;
				rxRuntime->mode  = CANTP_RX_WAIT;
				//CanTp_RxIndication();// for FC receive testing
			}
			break;
		default:
			break;
		}
	}

	else /* Invaild ID */
	{
#if DET_ERROR_STATUS == STD_ON
		txRuntime->state = IDLE;
		txRuntime->mode  = CANTP_TX_WAIT;
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TXCONFIRMATION_SERVICE_ID, CANTP_E_INVALID_TX_ID);
#endif
	}
}


/*****************************************************************************************************
 * Service name      : CanTp_RxIndication                                                            *
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
	/* API service called with a NULL pointer. In case of this error, the API service
      shall return immediately without any further action, besides reporting this
      development error. */
	if( PduInfoPtr == NULL_PTR )
	{
#if DET_ERROR_STATUS == STD_ON
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PARAM_POINTER);
#endif
	}


	/* [SWS_CanTp_00111] If called when the CanTp module is in the global state CANTP_ON,
                         the function CanTp_Init shall return the module to state Idle
                         (state = CANTP_ON, but neither transmission nor reception are in progress */

	/* [SWS_CanTp_00030] The function CanTp_Init shall initialize all global variables of the module and sets
	 * all transport protocol connections in a sub-state of CANTP_ON, in which neither segmented transmission
	 * nor segmented reception are in progress (Rx thread in state CANTP_RX_WAIT and Tx thread in state CANTP_TX_WAIT). (SRS_Can_01075)*/

	else if( InternalState == CANTP_OFF)
	{
#if DET_ERROR_STATUS == STD_ON
		/* API service used without module initialization : On any API call except CanTp_Init() and
               CanTp_GetVersionInfo() if CanTp is in state CANTP_OFF" */
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_UNINIT);
#endif
	}
	/* CANTP_E_INVALID_RX_ID
           Invalid Receive PDU identifier (e.g. a service is called with
           an inexistent Rx PDU identifier)*/
	else if (RxPduId >= CANTP_NSDU_CONFIG_LIST_SIZE_RX )
	{
#if DET_ERROR_STATUS == STD_ON

		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_INVALID_RX_ID);
#endif

	}
	else
	{
		HandleReceivedFrame(RxPduId, PduInfoPtr);
	}
}

/*****************************************************************************************************
 * Service name      : HandleReceivedFrame                                                           *
 * Parameters (in)   : RxPduId      -> ID of the received PDU.                                       *
 *                     CanTpPduData -> Contains the length (SduLength) of the received PDU,          *
 *                                     a pointer to a buffer (SduDataPtr) containing the PDU,        *
 *                                     and the MetaData  related to this PDU.                        *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Function Used By CanTp_RxIndication To Get the FrameType and Receive Data     *
 *****************************************************************************************************/
static void HandleReceivedFrame(PduIdType RxPduId, const PduInfoType *CanTpPduData)
{
	uint8 CanTpPduDataPCI_Offset ;
	uint8 CanTp_FrameID;
	//TODO: Removed Pointer
	const CanTpRxNSdu_s *rxNSduConfig = NULL_PTR;

	rxNSduConfig = &CanTpRxNSdu[RxPduId];

	if ( rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD ) /*(SF & FF & CF & FC)-> [ ID -> (7-4)Bits at first Byte ] */
	{
		CanTpPduDataPCI_Offset = 0;
	}
	else if ( rxNSduConfig->CanTpRxAddressingFormat == CANTP_EXTENDED )
	{
		CanTpPduDataPCI_Offset = 1;
	}
	else
	{
		/* Nothing */
	}

	/* Get The Frame ID */
	CanTp_FrameID   = CanTpPduData->SduDataPtr[CanTpPduDataPCI_Offset] & ISO15765_TPCI_MASK    ;


	switch (CanTp_FrameID)
	{

	case ISO15765_TPCI_SF: /* Single Frame ***************************************************/

		/*[SWS_CanTp_00345]  If frames with a payload <= 8 (either CAN 2.0 frames or small CAN FD frames)
                        are used for a Rx N-SDU and CanTpRxPaddingActivation is equal to CANTP_ON,
                        then CanTp receives by means of CanTp_RxIndication() call an SF Rx N-PDU
                        belonging to that N-SDU, with a length smaller than eight bytes
                        (i.e. PduInfoPtr.SduLength < 8), CanTp shall reject the reception.
                        The runtime error code CANTP_E_PADDING shall be
                        reported to the Default Error Tracer. */
		if( (CanTpPduData->SduLength < MAX_FRAME_BYTES) &&  \
				(CanTpRxNSdu[RxPduId].CanTpRxPaddingActivation == CANTP_ON) )
		{
#if DET_ERROR_STATUS == STD_ON
			Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
#endif
			PduR_CanTpRxIndication(cantp_com_rx_or_copy[RxPduId], E_NOT_OK);
		}
		else
		{
			// if((CanTpTxNSdu[RxPduId] != NULL_PTR))
			ReceiveSingleFrame(&CanTpRxNSdu[RxPduId], &CanTpRunTimeRxData[RxPduId], CanTpPduData);
		}
		break;

	case ISO15765_TPCI_FF: /* First Frame ******************************************************/
		ReceiveFirstFrame(&CanTpRxNSdu[RxPduId], &CanTpRunTimeRxData[RxPduId], CanTpPduData);
		break;

	case ISO15765_TPCI_CF: /* Consecutive Frame ************************************************/

		/* [SWS_CanTp_00346] If frames with a payload <= 8 (either CAN 2.0 frames or small CAN FD frames)
                              are used for a Rx N-SDU and CanTpRxPaddingActivation is equal to CANTP_ON,
                              and CanTp receives by means of CanTp_RxIndication() call a last CF Rx N-PDU
                              belonging to that NSDU, with a length smaller than eight bytes
                              (i.e. PduInfoPtr. SduLength != 8), CanTp shall abort the ongoing reception by
                              calling PduR_CanTpRxIndication() with the result E_NOT_OK. The runtime error
                              code CANTP_E_PADDING shall be reported to the Default Error Tracer */

		if ( (CanTpRunTimeRxData[RxPduId].nextFlowControlCount == 0)  && (CanTpRunTimeRxData[RxPduId].BS) ) /* Last Consecutive Frame */
		{
			if( (CanTpPduData->SduLength < MAX_FRAME_BYTES) &&  \
					(CanTpRxNSdu[RxPduId].CanTpRxPaddingActivation == CANTP_ON) )
			{
#if DET_ERROR_STATUS == STD_ON
				Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
#endif
				PduR_CanTpRxIndication(cantp_com_rx_or_copy[RxPduId], E_NOT_OK);
			}
			else /* At Last CF   */
			{
				ReceiveConsecutiveFrame(&CanTpRxNSdu[RxPduId], &CanTpRunTimeRxData[RxPduId], CanTpPduData);
			}
		}
		else /* Not Last CF*/
		{
			ReceiveConsecutiveFrame(&CanTpRxNSdu[RxPduId], &CanTpRunTimeRxData[RxPduId], CanTpPduData);
		}
		break;

	case ISO15765_TPCI_FC: /* Flow Control *****************************************************/

		/* [SWS_CanTp_00349] If CanTpTxPaddingActivation is equal to CANTP_ON for a Tx N-SDU,
                              and if a FC N-PDU is received for that Tx N-SDU on a ongoing
                              transmission, by means of CanTp_RxIndication() call, and the length
                              of this FC is smaller than eight bytes (i.e. PduInfoPtr.SduLength <8)
                              the CanTp module shall abort the transmission session by
                              calling PduR_CanTpTxConfirmation() with the */
		if( (CanTpPduData->SduLength < MAX_FRAME_BYTES) &&  \
				(CanTpTxNSdu[RxPduId].CanTpTxPaddingActivation == CANTP_ON) )
		{
#if DET_ERROR_STATUS == STD_ON
			Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
#endif
			PduR_CanTpTxConfirmation(cantp_com_rx_or_copy[RxPduId], E_NOT_OK);
		}
		else
		{
			/* Get the Frame Status for Flow Control */
			ReceiveFlowControlFrame(&CanTpTxNSdu[RxPduId], &CanTpRunTimeTxData[RxPduId], CanTpPduData);
		}
		break;
	default:
		break;
	}
}


/*****************************************************************************************************
 * Service name      : SendNextTxFrame                                                               *
 * Parameters (in)   : txConfig       -> Pointer to  CanTpTxNSdu_s container                         *
 *                     txRuntime      -> Pointer to RunTimeInfo_s container                          *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Get Data From Pdur to Cantp                                                   *
 *****************************************************************************************************/
static BufReq_ReturnType SendNextTxFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo_s *txRuntime)
{
	BufReq_ReturnType ret = BUFREQ_E_NOT_OK;
	PduInfoType pduInfo; /**/
	Std_ReturnType resp; /**/
	uint8 canif_id = canif_ids[txConfig->CanTpTxNSduId];/**/

	/* [SWS_CanTp_00272] The API PduR_CanTpCopyTxData() contains a parameter used for the recovery mechanism – ‘retry’.
	 * Because ISO 15765-2 does not support such a mechanism, the CAN Transport Layer does not implement
	 * any kind of recovery. Thus, the parameter is always set to NULL pointer. */
	RetryInfoType* retry = NULL_PTR;

	/* make SduDataPtr point to first byte of Payload data */
	//txRuntime->pdurBuffer.SduDataPtr = &CanTpRunTimeTxData[txConfig->CanTpTxNSduId].IfBuffer.IFdataPtr[CanTpRunTimeTxData[txConfig->CanTpTxNSduId].IfBuffer.IFByteCount++];

	if (txRuntime->availableDataSize == 0)           /* in case of SF or FF, no data left unsent */
	{
		ret                       = PduR_CanTpCopyTxData(cantp_com_copy_tx[txConfig->CanTpTxNSduId],&txRuntime->pdurBuffer,retry,&txRuntime->availableDataSize);
		txRuntime->transferCount += txRuntime->pdurBuffer.SduLength;
	}

	else /* In case of any CF */
	{

		/* Prepare TX buffer for next frame to be sent */
		txRuntime->IfBuffer.IFByteCount = 0;

		if (txConfig->CanTpTxAddressingFormat == CANTP_EXTENDED )   /* for Extended Addressing Mode */
		{
			txRuntime->IfBuffer.IFByteCount++;
		}

		/* Increment Number of frames handled in this specific block of Consecutive frames */
		txRuntime->framesHandledCount++;

		/* To prepare First byte in CF */
		txRuntime->IfBuffer.IFdataPtr[txRuntime->IfBuffer.IFByteCount++] = (txRuntime->framesHandledCount & SEGMENT_NUMBER_MASK) + ISO15765_TPCI_CF;

		/* Decrement nextFlowControlCount each time entering function indicating
		 * how much frames i need to send later until the state of waiting for FC Frame */
		COUNT_DECREMENT(txRuntime->nextFlowControlCount);

		/* Value of MAX_PAYLOAD of each frame differentiates depending on the addressing mode whether
		 * it's standard or extended Addressing modes */
		if ( (txRuntime->availableDataSize) > MAX_PAYLOAD_STANDRAD_CF)        /* in case of Full CF */
		{
			(txRuntime->pdurBuffer.SduLength) =  MAX_PAYLOAD_STANDRAD_CF;
		}
		else if ( (txRuntime->availableDataSize) <= MAX_PAYLOAD_STANDRAD_CF)  /* in case of Last CF */
		{
			(txRuntime->pdurBuffer.SduLength) = (txRuntime->availableDataSize);
		}

		ret                               = PduR_CanTpCopyTxData(cantp_com_copy_tx[txConfig->CanTpTxNSduId],&txRuntime->pdurBuffer,retry,&txRuntime->availableDataSize);

		/* Updating total transfered amount of bytes */
		txRuntime->transferCount         += txRuntime->pdurBuffer.SduLength;
	}

	/* make SduDataPtr point to first byte of Payload data */
	//txRuntime->pdurBuffer.SduDataPtr = txRuntime->IfBuffer.IFdataPtr[txRuntime->IfBuffer.IFByteCount];
	uint8 ByteCounter;
	for(ByteCounter=0;ByteCounter<txRuntime->pdurBuffer.SduLength;ByteCounter++)
	{
		txRuntime->IfBuffer.IFdataPtr[ByteCounter+txRuntime->IfBuffer.IFByteCount] = txRuntime->pdurBuffer.SduDataPtr[ByteCounter];
	}

	/* Updating total number of bytes to be transmitted to CanIF */
	txRuntime->IfBuffer.IFByteCount           += txRuntime->pdurBuffer.SduLength;


	if(BUFREQ_OK == ret) /* data copied successfully */
	{

		pduInfo.SduDataPtr = txRuntime->IfBuffer.IFdataPtr;
		pduInfo.SduLength  = txRuntime->IfBuffer.IFByteCount;

		if (txConfig->CanTpTxPaddingActivation == CANTP_ON)
		{
			PadFrame(&pduInfo);
		}
		else
		{
			/* Do Nothing in case of padding isn't activated */
		}

		/* [SWS_CanTp_00075] When the transmit confirmation is not received after a maximum time (equal to N_As),
		 * the CanTp module shall abort the corresponding session. The N-PDU remains unavailable
		 * to other concurrent sessions until the TxConfirmation is received, successful or not.*/

		/* change state to verify tx confirm within timeout */
		txRuntime->stateTimeoutCount = (txConfig->CanTpNas);
		txRuntime->state             =  TX_WAIT_TX_CONFIRMATION;

		/* Calling CanIf to transmit underlying frame */
		resp                         = CanIf_Transmit(canif_id, &pduInfo);

		switch(resp)
		{
		case E_NOT_OK:

			/* [SWS_CanTp_00343] CanTp shall terminate the current transmission connection when CanIf_Transmit()
			 * returns E_NOT_OK when transmitting an SF, FF, of CF */

			/* failed to send, returning value of BUFREQ_E_NOT_OK to caller */
			ret = BUFREQ_E_NOT_OK;

			break;

		case E_OK:

			/* Data sent successfully, PduR_CanTpTxConfirmation will be
			 * called in 'handleNextTxFrame' function */
			ret = BUFREQ_OK;
			break;

		default:
			break;
		}

	}
	else /* BUFREQ_E_NOT_OK or BUFREQ_E_BUSY */
	{
		/* failed to copy new data */
	}

	return ret;
}

/*****************************************************************************************************
 * Service name      : HandleNextTxFrame                                                             *
 * Parameters (in)   : txConfig       -> Pointer to  CanTpTxNSdu_s container                         *
 *                     txRuntime      -> Pointer to RunTimeInfo_s container                          *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Handle next Consective Frame To Transmit                                      *
 *****************************************************************************************************/
static void HandleNextTxFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo_s *txRuntime)
{

	/* In Case of Last Block of Consecutive Frames is Received (End of Transmission) */
	if ( (txRuntime->transferTotal) <= (txRuntime->transferCount) )
	{
		/* Transfer finished! */
		PduR_CanTpTxConfirmation(txConfig->CanTpTxNPduConfirmationPduId, E_OK);
		txRuntime->state = IDLE;
		txRuntime->mode  = CANTP_TX_WAIT;
	}

	/* In Case of the last CF in the block and sender is waiting for Flow Control */
	else if ( (txRuntime->nextFlowControlCount == 0) && (txRuntime->BS) )
	{
		/* [SWS_CanTp_00315] The CanTp module shall start a timeout observation for N_Bs time
		 * at confirmation of the FF transmission, last CF of a block transmission
		 * and at each indication of FC with FS=WT (i.e. time until reception of the next FC). */

		/* TX ECU expects flow control */
		txRuntime->stateTimeoutCount = (txConfig->CanTpNbs);
		txRuntime->state             =  TX_WAIT_FLOW_CONTROL;
	}
	/* In case of nextFlowControlCount != 0 (in-between CF) or BS == 0 (last block of CFs)
	 * and it's time to send that CF (STmin == 0) */
	else if (txRuntime->stateTimeoutCount == 0)  // it was STmin, i am not sure about this
	{
		/* Send next consecutive frame! */
		BufReq_ReturnType resp;

		/* [SWS_CanTp_00167] After a transmission request from upper layer,
		 * the CanTp module shall start time-out N_Cs before the call of PduR_CanTpCopyTxData.
		 * If no data is available before the timer elapsed, the CanTp module shall abort the communication.*/
		txRuntime->stateTimeoutCount = (txConfig->CanTpNcs);

		/* Sending Next frame using the function responsible for calling PduR_CanTpCopyTxData() function */
		resp = SendNextTxFrame(txConfig, txRuntime);

		switch(resp)
		{
		case BUFREQ_OK:
		{
			/* successfully sent frame, wait for TX confirm */

			/* [SWS_CanTp_00090] ⌈When the transport transmission session is successfully completed,
			 * the CanTp module shall call a notification service of the upper layer,
			 * PduR_CanTpTxConfirmation(), with the result E_OK. */
			PduR_CanTpTxConfirmation(txConfig->CanTpTxNPduConfirmationPduId, E_OK);

			break;
		}
		case BUFREQ_E_BUSY:
		{
			/* [SWS_CanTp_00184] If the PduR_CanTpCopyTxData() function returns BUFREQ_E_BUSY,
			 * the CanTp module shall later (implementation specific) retry to copy the data. */

			/* Change State and Setup Timeout trying to copy the data later if it's available
			 * before N_Cs timeout */
			txRuntime->stateTimeoutCount = (txConfig->CanTpNcs);
			txRuntime->state             =  TX_WAIT_TRANSMIT;

			break;
		}
		case BUFREQ_E_NOT_OK:
		{
			/* [SWS_CanTp_00087] If PduR_CanTpCopyTxData() returns BUFREQ_E_NOT_OK,
			 * the CanTp module shall abort the transmit request and notify the upper layer of this failure
			 * by calling the callback function PduR_CanTpTxConfirmation() with the result E_NOT_OK. */

			/* [SWS_CanTp_00343]⌈CanTp shall terminate the current transmission connection when CanIf_Transmit()
			 * returns E_NOT_OK when transmitting an SF, FF, of CF */

			PduR_CanTpTxConfirmation(txConfig->CanTpTxNPduConfirmationPduId, E_NOT_OK);
			txRuntime->state = IDLE;
			txRuntime->mode  = CANTP_TX_WAIT;

			break;

		}
		default:
		{
			break;
		}
		}

	}
	else
	{

		/* In case of nextFlowControlCount != 0 (in-between CF) or BS == 0 (last block of CFs)
		 * and it's not time to send (STmin != 0) */

		/* Send next consecutive frame after STmin!
		 * ST MIN error handling, please read ISO 15765-2 Sec7.6 for info about
		 * setting value of STmin */
		if ( (txRuntime->STmin) < 0x80 )
		{
			txRuntime->stateTimeoutCount = (txRuntime->STmin) + 1;
		}
		else if ( ((txRuntime->STmin) > 0xF0) && ((txRuntime->STmin) < 0xFA) )
		{
			/* 0.1 mS resolution needs a lower task period. So hard coded to 1 main cycle */
			txRuntime->stateTimeoutCount = 1;
		}
		else
		{
			txRuntime->stateTimeoutCount = (0x7F) + 1;
		}
		/* Changing state of TX to be waiting for STmin Time stored in stateTimeoutCount */
		txRuntime->state = TX_WAIT_STMIN;
	}
}

/*****************************************************************************************************
 * Service name      : ReceiveFlowControlFrame                                                       *
 * Parameters (in)   : txConfig       -> Pointer to  CanTpRxNSdu_s container                         *
 *                     txRuntime      -> Pointer to RunTimeInfo_s container                          *
 *                     PduData        -> Pointer to PduInfoType Container                            *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Receive Flow Control Frame                                                    *
 *****************************************************************************************************/
static void ReceiveFlowControlFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo_s *txRuntime, const PduInfoType *PduData)
{
	uint8 indexCount = 0;

	/* Make sure that the frame is in 'TX_WAIT_FLOW_CONTROL' state */
	if ( txRuntime->state == TX_WAIT_FLOW_CONTROL )
	{
		if (txConfig->CanTpTxAddressingFormat == CANTP_EXTENDED)   /* for Extended Addressing Mode */
		{
			indexCount++;
		}
		/* First Byte of FC Frame consists of (frame type + flow control flag)
		 * Stored in PduData->SduDataPtr[0]
		 * Checking the Flow Control Flag Status(ClearToSend=0, Wait=1, OverFlow=2)*/
		switch (PduData->SduDataPtr[indexCount++] & ISO15765_TPCI_FS_MASK)
		{
		case ISO15765_FLOW_CONTROL_STATUS_CTS:
		{
			/* Extracting BlockSize(BS) which is Stored in PduData->SduDataPtr[1] */
			txRuntime->BS                   = PduData->SduDataPtr[indexCount];
			/* Storing value of BlockSize in Counter that shall be decremented at the reception of each CF */
			txRuntime->nextFlowControlCount = PduData->SduDataPtr[indexCount++];
			/* Extracting SeparateTime(STmin) which is Stored in PduData->SduDataPtr[2] */
			txRuntime->STmin                = PduData->SduDataPtr[indexCount++];
			/* Changing state of the frame to be waiting for Next CF Transmission */
			txRuntime->state                = TX_WAIT_TRANSMIT;

			break;
		}

		case ISO15765_FLOW_CONTROL_STATUS_WAIT:
		{
			/* [SWS_CanTp_00315] The CanTp module shall start a timeout observation for N_Bs time at
			 * confirmation of the FF transmission, last CF of a block transmission and at
			 * each indication of FC with FS=WT (i.e. time until reception of the next FC).*/

			/* Starting N_BS Timer */
			txRuntime->stateTimeoutCount    = (txConfig->CanTpNbs);
			/* Frame state remains unchanged */
			txRuntime->state                = TX_WAIT_FLOW_CONTROL;

			break;
		}

		case ISO15765_FLOW_CONTROL_STATUS_OVFLW:
		{
			/* [SWS_CanTp_00309] If a FC frame is received with the FS set to OVFLW the CanTp module shall
			 * abort the transmit request and notify the upper layer by calling the callback
			 * function PduR_CanTpTxConfirmation() with the result E_NOT_OK.*/

			/* Abort the transmission requested before by setting Frame State to be in idle state
			 * and Changing CanTP Module Mode to be in 'CANTP_TX_WAIT' mode */
			txRuntime->state                = IDLE;
			txRuntime->mode                 = CANTP_TX_WAIT;

			/* PduR Notification of Transmission failure by the value 'E_NOT_OK' */
			PduR_CanTpTxConfirmation(txConfig->CanTpRxFcNPduId, E_NOT_OK);

			break;

		}
		default:
		{
			/* In case the FS of FC is invalid, exit */
			break;
		}

		}
	}

	else
	{
		/* Do nothing if the frame isn't in 'TX_WAIT_FLOW_CONTROL' state keeping state and mode unchanged */
	}

}


/*****************************************************************************************************
 * Service name      : ReceiveConsecutiveFrame                                                       *
 * Parameters (in)   : rxConfig       -> Pointer to  CanTpRxNSdu_s container                         *
 *                     rxRuntime      -> Pointer to RunTimeInfo_s container                          *
 *                     rxPduData      -> Pointer to PduInfoType Container                            *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Copy Cantp Received Data to Pdur  for Consective Frame                        *
 *****************************************************************************************************/
static void ReceiveConsecutiveFrame(const CanTpRxNSdu_s *rxConfig, RunTimeInfo_s *rxRuntime, const PduInfoType *rxPduData)
{
	uint8 indexCount                        = 0;
	uint8 segmentNumber                     = 0;
	//PduLengthType currentSegmentSize        = 0;
	//PduLengthType bytesCopiedToPdurRxBuffer = 0;
	BufReq_ReturnType ret                   = BUFREQ_E_NOT_OK;

	PduInfoType info;
	uint8 id          = cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId] ;

	//TODO: if(mode == CanTp_Rx_PROCESSING) ---> FF sets the mode to be processing,
	//so we have to ensure we are entering in the right mode...
	if (rxRuntime->state == RX_WAIT_CONSECUTIVE_FRAME)
	{

		if (rxConfig->CanTpRxAddressingFormat == CANTP_EXTENDED)   /* for Extended Addressing Mode */
		{
			indexCount++;
		}
		else
		{
			/* Nothing */
		}
		/* [SWS_CanTp_00284] In the reception direction, the first data byte value of each (SF, FF or CF)
		 * transport protocol data unit will be used to determine the relevant N-SDU.*/

		/* Extracting CF SegmentNumber(SN) from the low nibble of Byte 0 */
		segmentNumber = rxPduData->SduDataPtr[indexCount++] & SEGMENT_NUMBER_MASK;

		/* Checking if this consecutive frame is handled or not */
		if (segmentNumber != (rxRuntime->framesHandledCount & SEGMENT_NUMBER_MASK))
		{
			/*[SWS_CanTp_00314] The CanTp shall check the correctness of each SN received
			 * during a segmented reception. In case of wrong SN received the CanTp module
			 * shall abort reception and notify the upper layer of this failure by calling
			 * the indication function PduR_CanTpRxIndication() with the result E_NOT_OK.*/

			/* Abort Reception by setting frame state to IDLE and canTP mode to CANTP_RX_WAIT */
			rxRuntime->state = IDLE;
			rxRuntime->mode  = CANTP_RX_WAIT;

			/* notify PduR of this failure by calling the indication function with the result E_NOT_OK */
			PduR_CanTpRxIndication(rxConfig->CanTpRxNPduId, E_NOT_OK);

		}

		else /* Correct SN received */
		{

			/* [SWS_CanTp_00269] ⌈After reception of each Consecutive Frame the CanTp module shall call the PduR_CanTpCopyRxData()
			 * function with a PduInfo pointer containing data buffer and data length:
			 *  - 6 or 7 bytes or less in case of the last CF for CAN 2.0 frames
			 *  - DLC-1 or DLC-2 bytes for CAN FD frames (see Figure 5 and SWS_CanTp_00351).
			 * The output pointer parameter provides CanTp with available Rx buffer size after data have been copied.*/

			/* Move SduDataPtr to be pointing to Byte#1 in received L-Pdu which is the start of payload data */
			info.SduDataPtr =  &(rxPduData->SduDataPtr[indexCount]);

			/* Update SduLength by the value of 'L-PduLength(containing PCI) - 1' */
			info.SduLength  = (rxPduData->SduLength) - 1;

			COUNT_DECREMENT(rxRuntime->nextFlowControlCount);

			/* Checking that it's the last CF or not. If the condition is true, it means that it's a last CF */
			if ( (rxRuntime->nextFlowControlCount == 0)  && (rxRuntime->BS) )
			{
				/* [SWS_CanTp_00166] At the reception of a FF or last CF of a block, the CanTp module shall
				 *  start a time-out N_Br before calling PduR_CanTpStartOfReception or PduR_CanTpCopyRxData.*/

				/* start N_Br timer */
				rxRuntime->stateTimeoutCount = (rxConfig->CanTpNbr);

				/* copy data to PduR Buffer */
				ret = PduR_CanTpCopyRxData(id,&info,&rxRuntime->Buffersize);

				/* Incoming frame is copied successfully then we should increment number of handled frames */
				rxRuntime->framesHandledCount++;

				/* Checking that we are handling the last CF of the last Block or just in-between block */
				if(rxRuntime->Buffersize == 0) // it was buffersize ==0 !!//TODO: handle ending frame condition right
				{
					/* reception session is completed*/

					/* [SWS_CanTp_00084] When the transport reception session is completed (successfully or not)
					 * the CanTp module shall call the upper layer notification service PduR_CanTpRxIndication().*/
					PduR_CanTpRxIndication(id, ret);

					/* Setting frame state to 'IDLE' and CanTP mode to 'CANTP_RX_WAIT' */
					rxRuntime->state = IDLE;
					rxRuntime->mode  = CANTP_RX_WAIT;
				}
				else if(BUFREQ_OK == ret) /* It's last CF of this block */
				{
					/* Now, i need to send a FC Frame with returned buffer status stored in ret
					 * and available buffer size stored in 'rxRuntime' */

					//TODO: check BS size f next block VS Buffer size available of pduR
					//if no buffer available for next block --> RX_WAIT_SDU_BUFFER, start N_Br and CANTP_RX_PROCESSING

					rxRuntime->state = RX_WAIT_SDU_BUFFER;
					rxRuntime->mode  = CANTP_RX_PROCESSING;
				}
				else /* ret == BUFREQ_E_NOT_OK */
				{
					/* in case of failing to copy frame, return nextFlowControlCount to its value before calling
					 * PduR_CanTpCopyRxData */
					rxRuntime->nextFlowControlCount++;

					/* [SWS_CanTp_00271] If the PduR_CanTpCopyRxData() returns BUFREQ_E_NOT_OK after reception of
					 * a Consecutive Frame in a block the CanTp shall abort the reception of N-SDU and notify
					 * the PduR module by calling the PduR_CanTpRxIndication() with the result E_NOT_OK. */

					rxRuntime->state = IDLE;
					rxRuntime->mode  = CANTP_RX_WAIT;
					PduR_CanTpRxIndication(id, ret);
				}

			}

			else /* It's not last CF which means it's in-between */
			{
				/* Copy the data in the buffer as long as there`s a room for copying
				 * and then checking the returned buffer status and available buffer size */
				ret = PduR_CanTpCopyRxData(id,&info,&rxRuntime->Buffersize); //

				if (ret == BUFREQ_E_NOT_OK)
				{

					/* [SWS_CanTp_00271] If the PduR_CanTpCopyRxData() returns BUFREQ_E_NOT_OK after reception of
					 * a Consecutive Frame in a block the CanTp shall abort the reception of N-SDU and notify
					 * the PduR module by calling the PduR_CanTpRxIndication() with the result E_NOT_OK. */

					PduR_CanTpRxIndication(id, ret);
					rxRuntime->state = IDLE;
					rxRuntime->mode  = CANTP_RX_WAIT;
				}

				else if (ret == BUFREQ_OK)
				{
					/* Current CF is handled and counter shall be incremented */
					rxRuntime->framesHandledCount++;

					/* [SWS_CanTp_00312] The CanTp module shall start a time-out N_Cr at each indication of CF reception
					 * (except the last one in a block) and at each confirmation of a FC transmission that initiate
					 * a CF transmission on the sender side (FC with FS=CTS).*/

					/* Start N_Cr timer which contains the time in seconds until reception of the next Consecutive Frame N_PDU.*/
					rxRuntime->stateTimeoutCount = (rxConfig->CanTpNcr);
				}/* end of returned PduR Buffer status checking*/

			}/* end of last CF checking */

		} /* end of SN checking */

	}/* end of state checking */
	else
	{
		/* Do Nothing if the frame is not in 'TX_WAIT_FLOW_CONTROL' state and nothing should be changed */
	}
}


/*****************************************************************************************************
 * Service name      : ReceiveSingleFrame                                                            *
 * Parameters (in)   : rxNSduConfig   -> Pointer to  CanTpRxNSdu container                           *
 *                     rxRuntimeParam -> Pointer to RunTimeInfo container                            *
 *                     CanTpPduData   -> Pointer to PduInfoType Container                            *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Copy Cantp Received Data to Pdur for Single Frame                             *
 *****************************************************************************************************/
static void ReceiveSingleFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, const PduInfoType *CanTpPduData)
{
	PduInfoType rxPduData;  /* Pdur Receive Data */
	uint8 CanTpPduData_Offset ;
	uint8 CanTpPduDataLength_Offset;
	BufReq_ReturnType  BufferRequest_Status = BUFREQ_E_NOT_OK ;
	BufReq_ReturnType  CopyData_Status      = BUFREQ_E_NOT_OK ;

	/* Idle = CANTP_ON.CANTP_RX_WAIT and CANTP_ON.CANTP_TX_WAIT */
	/* CanTp status = Idle so   Process the SF N-PDU as the start of a new reception */
	/* [SWS_CanTp_00124] When an SF or FF N-PDU without MetaData is received, and
                        the corresponding connection channel is currently receiving the same connection
                        (state CANTP_RX_PROCESSING, same N_AI), the CanTp module shall abort the
                        reception in progress and shall process the received frame as the start of a new reception */
	/* [SWS_CanTp_00290]  CanTp status = Segmented Receive in progress
	 *                    Terminate the current reception, report an indication, with parameter Result set to E_NOT_OK,
                          to the upper layer, and process the SF N-PDU as the start of a new reception */
	if(rxRuntimeParam->mode == CANTP_RX_PROCESSING)
	{
		/* Report an PDUR with E_NOT_OK to Abort*/
		PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);

		/* Start new Reception */
		StartNewReception(rxRuntimeParam);
	}
	else
	{
		/* Do Nothing */
	}

	if ( rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD )  /* Single Frame -> [ 1Byte (PCI) + 7Bytes (Data) ] */
	{//TODO: CanTP_Standard OFFSET
		CanTpPduData_Offset       = 1;
		CanTpPduDataLength_Offset = 0;
	}
	else if ( rxNSduConfig->CanTpRxAddressingFormat == CANTP_EXTENDED )
	{
		CanTpPduData_Offset       = 2;
		CanTpPduDataLength_Offset = 1;
	}
	else
	{
		/* Do Nothing */
	}

	/* Pdu Recieved Data */
	rxPduData.SduDataPtr          = &CanTpPduData->SduDataPtr[CanTpPduData_Offset] ;

	/* Total length of the N-SDU Data to be received. */
	rxRuntimeParam->transferTotal = CanTpPduData->SduDataPtr[CanTpPduDataLength_Offset] & ISO15765_TPCI_DL;

	/* Processing Mode*/
	rxRuntimeParam->mode = CANTP_RX_PROCESSING;

	/* Change State To SF  wait Pdur Buffer */
	rxRuntimeParam->state = SF_OR_FF_RECEIVED_WAITING_PDUR_BUFFER;


	/* [SWS_CanTp_00079] When receiving an SF or an FF N-PDU, the CanTp module
             shall notify the upper layer (PDU Router) about this reception using the
             PduR_CanTpStartOfReception function */
	/* TODO Change ID */
	BufferRequest_Status = PduR_CanTpStartOfReception( cantp_com_rx_or_copy[rxNSduConfig->CanTpRxNSduId], \
			&rxPduData,  \
			rxRuntimeParam->transferTotal,  \
			&rxRuntimeParam->Buffersize );

	switch(BufferRequest_Status)
	{

	/* [SWS_CanTp_00339] After the reception of a First Frame or Single Frame, if the
                         function PduR_CanTpStartOfReception() returns BUFREQ_OK with a smaller
                         available buffer size than needed for the already received data, the CanTp module
                         shall abort the reception of the N-SDU and call PduR_CanTpRxIndication() with
                         the result E_NOT_OK */
	case BUFREQ_OK:
		if( rxRuntimeParam->Buffersize < rxRuntimeParam->transferTotal )
		{
			PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);
		}
		else
		{
			/* TODO Change ID */
			/* Id           ->  dent
ication of the received I-PDU.
			 * info         -> Provides the source buffer (SduDataPtr) and the number of
                               bytes to be copied
			 *bufferSizePtr -> Available receive buffer after data has been copied.
			 */
			CopyData_Status = PduR_CanTpCopyRxData( cantp_com_rx_or_copy[rxNSduConfig->CanTpRxNSduId],\
					&rxPduData,  \
					&rxRuntimeParam->Buffersize );
			switch(CopyData_Status)
			{
			case BUFREQ_OK :
				PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_OK);
				break;
			case BUFREQ_E_NOT_OK :
				PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);
				break;
			default :
				/* Do Nothing*/
				break;
			}
		}
		/* End of the reception*/
		rxRuntimeParam->state = IDLE;
		rxRuntimeParam->mode = CANTP_RX_WAIT;
		break;

		/*[SWS_CanTp_00081] After the reception of a First Frame or Single Frame, if the function
                            PduR_CanTpStartOfReception()returns BUFREQ_E_NOT_OK to the CanTp module,
                            the CanTp module shall abort the reception of this N-SDU. No Flow
                            Control will be sent and PduR_CanTpRxIndication() will not be called in this case. */

		/* [SWS_CanTp_00353] After the reception of a Single Frame, if the function
                            PduR_CanTpStartOfReception()returns BUFREQ_E_OVFL to the CanTp module,
                            the CanTp module shall abort the N-SDU reception.*/

	case BUFREQ_E_NOT_OK :
		/* abort the reception*/
	case BUFREQ_E_OVFL :
		/* abort the reception*/
		rxRuntimeParam->state = IDLE;
		rxRuntimeParam->mode  = CANTP_RX_WAIT;
		break;
	default:
		/* Do Nothing */
		break;
	}

}

/*****************************************************************************************************
 * Service name      : ReceiveFirstFrame                                                             *
 * Parameters (in)   : rxNSduConfig   -> Pointer to  CanTpRxNSdu container                           *
 *                     rxRuntimeParam -> Pointer to RunTimeInfo container                            *
 *                     CanTpPduData   -> Pointer to PduInfoType Container                            *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Copy Cantp Received Data to Pdur  for First Frame                             *
 *****************************************************************************************************/
static void ReceiveFirstFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, const PduInfoType *CanTpPduData)
{
	PduInfoType rxPduData;  /* Pdur Receive Data */
	uint8 CanTpPduData_Offset ;
	uint8 CanTpPduDataLength_Offset;
	BufReq_ReturnType  BufferRequest_Status ;
	BufReq_ReturnType  CopyData_Status      = BUFREQ_E_NOT_OK ;

	/* Status -> Segmented Receive in progress
                 Terminate the current reception,report an indication, with parameter Result set to
                 E_NOT_OK, to the upper layer, and process the FF N-PDU as the start of a new reception
       Status -> IDle
                Process the SF N-PDU as the start of a new reception */
	if(rxRuntimeParam->mode == CANTP_RX_PROCESSING)
	{
		/* Report an PDUR with E_NOT_OK to Abort*/
		PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);

		/* Start new Reception */
		StartNewReception(rxRuntimeParam);
	}
	else
	{
		/* Do Nothing */
	}

	if (rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD) /* First Frame -> [ 2Byte (PCI) + 6Bytes (Data) ] */
	{
		CanTpPduData_Offset       = 2;
		CanTpPduDataLength_Offset = 0;
	}
	else
	{
		/* Nothing */
	}

	/* Pdu Recieved Data */
	rxPduData.SduDataPtr          = &CanTpPduData->SduDataPtr[CanTpPduData_Offset] ;

	rxPduData.SduLength          = CanTpPduData->SduLength - CanTpPduData_Offset;

	/* Get total Length FF_DL */
	rxRuntimeParam->transferTotal = ( (CanTpPduData->SduDataPtr[CanTpPduDataLength_Offset] & ISO15765_TPCI_DLFF ) << 8 ) + CanTpPduData->SduDataPtr[CanTpPduDataLength_Offset + 1] ;

	/* Processing Mode*/
	rxRuntimeParam->mode = CANTP_RX_PROCESSING;

	/* Change State To SF  wait Pdur Buffer */
	rxRuntimeParam->state = SF_OR_FF_RECEIVED_WAITING_PDUR_BUFFER;


	/* [SWS_CanTp_00079] When receiving an SF or an FF N-PDU, the CanTp module shall notify
                         the upper layer (PDU Router) about this reception using the
                         PduR_CanTpStartOfReception function */

	/* [SWS_CanTp_00329]  CanTp shall provide the content of the FF/SF to PduR using
                         the parameter TpSduInfoPtr of PduR_CanTpStartOfReception()*/

	/* [SWS_CanTp_00064] Furthermore, it should be noted that when receiving a FF NPDU,
	 *                   the Flow Control shall only be sent after having the result of the
                         PduR_CanTpStartOfReception() service. */

	/*[SWS_CanTp_00166] At the reception of a FF or last CF of a block, the CanTp
                        module shall start a time-out N_Br before calling PduR_CanTpStartOfReception
                        or PduR_CanTpCopyRxData*/

	rxRuntimeParam->stateTimeoutCount = (rxNSduConfig->CanTpNbr);

	BufferRequest_Status = PduR_CanTpStartOfReception( cantp_com_rx_or_copy[rxNSduConfig->CanTpRxNSduId], \
			&rxPduData,  \
			rxRuntimeParam->transferTotal,  \
			&rxRuntimeParam->Buffersize );


	switch(BufferRequest_Status)
	{
	case BUFREQ_OK :

		/* [SWS_CanTp_00339] After the reception of a First Frame or Single Frame, if the
                             function PduR_CanTpStartOfReception() returns BUFREQ_OK with a smaller
                             available buffer size than needed for the already received data, the CanTp module
                             shall abort the reception of the N-SDU and call PduR_CanTpRxIndication() with
                             the result E_NOT_OK */
		if( rxRuntimeParam->Buffersize < rxPduData.SduLength ) //sdulength
		{
			/* call PduR_CanTpRxIndication() with the result E_NOT_OK */
			PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);
			/* abort the reception*/
			rxRuntimeParam->state = IDLE;
			rxRuntimeParam->mode = CANTP_RX_WAIT;

		}
		else
		{
			/*[SWS_CanTp_00224] When the Rx buffer is large enough for the next block
                               (directly after the First Frame or the last Consecutive Frame of a block, or after
                               repeated calls to PduR_CanTpCopyRxData() according to SWS_CanTp_00222),
                               the CanTp module shall send a Flow Control N-PDU with ClearToSend status
                               (FC(CTS)) and shall then expect the reception of Consecutive Frame N-PDUs.*/

			/* TODO Change ID */
			/* Id           ->  dentification of the received I-PDU.
			 * info         -> Provides the source buffer (SduDataPtr) and the number of
                               bytes to be copied
			 *bufferSizePtr -> Available receive buffer after data has been copied.
			 */
			CopyData_Status = PduR_CanTpCopyRxData( cantp_com_rx_or_copy[rxNSduConfig->CanTpRxNSduId],\
					&rxPduData,  \
					&rxRuntimeParam->Buffersize );

			switch(CopyData_Status)
			{
			case BUFREQ_OK: /* Data copied successfully */

				/* [SWS_CanTp_00082] After the reception of a First Frame, if the function
                                     PduR_CanTpStartOfReception() returns BUFREQ_OK with a smaller available buffer
                                     size than needed for the next block, the CanTp module shall start the timer N_Br.*/
				if( rxRuntimeParam->Buffersize < ((rxRuntimeParam->transferTotal /7)*7) )
				{
					/* [SWS_CanTp_00222] While the timer N_Br is active, the CanTp module shall call
                                         the service PduR_CanTpCopyRxData() with a data length 0 (zero) and NULL_PTR
                                         as data buffer during each processing of the MainFunction */
					/* Set N_BR timer */
					rxRuntimeParam->stateTimeoutCount = (rxNSduConfig->CanTpNbr) ;

					/* main function will check this state to handle if the buffer size
                      become available or not and if the timer is expired send wait flow control */
					rxRuntimeParam->state             = RX_WAIT_SDU_BUFFER       ;

					/* Receiver in progressing mode */
					rxRuntimeParam->mode              = CANTP_RX_PROCESSING      ;
				}
				else
				{
					PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_OK);

					/* Change State To wait Consecutive Frame */
					rxRuntimeParam->state = RX_WAIT_CONSECUTIVE_FRAME;

					/* Processing Received mode */
					rxRuntimeParam->mode       = CANTP_RX_PROCESSING;
					/* Count on First CF = 1 */
					rxRuntimeParam->framesHandledCount = 1;

					/* send a Flow Control N-PDU with ClearToSend status  */
					SendFlowControlFrame( rxNSduConfig, rxRuntimeParam, FLOW_CONTROL_CTS_FRAME);
				}
				break;
			case BUFREQ_E_NOT_OK:
				PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);
				break;
			default:
				/* Nothing */
				break;
			}
		}
		break;

		/*[SWS_CanTp_00081] After the reception of a First Frame or Single Frame, if the function
                            PduR_CanTpStartOfReception()returns BUFREQ_E_NOT_OK to the CanTp module,
                            the CanTp module shall abort the reception of this N-SDU. No Flow
                            Control will be sent and PduR_CanTpRxIndication() will not be called in this case. */
	case BUFREQ_E_NOT_OK :

		/* abort the reception*/
		rxRuntimeParam->state = IDLE;
		rxRuntimeParam->mode  = CANTP_RX_WAIT;
		break;

		/*[SWS_CanTp_00318]  After the reception of a First Frame, if the function
                             PduR_CanTpStartOfReception()returns BUFREQ_E_OVFL to the CanTp module,
                             the CanTp module shall send a Flow Control N-PDU with overflow status
                             (FC(OVFLW)) and abort the N-SDU reception.*/
		/* No buffer of the required length can be
             provided; reception is aborted. bufferSizePtr */
		/* [SWS_CanTp_00353] After the reception of a Single Frame, if the function
                                PduR_CanTpStartOfReception()returns BUFREQ_E_OVFL to the CanTp module,
                                the CanTp module shall abort the N-SDU reception.*/
	case BUFREQ_E_OVFL :
		/* send a Flow Control N-PDU with overflow status */
		SendFlowControlFrame( rxNSduConfig, rxRuntimeParam, FLOW_CONTROL_OVERFLOW_FRAME );

		/* abort the reception*/
		rxRuntimeParam->state = IDLE;
		rxRuntimeParam->mode  = CANTP_RX_WAIT;
		break;

	}

}


/*****************************************************************************************************
 * Service name      : SendFlowControlFrame                                                          *
 * Parameters (in)   : rxNSduConfig       -> Pointer to  CanTpRxNSdu container                       *
 *                     rxRuntimeParam     -> Pointer to RunTimeInfo container                        *
 *                     flowControlStatus  -> Flow Control Status                                     *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Receiver Send Flow Control                                                    *
 *****************************************************************************************************/
static void SendFlowControlFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, FrameType FlowControlStatus)
{
	BufReq_ReturnType  TransmitReqCanIf_Status = BUFREQ_E_NOT_OK ;
	PduInfoType txFlowControlData;
	uint8 Local_NSduData[8] = {0};
	uint8 Local_IndexCount = 0;
	uint8 Local_BSValue;

	txFlowControlData.SduDataPtr = &Local_NSduData[Local_IndexCount] ;

	/*************************** Cases when FlowControl Status be CTS *******************/
	/* 1- [SWS_CanTp_00224] When the Rx buffer is large enough for the next block
                                (directly after the First Frame or the last Consecutive Frame of a block,
                                 or after repeated calls to PduR_CanTpCopyRxData() according to SWS_CanTp_00222),
                                 the CanTp module shall send a Flow Control N-PDU with ClearToSend status
                                 (FC(CTS)) and shall then expect the reception of Consecutive Frame N-PDUs.*/
	/* 2- [SWS_CanTp_00312] The CanTp module shall start a time-out N_Cr at each indication of CF reception
                            (except the last one in a block) and at each confirmation of a FC transmission
                            that initiate a CF transmission on the sender side (FC with FS=CTS)*/

	/************************ cases when flowControl status is wait ****************/
	/* 1- [SWS_CanTp_00341] If the N_Br timer expires and the available buffer size is still
                            not big enough, the CanTp module shall send a new FC(WAIT) to suspend
                            the NSDU reception and reload the N_Br timer. */

	/************************ cases when flowControl status is Overflow ****************/
	/* 1- [SWS_CanTp_00318] After the reception of a First Frame, if the function PduR_CanTpStartOfReception()r
                         returns BUFREQ_E_OVFL to the CanTp module, the CanTp module shall send
                         a Flow Control N-PDU with overflow status (FC(OVFLW)) and abort the N-SDU reception */
	/* 2- [SWS_CanTp_00309] If a FC frame is received with the FS set to OVFLW the CanTp module shall abort
                            the transmit request and notify the upper layer by calling the callback function
                            PduR_CanTpTxConfirmation() with the result E_NOT_OK */
	switch (FlowControlStatus)
	{

	case FLOW_CONTROL_CTS_FRAME :
		/* Continue To Send (CTS) -> it cause the sender to resume the sending of Consecutive frame,
                                     it means that the receiver is ready to receive
                                     a maximum of BS number of Consecutive frame */

		/* PCI   ID->[0x30] && FS =0 */
		txFlowControlData.SduDataPtr[Local_IndexCount++] = ISO15765_TPCI_FC ;

		/* According to Pdur sws the paramater out(bufferSizePtr) in PduR_CanTpStartOfReception function
		 *                          ->  (rxRuntimeParam->Buffersize)
                                     This parameter will be used to compute the Block Size (BS) in the
                                     transport protocol module */
		if ( rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD ) /* Cantp is standard  addressing format */
		{
			Local_BSValue = ( rxRuntimeParam->Buffersize / MAX_PAYLOAD_STANDRAD_CF ) ;

			/*
            if( (rxRuntimeParam->Buffersize % MAX_PAYLOAD_STANDRAD_CF) )
            {
                Local_BSValue += 1;
            }
            else
            {
                 Do nothing
            }
			 */
		}
		else
		{
			/* Do nothing */
		}
		/* rxNSduConfig->CanTpBs :-
                              Sets the number of N-PDUs the CanTp receiver allows the sender to send,
                              before waiting for an authorization to continue transmission
                              of the following N-PDUs */
		if( Local_BSValue > rxNSduConfig->CanTpBs )
		{
			Local_BSValue = rxNSduConfig->CanTpBs ;
		}
		rxRuntimeParam->BS                               = Local_BSValue ;
		rxRuntimeParam->nextFlowControlCount             = Local_BSValue ;

		txFlowControlData.SduDataPtr[Local_IndexCount++] = rxRuntimeParam->BS    ; /* Block Size */
		txFlowControlData.SduDataPtr[Local_IndexCount++] = rxNSduConfig->CanTpSTmin ;

		txFlowControlData.SduLength                      = Local_IndexCount;
		break;

	case FLOW_CONTROL_WAIT_FRAME:

		/*[SWS_CanTp_00315] The CanTp module shall start a timeout observation for N_Bs time
                             at confirmation of the FF transmission, last CF of a block transmission and
                             at each indication of FC with FS=WT (i.e. time until reception of the next FC) */
		// rxRuntimeParam->stateTimeoutCount                = (rxNSduConfig->CanTpNbs);
		txFlowControlData.SduDataPtr[Local_IndexCount++] = ISO15765_TPCI_FC | ISO15765_FLOW_CONTROL_STATUS_WAIT;
		txFlowControlData.SduLength                      = Local_IndexCount + 2;

		rxRuntimeParam->state                            = RX_WAIT_SDU_BUFFER  ;

		break;

	case FLOW_CONTROL_OVERFLOW_FRAME:
		/* Overflow (OVFLW) -> it cause  the sender to abort the transmission of a segmented
                               message */
		txFlowControlData.SduDataPtr[Local_IndexCount++] = ISO15765_TPCI_FC | ISO15765_FLOW_CONTROL_STATUS_OVFLW;
		txFlowControlData.SduLength                      = Local_IndexCount + 2;
		break;
	}

	/*TODO ADD Macro For CanTpPaddingByte */
	/*[SWS_CanTp_00347]  If CanTpRxPaddingActivation is equal to CANTP_ON for  an Rx N-SDU,
                         the CanTp module shall transmit FC N-PDUs with a length of eight
                         bytes. Unused bytes in N-PDU shall be updated with CANTP_PADDING_BYTE*/
	if ( rxNSduConfig->CanTpRxPaddingActivation == CANTP_ON )
	{
		PadFrame( &txFlowControlData);
	}
	else
	{
		/* Do Nothing */
	}

	rxRuntimeParam->stateTimeoutCount = rxNSduConfig->CanTpNar;
	rxRuntimeParam->state             = RX_WAIT_TX_CONFIRMATION;

	/* [SWS_CanTp_00343]  CanTp shall terminate the current transmission connection
                          when CanIf_Transmit() returns E_NOT_OK when transmitting an SF, FF, of CF */

	CanTpRunTimeRxData[rxNSduConfig->CanTpRxNSduId].IfBuffer.IFdataPtr = txFlowControlData.SduDataPtr;
	CanTpRunTimeRxData[rxNSduConfig->CanTpRxNSduId].IfBuffer.IFByteCount = txFlowControlData.SduLength;

	TransmitReqCanIf_Status = CanIf_Transmit(cantp_cainf_FC[rxNSduConfig->CanTpTxFcNPduConfirmationPduId], &txFlowControlData);
	switch(TransmitReqCanIf_Status)
	{
	case E_OK :
		/* [SWS_CanTp_00090] When the transport transmission session is successfully
                             completed, the CanTp module shall call a notification service of the upper layer,
                             PduR_CanTpTxConfirmation(), with the result E_OK.*/
		PduR_CanTpTxConfirmation(cantp_cainf_FC[rxNSduConfig->CanTpRxNSduId] ,E_OK );
		break;
	case E_NOT_OK :
		/* terminate the current transmission connection */
		/* Note: The Api PduR_CanTpTxConfirmation() shall be called after a transmit
                 cancellation with value E_NOT_OK */
		/* TODO Change ID */
		PduR_CanTpTxConfirmation(cantp_cainf_FC[rxNSduConfig->CanTpRxNSduId] ,E_NOT_OK );
		break;
	}
}

/***************************************************************************************************
 * Service name      : PadFrame                                                                    *
 * Parameters (in)   : PduInfoPtr -> Pointer To PduInfoType Container                              *
 * Parameters (inout): None                                                                        *
 * Parameters (out)  : None                                                                        *
 * Return value      : None                                                                        *
 * Description       : Used for the initialization of unused bytes with a certain value            *
 ***************************************************************************************************/
static void PadFrame(PduInfoType *PduInfoPtr)
{
	uint8 i = 0;
	for (i = PduInfoPtr->SduLength; i < MAX_SEGMENT_DATA_SIZE; i++)
	{
		PduInfoPtr->SduDataPtr[i] = CANTP_PADDING_BYTE;
	}
	PduInfoPtr->SduLength = MAX_SEGMENT_DATA_SIZE;
}


/**************************************************************************************************
 * Service name      : StartNewReception                                                          *
 * Parameters (in)   : rxRuntimeParam -> Pointer to the container RunTimeInfo_s                   *
 * Parameters (inout): None                                                                       *
 * Parameters (out)  : None                                                                       *
 * Return value      : None                                                                       *
 * Description       : Used to initialize the Runtime value that will used in Receiption          *
 **************************************************************************************************/
static void StartNewReception(RunTimeInfo_s *rxRuntimeParam)
{
	/*[SWS_CanTp_00030] The function CanTp_Init shall initialize all global variables of the module and sets
	 * all transport protocol connections in a sub-state of CANTP_ON, in which neither segmented transmission
	 * nor segmented reception are in progress (Rx thread in state CANTP_RX_WAIT and Tx thread in state CANTP_TX_WAIT). (SRS_Can_01075)*/

	rxRuntimeParam->state                 = IDLE          ;
	rxRuntimeParam->mode                  = CANTP_RX_WAIT ;
	rxRuntimeParam->transferTotal         = 0;
	rxRuntimeParam->Buffersize            = 0;
	rxRuntimeParam->availableDataSize     = 0;
	rxRuntimeParam->transferCount         = 0;
	rxRuntimeParam->framesHandledCount    = 0;
	rxRuntimeParam->nextFlowControlCount  = 0;
	rxRuntimeParam->pdurBufferCount       = 0;
	rxRuntimeParam->pdurBuffer.SduDataPtr = NULL_PTR;
}


/***************************************************************************************************
 * Service name      : StartNewTransmission                                                        *
 * Parameters (in)   : txRuntimeParam -> Pointer to the container RunTimeInfo_s                    *
 * Parameters (inout): None                                                                        *
 * Parameters (out)  : None                                                                        *
 * Return value      : None                                                                        *
 * Description       : Used to initialize the Runtime value that will used in transmission         *
 ***************************************************************************************************/
static void StartNewTransmission(RunTimeInfo_s *txRuntimeParam)
{
	/*[SWS_CanTp_00030] The function CanTp_Init shall initialize all global variables of the module and sets
	 * all transport protocol connections in a sub-state of CANTP_ON, in which neither segmented transmission
	 * nor segmented reception are in progress (Rx thread in state CANTP_RX_WAIT and Tx thread in state CANTP_TX_WAIT). (SRS_Can_01075)*/

	txRuntimeParam->state                 = IDLE          ;
	txRuntimeParam->mode                  = CANTP_TX_WAIT ;
	txRuntimeParam->transferTotal         = 0;
	txRuntimeParam->Buffersize            = 0;
	txRuntimeParam->availableDataSize     = 0;
	txRuntimeParam->transferCount         = 0;
	txRuntimeParam->framesHandledCount    = 0;
	txRuntimeParam->nextFlowControlCount  = 0;
	txRuntimeParam->pdurBufferCount       = 0;
	txRuntimeParam->pdurBuffer.SduDataPtr = NULL_PTR;

}


