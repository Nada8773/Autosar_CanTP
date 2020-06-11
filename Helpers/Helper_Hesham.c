/*
 * Helper.c
 *
 *  Created on: Jun 7, 2020
 *      Author: Hesham Elsherbieny
 */
#include"Std_Types.h"
#include"CanTp_Cfg.h"
#include"CanTp_CanIf.h"
#include"PduR_CanTp.h"
#include"CanTp.h"

uint8 canif_ids[]={cantp_canif_tx};
uint8 cantp_com_copy_tx[]={CanTp_copy_cantp};
uint8 cantp_com_rx_or_copy[]= {cantp_com_rx};
uint8 cantp_cainf_FC[] ={CanTpTxFcNPdu_canif};

static void PadFrame(PduInfoType *PduInfoPtr)
{
	uint8 i = 0;
	for (i = PduInfoPtr->SduLength; i < MAX_SEGMENT_DATA_SIZE; i++)
	{
		PduInfoPtr->SduDataPtr[i] = CANTP_PADDING_BYTE;
	}
	PduInfoPtr->SduLength = MAX_SEGMENT_DATA_SIZE;
}

/*This function copies Data from pdurBuffer to CanFrameBuffer */
static BufReq_ReturnType sendNextTxFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo *txRuntime)
{
	BufReq_ReturnType ret = BUFREQ_OK;

	/* [SWS_CanTp_00272] The API PduR_CanTpCopyTxData() contains a parameter used for the recovery mechanism – ‘retry’.
	 * Because ISO 15765-2 does not support such a mechanism, the CAN Transport Layer does not implement
	 * any kind of recovery. Thus, the parameter is always set to NULL pointer. */
	RetryInfoType* retry = NULL_PTR;

	/* make SduDataPtr point to first byte of Payload data */
	txRuntime->pdurBuffer.SduDataPtr = & (txRuntime->IFdata[txRuntime->IFByteCount]);

	if (txRuntime->availableDataSize == 0)           /* in case of SF or FF, no data left un sent */
	{
		ret                       = PduR_CanTpCopyTxData(cantp_com_copy_tx[txConfig->CanTpTxNSduId],&txRuntime->pdurBuffer,retry,&txRuntime->availableDataSize);
		txRuntime->transferCount += txRuntime->pdurBuffer.SduLength;
	}

	else

#if (CANTP_USE_STANDARD_ADDRESSING == STD_ON)          /* in case of standard Addressing */
	{
		if ( (txRuntime->availableDataSize) > MAX_PAYLOAD_CF)	     /* in case of Full CF */
		{
			(txRuntime->pdurBuffer.SduLength) =  MAX_PAYLOAD_SF;
		}
		else if ( (txRuntime->availableDataSize) <= MAX_PAYLOAD_CF)	/* in case of Last CF */
		{
			(txRuntime->pdurBuffer.SduLength) = (txRuntime->availableDataSize);
		}

	    (txRuntime->transferCount)       += (txRuntime->pdurBuffer.SduLength);
		ret                               = PduR_CanTpCopyTxData(cantp_com_copy_tx[txConfig->CanTpTxNSduId],&txRuntime->pdurBuffer,retry,&txRuntime->availableDataSize);
	}
#elif(CANTP_USE_EXTENDED_ADDRESSING == STD_ON)		  /* in case of Extended */
	{
		if ( (txRuntime->availableDataSize) > MAX_PAYLOAD_CF)	     /* in case of Full CF */
		{
			(txRuntime->pdurBuffer.SduLength) = MAX_PAYLOAD_SF;
			ret                               = PduR_CanTpCopyTxData(txConfig->CanTpTxNSduId,&txRuntime->pdurBuffer,Retry,&txRuntime->availableDataSize);
		}

		else if ( (txRuntime->availableDataSize) <= MAX_PAYLOAD_CF)	/* in case of Last CF */
		{
			txRuntime->pdurBuffer.SduLength = txRuntime->availableDataSize;
			ret                             = PduR_CanTpCopyTxData(txConfig->CanTpTxNSduId,&txRuntime->pdurBuffer,Retry,&txRuntime->availableDataSize);
		}
	}
#endif

	txRuntime->IFByteCount += txRuntime->pdurBuffer.SduLength;   // Updating bytecount

	/* copying Data from pdurBuffer to canFrameBuffer and all in runtime */
	txRuntime->IFdata = txRuntime->pdurBuffer.SduDataPtr;

 	if(BUFREQ_OK == ret) /* data copied successfully */
	{
 		PduInfoType pduInfo;
		Std_ReturnType resp;
    
		pduInfo.SduDataPtr = txRuntime->IFdata;
		pduInfo.SduLength  = txRuntime->IFByteCount;

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
		txRuntime->NasNarPending     =  TRUE;
		txRuntime->state             =  TX_WAIT_TX_CONFIRMATION;

		uint8 canif_id = canif_ids[txConfig->CanTpTxNSduId];
		resp           = CanIf_Transmit(canif_id, pduInfo);

		if(E_OK == resp)
		{
			/* data sent successfully */
		}
		else
		{
			/* failed to send, retruning value of BUFREQ_E_NOT_OK to caller */
			ret = BUFREQ_E_NOT_OK;
		}
	}
  else /* BUFREQ_E_NOT_OK or BUFREQ_E_BUSY */
	{
		/* failed to copy new data */
	}

 	return ret;
}


static void handleNextTxFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo *txRuntime)
{
	/* Increment Number of frames handled in this specific block of Consecutive frames */
	txRuntime->framesHandledCount++;

	/* Prepare TX buffer for next frame to be sent */
	txRuntime->IFByteCount = 0;

#if (CANTP_USE_EXTENDED_ADDRESSING == STD_ON)   /* for Extended Addressing Mode */
	txRuntime->IFByteCount++;
#endif

	/* To prepare First byte in CF */
	txRuntime->IFdata[txRuntime->IFByteCount++] = (txRuntime->framesHandledCount & SEGMENT_NUMBER_MASK) + ISO15765_TPCI_CF;

	/* Decrement nextFlowControlCount each time entering function indicating
	 * how much frames i need to send later until the state of waiting for FC Frame */
	COUNT_DECREMENT(txRuntime->nextFlowControlCount);

	/* In Case of Last Block of Consecutive Frames is Received (End of Transmission) */
	if ( (txRuntime->transferTotal) <= (txRuntime->transferCount) )
	{
		/* Transfer finished! */
		//PduR_CanTpTxConfirmation(txConfig->PduR_PduId, E_OK);
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
		resp = sendNextTxFrame(txConfig, txRuntime);

		if (BUFREQ_OK == resp)
		{
			// successfully sent frame, wait for TX confirm
		} else if(BUFREQ_E_BUSY == resp)
		{
			/* [SWS_CanTp_00184] If the PduR_CanTpCopyTxData() function returns BUFREQ_E_BUSY,
			 * the CanTp module shall later (implementation specific) retry to copy the data. */

			/* Change State and Setup Timeout trying to copy the data later if it's available
			 * before N_Cs timeout */
			txRuntime->stateTimeoutCount = (txConfig->CanTpNcs);
			txRuntime->state             =  TX_WAIT_TRANSMIT;
		}
		else if(BUFREQ_E_NOT_OK == resp)
		{
			/* [SWS_CanTp_00087] If PduR_CanTpCopyTxData() returns BUFREQ_E_NOT_OK,
			 * the CanTp module shall abort the transmit request and notify the upper layer of this failure
			 * by calling the callback function PduR_CanTpTxConfirmation() with the result E_NOT_OK. */

			//PduR_CanTpTxConfirmation(txConfig->PduR_PduId, E_NOT_OK);
			txRuntime->state = IDLE;
			txRuntime->mode  = CANTP_TX_WAIT;
		}
	}
	else
		/* In case of nextFlowControlCount != 0 (in-between CF) or BS == 0 (last block of CFs)
		 * and it's not time to send (STmin != 0) */
	{
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



// The Caller 'RxIndication' needs to be re-handled to pass a general FC Frame
static void ReceiveFlowControlFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo *txRuntime, const PduInfoType *PduData)
{
	uint8 indexCount = 0;

	/* Make sure that the frame is in 'TX_WAIT_FLOW_CONTROL' state */
	if ( txRuntime->state == TX_WAIT_FLOW_CONTROL )
	{
#if (CANTP_USE_EXTENDED_ADDRESSING == STD_ON)   /* for Extended Addressing Mode */
		indexCount++;
#endif
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

				// commented and called before Copying fro PduR Buffer as per SWS
				//txRuntime->stateTimeoutCount    = (txConfig->CanTpNcs);
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
