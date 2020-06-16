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
static BufReq_ReturnType SendNextTxFrame(const CanTpTxNSdu_s *txConfig, RunTimeInfo *txRuntime)
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
	txRuntime->pdurBuffer.SduDataPtr = & (txRuntime->IFdata[txRuntime->IFByteCount]);

	if (txRuntime->availableDataSize == 0)           /* in case of SF or FF, no data left unsent */
	{
		ret                       = PduR_CanTpCopyTxData(cantp_com_copy_tx[txConfig->CanTpTxNSduId],&txRuntime->pdurBuffer,retry,&txRuntime->availableDataSize);
		txRuntime->transferCount += txRuntime->pdurBuffer.SduLength;
	}

	else /* In case of any CF */
	{
		/* Value of MAX_PAYLOAD of each frame differentiates depending on the addressing mode whether
		 * it's standard or extended Addressing modes */
		if ( (txRuntime->availableDataSize) > MAX_PAYLOAD_CF)	     /* in case of Full CF */
		{
			(txRuntime->pdurBuffer.SduLength) =  MAX_PAYLOAD_CF;
		}
		else if ( (txRuntime->availableDataSize) <= MAX_PAYLOAD_CF)	 /* in case of Last CF */
		{
			(txRuntime->pdurBuffer.SduLength) = (txRuntime->availableDataSize);
		}

		ret                               = PduR_CanTpCopyTxData(cantp_com_copy_tx[txConfig->CanTpTxNSduId],&txRuntime->pdurBuffer,retry,&txRuntime->availableDataSize);
		/* copying Data from pdurBuffer to canFrameBuffer and all in runtime */
		txRuntime->IFdata                 = txRuntime->pdurBuffer.SduDataPtr;
		/* Updating total transfered amount of bytes */
		txRuntime->transferCount         += txRuntime->pdurBuffer.SduLength;
		/* Updating total number of bytes to be transmitted to CanIF */
		txRuntime->IFByteCount           += txRuntime->pdurBuffer.SduLength;
	}

	if(BUFREQ_OK == ret) /* data copied successfully */
	{
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
		txRuntime->NasNarPending     =  TRUE; // i see no clear use for it !!
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
		resp = sendNextTxFrame(txConfig, txRuntime);

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

static void ReceiveConsecutiveFrame(const CanTpRxNSdu_s *rxConfig, RunTimeInfo *rxRuntime, const PduInfoType *rxPduData)
{
	uint8 indexCount                        = 0;
	uint8 segmentNumber                     = 0;
	PduLengthType currentSegmentSize        = 0;
	PduLengthType bytesCopiedToPdurRxBuffer = 0;
	BufReq_ReturnType ret                   = BUFREQ_E_NOT_OK;

	PduInfoType *info = rxPduData;
	uint8 id          = cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId] ;

	//TODO: if(mode == CanTp_Rx_PROCESSING) ---> FF sets the mode to be processing,
	//so we have to ensure we are entering in the right mode...
	if (rxRuntime->state == RX_WAIT_CONSECUTIVE_FRAME)
	{

#if (CANTP_USE_EXTENDED_ADDRESSING == STD_ON)   /* for Extended Addressing Mode */
		indexCount++;
#endif
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
			info->SduDataPtr = &rxPduData->SduDataPtr[indexCount];

			/* Update SduLength by the value of 'L-PduLength(containing PCI) - 1' */
			info->SduLength  = (rxPduData->SduLength) - 1;

			COUNT_DECREMENT(rxRuntime->nextFlowControlCount);

			/* Checking that it's the last CF or not. If the condition is true, it means that it's a last CF */
			if ( (rxRuntime->nextFlowControlCount == 0)  && (rxRuntime->BS) )
			{
				/* [SWS_CanTp_00166] At the reception of a FF or last CF of a block, the CanTp module shall
				 *  start a time-out N_Br before calling PduR_CanTpStartOfReception or PduR_CanTpCopyRxData.*/

				/* start N_Br timer */
				rxRuntime->stateTimeoutCount = (rxConfig->CanTpNbr);

				/* copy data to PduR Buffer */
				ret = PduR_CanTpCopyRxData(id,info,&rxRuntime->Buffersize);

				/* Incoming frame is copied successfully then we should increment number of handled frames */
				rxRuntime->framesHandledCount++;

				/* Checking that we are handling the last CF of the last Block or just in-between block */
				if(rxRuntime->Buffersize == 0) // it was buffersize ==0 !!
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

					sendFlowControlFrame(rxConfig, rxRuntime, FS);
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
				ret = PduR_CanTpCopyRxData(id,info,&rxRuntime->Buffersize); //

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
