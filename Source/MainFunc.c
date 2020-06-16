void CanTp_MainFunction(void)
{
	const  CanTpTxNSdu_s *txConfig        = NULL_PTR;
	const  CanTpRxNSdu_s *rxConfig        = NULL_PTR;

	static uint8          FCWaitCount     = 0;

	       RunTimeInfo_s *txRuntime       = NULL_PTR;
	       RunTimeInfo_s *rxRuntime       = NULL_PTR;

	       PduInfoType   *NextBlock       = NULL_PTR;
	       PduInfoType   *Request         = NULL_PTR;
	       PduLengthType  RemainingBytes  = 0;

	       PduIdType      NSduCounter     = 0;

	       BufReq_ReturnType ret          = BUFREQ_OK;

	/* [SWS_CanTp_00238] The CanTp module shall perform segmentation and reassembly tasks only when
	 * the CanTp is in the CANTP_ON state. */

	/* Checking that CanTp is in the CANTP_ON state which means that's initialized before */
	if(RunTimeInfo.internalState != CANTP_ON)
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

		/* for each Tx NSDU */
		for( NSduCounter=0; NSduCounter < CANTP_NSDU_CONFIG_LIST_SIZE_TX; NSduCounter++ ) //TODO: replace for loop if possible
		{
			txConfig  = (CanTpTxNSdu_s*)&CanTpTxNSdu[NSduCounter];
			rxConfig  = (CanTpRxNSdu_s*)&CanTpRxNSdu[NSduCounter];

			txRuntime = (RunTimeInfo_s*)&RunTimeInfo;

			switch (txRuntime->state)
			{

			case TX_WAIT_STMIN:

				/* Make sure that STmin timer has expired */
				TIMER_DECREMENT(txRuntime->stateTimeoutCount);

				if (txRuntime->stateTimeoutCount != 0)
				{
					break;
				}
				txRuntime->state = TX_WAIT_TRANSMIT;                /* when stateTimeoutCount = 0 */

				/* Update state timeout with the remaining time of N_Cr after ST has passed, as shown in ISO 15765-2 */
				txRuntime->stateTimeoutCount = (rxConfig->CanTpNcr) - (txRuntime->STmin);

			/* No need for break here as we want to go for next case directly not to be delayed for main period time */
			case TX_WAIT_TRANSMIT:
			{
				ret = sendNextTxFrame(txConfig, txRuntime);

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
				/* Decrement to check N_As/N_Ar timeout */ //TODO: handle where to start timer N_Ar from receiver side
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

		}

/*-----------------------------------------------Receiver Side-------------------------------------------------------*/

		/* Handling Receiver Side timeouts N_Ar,N_Br and N_Cr */

		/* for each Rx NSDU */
		for( NSduCounter=0; NSduCounter < CANTP_NSDU_CONFIG_LIST_SIZE_RX; NSduCounter++ )
		{
			txConfig  = (CanTpTxNSdu_s*)&CanTpTxNSdu[NSduCounter];
			rxConfig  = (CanTpRxNSdu_s*)&CanTpRxNSdu[NSduCounter];

			rxRuntime = (RunTimeInfo_s*)&RunTimeInfo;

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

						SendFlowControl(rxConfig, rxRuntime, FLOW_CONTROL_WAIT_FRAME);
						rxRuntime->stateTimeoutCount = rxConfig->CanTpNbr; //TODO: start N_Ar timout

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
				else			/*	stateTimeoutCount != 0, N_Br Timer is active */
				{

					/* [SWS_CanTp_00222]  While the timer N_Br is active, the CanTp module shall call the service PduR_CanTpCopyRxData()
					 * with a data length 0 (zero) and NULL_PTR as data buffer during each processing of the MainFunction.*/

					Request->SduDataPtr   = NULL_PTR;
					Request->SduLength    = 0;

					PduR_CanTpCopyRxData(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], Request, &rxRuntime->Buffersize);

					/* Assign 'NextBlock' with the next block information which in stored IFdata, IFByteCount*/
					NextBlock->SduDataPtr = rxRuntime->IFdata;
					NextBlock->SduLength  = rxRuntime->IFByteCount;

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
								sendFlowControlFrame( rxConfig, rxRuntime, FLOW_CONTROL_CTS_FRAME);

								/* [SWS_CanTp_00312] The CanTp module shall start a time-out N_Cr at each indication of
								 * CF reception (except the last one in a block) and at each confirmation of a FC transmission
								 * that initiate a CF transmission on the sender side (FC with FS=CTS). */
								rxRuntime->stateTimeoutCount = (rxConfig->CanTpNcr);
								rxRuntime->state             = RX_WAIT_CONSECUTIVE_FRAME;
							}
							else /* Receive process is ended successfully */
							{
								PduR_CanTpRxIndication(cantp_com_rx_or_copy[rxConfig->CanTpRxNSduId], E_OK);			  /* CanTp_00214 */
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
