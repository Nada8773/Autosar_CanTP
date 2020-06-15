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

#include <CanTp_Types.h>
#include "CanTp.h"
#include "CanTp_Cfg.h"
#include "CanTp_Cbk.h"
#include "SchM_CanTp.h"
#include "MemMap.h"
#include "CanTp_Det.h"
#include "Det.h"


/* Private Function Prototype */
static void ReceiveSingleFrame(const CanTpRxNSdu_s *rxConfig, RunTimeInfo_s *rxRuntime, const PduInfoType *rxPduData);
static void ReceiveFirstFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, const PduInfoType *CanTpPduData);
static void SendFlowControlFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, BufReq_ReturnType flowControlStatus);
static void PadFrame(PduInfoType *PduInfoPtr);
static void HandleReceivedFrame(PduIdType RxPduId, const PduInfoType *CanTpPduData);
static void StartNewReception(RunTimeInfo_s *rxRuntimeParam);

//static void ReceiveConsecutiveFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, const PduInfoType *CanTpPduData);

/* Global Object */
RunTimeInfo_s CanTpRunTimeData =
{       .initRun = FALSE,
        .internalState = CANTP_OFF,
};



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
    /* TODO used in SendnextTx*/
    /*txRuntime->availableDataSize =0;
    and IFData[] need to remove
    NASNARPandding -> Remove if not used*/


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
    const CanTpTxNSdu_s *txConfig  = NULL;
    RunTimeInfo_s       *txRuntime = NULL;
    uint8 MAX_PAYLOAD ;
    FramType Frame ;
    /* [SWS_CanTp_00225] For specific connections that do not use MetaData, the
                         function CanTp_Transmit shall only use the full SduLength
                         information and shall not use the available N-SDU data buffer
                         in order to prepare Single Frame or First Frame PCI */

    txConfig    =  &CanTpTxNSdu[TxPduId];


#if txConfig->CanTpTxAddressingFormat == CANTP_STANDARD
    MAX_PAYLOAD = MAX_PAYLOAD_STANDRAD_CF ;
#elif txConfig->CanTpTxAddressingFormat == CANTP_EXTENDED
    MAX_PAYLOAD = MAX_PAYLOAD_EXTENDED_CF ;
#endif


    if( CanTpRunTimeData.internalState == CANTP_OFF )
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
    else if(TxPduId < CANTP_NSDU_CONFIG_LIST_SIZE_TX)
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
    else if (txRuntime->state == IDLE)
    {

        txRuntime->transferTotal = PduInfoPtr->SduLength; /* How ?????????????????????*/

        txRuntime->mode          = CANTP_TX_PROCESSING;

        txRuntime->IFByteCount   = 0;

        if( txRuntime->transferTotal <= MAX_PAYLOAD ) /* Single Frame */
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
            txRuntime->IFdata[txRuntime->IFByteCount++] = ISO15765_TPCI_SF | (txRuntime->transferTotal);
            txRuntime->state = TX_WAIT_TRANSMIT;
            TransmitRequest_Status = E_OK ;
            //          txRuntime->pdurBuffer.SduLength = txRuntime->transferTotal ;

            break;
        case  FIRST_FRAM :
            txRuntime->IFdata[txRuntime->IFByteCount++] = ISO15765_TPCI_FF | (uint8)((txRuntime->transferTotal & 0xf00) >> 8);

            txRuntime->IFdata[txRuntime->IFByteCount++] = (uint8)(txRuntime->transferTotal & 0xff);

            txRuntime->state = TX_WAIT_TRANSMIT;

            TransmitRequest_Status = E_OK ;

            /* txRuntime->iso15765.nextFlowControlCount = 1;
               txRuntime->iso15765.BS = 1; */
            break;
        default:
            TransmitRequest_Status = E_NOT_OK ;
        break;

        }
    }

    else if(txRuntime->state == CANTP_TX_PROCESSING)
    {
        TransmitRequest_Status = E_NOT_OK ;
    }

    else
    {
        /* Nothing */
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
}


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

void CanTp_TxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{
    RunTimeInfo_s *RunTimeInfo;
    const CanTpTxNSdu_s *txConfig = NULL;

    uint8 Canif_Id ;
    //Canif_Id = canif_ids[CanTpTxNSdu[].CanTpTxNSduId];
    //Canif_Id = canif_ids[TxPduId];
    /* TODO Check if ID is Valid */
    /* In Transmit made an variable that will store the
     * index of CanTpTxNSdu to use it here
     * and Apply the same For RXIndication
     */


    /* [SWS_CanTp_00111] If called when the CanTp module is in the global state CANTP_ON,
                             the function CanTp_Init shall return the module to state Idle
                             (state = CANTP_ON, but neither transmission nor reception are in progress */

    if( CanTpRunTimeData.internalState == CANTP_OFF )
    {
#if DET_ERROR_STATUS == STD_ON
        /* API service used without module initialization : On any API call except CanTp_Init() and
               CanTp_GetVersionInfo() if CanTp is in state CANTP_OFF" */
        Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_UNINIT);
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
            RunTimeInfo->state = IDLE;
            RunTimeInfo->mode  = CANTP_RX_WAIT;
            break;
        case E_OK:
            switch(RunTimeInfo->Direction)
            {
            case CANTP_DIRECTION_TRANSMIT: /* CF & FC & FF & SF */

                if(RunTimeInfo->state == TX_WAIT_TX_CONFIRMATION)
                {
                    handleNextTxFrameSent(&CanTpTxNSdu[TxPduId], RunTimeInfo);
                }
                else
                {
                    /* Nothing */
                }
                break;
            case CANTP_DIRECTION_RECEIVE:
                /* TODO Update NasNarPending = FALSE */
                break;
            default:
                break;
            }
            break;
        }
    }
    else /* Invaild ID */
    {
#if DET_ERROR_STATUS == STD_ON
        RunTimeInfo->state = IDLE;
        RunTimeInfo->mode  = CANTP_RX_WAIT;
        Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TXCONFIRMATION_SERVICE_ID, CANTP_E_INVALID_RX_ID);
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
    else if(CanTpRunTimeData.internalState == CANTP_OFF)
    {
#if DET_ERROR_STATUS == STD_ON
        /* API service used without module initialization : On any API call except CanTp_Init() and
               CanTp_GetVersionInfo() if CanTp is in state CANTP_OFF" */
        Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_UNINIT);
#endif
    }


    else
    {
        /* TODO Need To Check If ID is Valid page 48*/
        HandleReceivedFrame(RxPduId, PduInfoPtr);
    }
}

/*****************************************************************************************************
 * Service name      : HandleReceivedFrame                                                           *
 * Parameters (in)   : RxPduId ID of the received PDU.                                               *
 *                     CanTpPduData Contains the length (SduLength) of the received PDU,             *
 *                     a pointer to a buffer (SduDataPtr) containing the PDU, and the MetaData       *
 *                     related to this PDU.                                                          *
 * Parameters (inout): None                                                                          *
 * Parameters (out)  : None                                                                          *
 * Return value      : None                                                                          *
 * Description       : Function Used By CanTp_RxIndication To Get the FrameType and Receive Data     *
 *****************************************************************************************************/
static void HandleReceivedFrame(PduIdType RxPduId, const PduInfoType *CanTpPduData)
{
    uint8 CanTpPduDataPCI_Offset ;
    uint8 CanTp_FrameID;
    uint8 CanTp_FcFrameID ;
    RunTimeInfo_s *RunTimeInfo ;

#if rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD  /*(SF & FF & CF & FC)-> [ ID -> (7-4)Bits at first Byte ] */
    CanTpPduDataPCI_Offset = 0;
#elif rxNSduConfig->CanTpRxAddressingFormat == CANTP_EXTENDED
    CanTpPduDataPCI_Offset = 1;
#endif

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
                (CanTpRxNSdu[RxPduId].CanTpTxPaddingActivation == CANTP_ON) )
        {
#if DET_ERROR_STATUS == STD_ON
            Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
#endif
            PduR_CanTpRxIndication(CanTpRxNSdu[RxPduId].CanTpTxNSduId, E_NOT_OK);
        }
        else
        {
            // if((CanTpTxNSdu[RxPduId] != NULL_PTR))
            ReceiveSingleFrame(&CanTpRxNSdu[RxPduId], RunTimeInfo, CanTpPduData);
        }
        break;

    case ISO15765_TPCI_FF: /* First Frame ******************************************************/
        ReceiveFirstFrame(&CanTpRxNSdu[RxPduId], RunTimeInfo, CanTpPduData);
        break;

    case ISO15765_TPCI_CF: /* Consecutive Frame ************************************************/

        /* [SWS_CanTp_00346] If frames with a payload <= 8 (either CAN 2.0 frames or small CAN FD frames)
                              are used for a Rx N-SDU and CanTpRxPaddingActivation is equal to CANTP_ON,
                              and CanTp receives by means of CanTp_RxIndication() call a last CF Rx N-PDU
                              belonging to that NSDU, with a length smaller than eight bytes
                              (i.e. PduInfoPtr. SduLength != 8), CanTp shall abort the ongoing reception by
                              calling PduR_CanTpRxIndication() with the result E_NOT_OK. The runtime error
                              code CANTP_E_PADDING shall be reported to the Default Error Tracer */
        /* TODO RunTimeInfo is it Array or not ??*/
        if ( (RunTimeInfo[RxPduId].nextFlowControlCount == 0)  && (RunTimeInfo[RxPduId].BS) ) /* Last Consecutive Frame */
        {
            if( (CanTpPduData->SduLength < MAX_FRAME_BYTES) &&  \
                    (CanTpRxNSdu[RxPduId].CanTpTxPaddingActivation == CANTP_ON) )
            {
#if DET_ERROR_STATUS == STD_ON
                Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
#endif
                PduR_CanTpRxIndication(CanTpRxNSdu[RxPduId].CanTpTxNSduId, E_NOT_OK);
            }
            else
            {
                ReceiveConsecutiveFrame(&CanTpRxNSdu[RxPduId], RunTimeInfo, CanTpPduData);
            }
        }
        else
        {
            ReceiveConsecutiveFrame(&CanTpRxNSdu[RxPduId], RunTimeInfo, CanTpPduData);
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
                (CanTpRxNSdu[RxPduId].CanTpTxPaddingActivation == CANTP_ON) )
        {
#if DET_ERROR_STATUS == STD_ON
            Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
#endif
            /* TODO ????????????????????? */
            PduR_CanTpTxConfirmation(CanTpTxNSdu[RxPduId].CanTpTxNPduConfirmationPduId, E_NOT_OK);
        }
        else
        {
            /* Get the Frame Status for Flow Control */
            CanTp_FcFrameID = CanTpPduData->SduDataPtr[CanTpPduDataPCI_Offset] & ISO15765_TPCI_FS_MASK ;
            ReceiveFlowControlFrame(&CanTpTxNSdu[RxPduId], RunTimeInfo, CanTpPduData);
        }
        break;

    default:
        /* Do Nothing */
        break;
    }
    break;
    default:
        /* Do Nothing */
        break;
}
}

static void StartNewReception(RunTimeInfo_s *rxRuntimeParam)
{
    rxRuntimeParam->state           = IDLE          ;
    rxRuntimeParam->mode            = CANTP_RX_WAIT ;
    rxRuntimeParam->transferTotal   = 0; // ->  rxRuntime->TotalNSduLength = 0;
    rxRuntimeParam->Buffersize      = 0;
    rxRuntimeParam->CanTpRxWftCount = 0;
    rxRuntimeParam->transferCount   = 0;

    /*rxRuntime->NasNarPending = FALSE;
    rxRuntime->framesHandledCount = 0;
    rxRuntime->nextFlowControlCount = 0;
    rxRuntime->pdurBufferCount = 0;
    rxRuntime->TotalNSduLength = 0;
    rxRuntime->pdurBuffer.SduDataPtr = NULL;
     */
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
        CanTP_StartNewRx(rxRuntimeParam);
    }
    else
    {
        /* Do Nothing */
    }

#if rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD   /* Single Frame -> [ 1Byte (PCI) + 7Bytes (Data) ] */
    CanTpPduData_Offset       = 1;
    CanTpPduDataLength_Offset = 0;
#elif rxNSduConfig->CanTpRxAddressingFormat == CANTP_EXTENDED
    CanTpPduData_Offset       = 2;
    CanTpPduDataLength_Offset = 1;
#endif

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
        rxRuntimeParam->state = IDLE;
        rxRuntimeParam->mode  = CANTP_RX_WAIT;
        break;
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
    BufReq_ReturnType  BufferRequest_Status = BUFREQ_E_NOT_OK ;
    BufReq_ReturnType  CopyData_Status      = BUFREQ_E_NOT_OK ;

    /* Status -> Segmented Receive in progress
                 Terminate the current reception,report an indication, with parameter Result set to
                 E_NOT_OK, to the upper layer, and process the FF N-PDU as the start of a new reception
       Status -> IDle
                Process the SF N-PDU as the start of a new reception */
    if(rxRuntimeParam->state == CANTP_RX_PROCESSING)
    {
        /* Report an PDUR with E_NOT_OK to Abort*/
        PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);

        /* Start new Reception */
        CanTP_StartNewRx(rxRuntimeParam);
    }
    else
    {
        /* Do Nothing */
    }

#if rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD /* First Frame -> [ 2Byte (PCI) + 6Bytes (Data) ] */
    CanTpPduData_Offset       = 2;
    CanTpPduDataLength_Offset = 0;
#endif

    /* Pdu Recieved Data */
    rxPduData.SduDataPtr          = &CanTpPduData->SduDataPtr[CanTpPduData_Offset] ;

    rxPduData->SduLength          = CanTpPduData->SduLength;

    /* Get total Length FF_DL */
    rxRuntimeParam->transferTotal = ( (CanTpPduData->SduDataPtr[CanTpPduDataLength_Offset] & ISO15765_TPCI_DLFF ) << 8 ) \
            + CanTpPduData->SduDataPtr[CanTpPduDataLength_Offset + 1] ;

    /* No Flow Control is sent yet */
    rxRuntimeParam->CanTpRxWftCount = 0;

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
                    rxRuntime->mode                   = CANTP_RX_PROCESSING      ;
                }
                else
                {
                    PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_OK);

                    /* Change State To wait Consecutive Frame */
                    rxRuntimeParam->state = RX_WAIT_CONSECUTIVE_FRAME;

                    /* First Consecutive Frame Number */
                    rxRuntimeParam->framesHandledCount = 1;

                    /* Processing Received mode */
                    rxRuntime->mode       = CANTP_RX_PROCESSING;

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
        /* abort the reception*/
        rxRuntimeParam-state = IDLE;
        rxRuntimeParam->mode = CANTP_RX_WAIT;
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
static void SendFlowControlFrame(const CanTpTxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, BufReq_ReturnType FlowControlStatus)
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
#if rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD /* Cantp is standard  addressing format */

        Local_BSValue = ( rxRuntimeParam->Buffersize / MAX_PAYLOAD_STANDRAD_CF ) ;

        if( (rxRuntimeParam->Buffersize % MAX_PAYLOAD_SF_STD_ADDR) )
        {
            Local_BSValue += 1;
        }
        else
        {
            /* Do nothing */
        }
#endif

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
        rxRuntimeParam->stateTimeoutCount                = (rxNSduConfig->CanTpNbs);
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
#if rxNSduConfig->CanTpTxPaddingActivation == CANTP_ON
    PadFrame( &txFlowControlData);
#endif


    /* [SWS_CanTp_00343]  CanTp shall terminate the current transmission connection
                          when CanIf_Transmit() returns E_NOT_OK when transmitting an SF, FF, of CF */
    /* TODO change ID */
    TransmitReqCanIf_Status = CanIf_Transmit(cantp_cainf_FC[rxNSduConfig->CanTpTxNSduId], &txFlowControlData);
    switch(TransmitReqCanIf_Status)
    {
    case E_OK :
        /* [SWS_CanTp_00090] When the transport transmission session is successfully
                             completed, the CanTp module shall call a notification service of the upper layer,
                             PduR_CanTpTxConfirmation(), with the result E_OK.*/
        PduR_CanTpTxConfirmation(cantp_cainf_FC[rxNSduConfig->CanTpTxNSduId] ,E_OK );
        break;
    case E_NOT_OK :
        /* terminate the current transmission connection */
        /* Note: The Api PduR_CanTpTxConfirmation() shall be called after a transmit
                 cancellation with value E_NOT_OK */
        /* TODO Change ID */
        PduR_CanTpTxConfirmation(cantp_cainf_FC[rxNSduConfig->CanTpTxNSduId] ,E_NOT_OK );
        break;
    }
}

/***************************************************************************************************
 * Service name      : SetPaddingValue                                                             *
 * Parameters (in)   : NSduData           -> Pointer to FC Frame                                   *
 *                     Start_IndexCount   -> start byte used padding                               *
 *                     CanTp_PaddingByte  -> Byte Value                                            *
 * Parameters (inout): None                                                                        *
 * Parameters (out)  : None                                                                        *
 * Return value      : None                                                                        *
 * Description       : Used for the initialization of unused bytes with a certain value            *
 ***************************************************************************************************/
/*static void SetPaddingValue( uint8* NSduData, uint8 Start_IndexCount, uint8 CanTp_PaddingByte)
{
    uint8 Local_IndexCount ;
    for(Local_IndexCount = Start_IndexCount ; Local_IndexCount < MAX_FRAME_BYTES ; Local_IndexCount++)
    {
        NSduData[Local_IndexCount] = CanTp_PaddingByte ;
    }
}
 */
static void PadFrame(PduInfoType *PduInfoPtr)
{
    uint8 Local_IndexCount  = 0;
    for (Local_IndexCount = PduInfoPtr->SduLength; Local_IndexCount < MAX_SEGMENT_DATA_SIZE; Local_IndexCount++)
    {
        PduInfoPtr->SduDataPtr[Local_IndexCount] = CANTP_PADDING_BYTE;
    }
    PduInfoPtr->SduLength = MAX_SEGMENT_DATA_SIZE;
}













/*************************************************************************************************
 * Service name      : ReceiveConsecutiveFrame                                                   *
 * Parameters (in)   : rxNSduConfig       -> Pointer to  CanTpRxNSdu container                   *
 *                     rxRuntimeParam     -> Pointer to RunTimeInfo container                    *
 *                     flowControlStatus  -> Flow Control Status                                 *
 * Parameters (inout): None                                                                      *
 * Parameters (out)  : None                                                                      *
 * Return value      : None                                                                      *
 * Description       : Receiver Receive Consecutive Frame                                        *
 *************************************************************************************************/
static void ReceiveConsecutiveFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, const PduInfoType *CanTpPduData)
{
    uint8 CanTpPduData_Offset ;
    uint8 CanTpCF_SegmentNumber;
    static uint16 bytesRemaining;

    BufReq_ReturnType  BufferRequest_Status = BUFREQ_E_NOT_OK ;
    BufReq_ReturnType  CopyData_Status      = BUFREQ_E_NOT_OK ;

    /* [page 46] If awaited,process the CF N-PDU in the on-going reception and perform the
       required checks(e.g. SN in right order) */
    if(rxRuntimeParam->mode == CANTP_RX_PROCESSING)
    {
        if ( rxRuntimeParam->state == RX_WAIT_CONSECUTIVE_FRAME )
        {
#if rxNSduConfig->CanTpRxAddressingFormat == CANTP_STANDARD   /* Single Frame -> [ 1Byte (PCI) + 7Bytes (Data) ] */
            CanTpPduData_Offset       = 1;
#elif rxNSduConfig->CanTpRxAddressingFormat == CANTP_EXTENDED
            CanTpPduData_Offset       = 2;
#endif

            /* Get Segment number from received frame */
            CanTpCF_SegmentNumber = CanTpPduData->SduDataPtr[CanTpPduData_Offset] & SEGMENT_NUMBER_MASK;
            bytesRemaining = rxRuntimeParam->transferTotal -  rxRuntimeParam->transferCount;


            /*[SWS_CanTp_00314] The CanTp shall check the correctness of each SN received
                                during a segmented reception. In case of wrong SN received the CanTp module shall
                                abort reception and notify the upper layer of this failure by calling the indication
                                function PduR_CanTpRxIndication() with the result E_NOT_OK */
            if(  CanTpCF_SegmentNumber == rxRuntimeParam->framesHandledCount )
            {


                /* TODO Change ID */
                /* Id           ->  dentification of the received I-PDU.
                 * info         -> Provides the source buffer (SduDataPtr) and the number of
                                   bytes to be copied
                 *bufferSizePtr -> Available receive buffer after data has been copied.
                 */
                CopyData_Status = PduR_CanTpCopyRxData( cantp_com_rx_or_copy[rxNSduConfig->CanTpRxNSduId],\
                                                        CanTpPduData,  \
                                                        &rxRuntimeParam->Buffersize );

                switch(CopyData_Status)
                {
                case BUFREQ_OK :
                    rxRuntimeParam->framesHandledCount++; /* Number received Consecutive Frame */

                    rxRuntimeParam->nextFlowControlCount--; /* Decreament number of cf need to receive */

                    /* TODO???????? can use  if( rxRuntimeParam->framesHandledCount == rxRuntimeParam->BS ) */
                    if( rxRuntimeParam->nextFlowControlCount == 0 )  /* Last Consecutive Frame */
                    {
                        /* SWS_CanTp_00166] At the reception of a FF or last CF of a block, the CanTp
                                            shall start a time-out N_Br before calling PduR_CanTpStartOfReception
                                            or PduR_CanTpCopyRxData. */
                        /* [SWS_CanTp_00325] If the function PduR_CanTpCopyRxData() called after
                                              reception of the last Consecutive Frame of a block returns BUFREQ_OK,
                                              but the remaining buffer is not sufficient for the reception of the next
                                              block, the CanTp module shall start the timer N_Br. */
                        if( rxRuntimeParam->Buffersize < rxRuntimeParam->bytesRemaining )
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
                        /* [SWS_CanTp_00224] When the Rx buffer is large enough for the next block
                                            (directly after the First Frame or the last Consecutive Frame
                                            of a block, or after repeated calls to PduR_CanTpCopyRxData()
                                            according to SWS_CanTp_00222),the CanTp module shall send
                                            a Flow Control N-PDU with ClearToSend status (FC(CTS)) and
                                            shall then expect the reception of Consecutive Frame N-PDUs.*/
                        else /* Buffer Size in pdur is available */
                        {
                            PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_OK);
                            /* send a Flow Control N-PDU with ClearToSend status  */
                            SendFlowControlFrame( rxNSduConfig, rxRuntimeParam, FLOW_CONTROL_CTS_FRAME);
                        }

                        /* [page 73] Last CF Received: this consecutive frame is the last
                                     (Total length information was, as parameter, in the first frame).
                                      CanTp shall notify PDU Router with PduR_CanTpRxIndication callback.*/
                        // PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_OK);
                    }
                    else /* Consecutive Frame Not the Last one */
                    {
                        /* [SWS_CanTp_00312]  The CanTp module shall start a time-out N_Cr at each indication
                                              of CF reception (except the last one in a block) and at each
                                              confirmation of a FC transmission that initiate a CF transmission
                                              on the sender side (FC with FS=CTS). */

                        rxRuntimeParam->stateTimeoutCount = (rxNSduConfig->CanTpNcr);
                    }

                    break;
                    /*[SWS_CanTp_00271] If the PduR_CanTpCopyRxData() returns BUFREQ_E_NOT_OK
                                        after reception of a Consecutive Frame in a block the CanTp
                                        shall abort the reception of N-SDU and notify the PduR module
                                         by calling the PduR_CanTpRxIndication() with the result E_NOT_OK */
                case BUFREQ_E_NOT_OK :
                    /* Report an PDUR with E_NOT_OK to Abort*/
                    PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);

                    /* End of the reception*/
                    rxRuntimeParam->state = IDLE;
                    rxRuntimeParam->mode = CANTP_RX_WAIT;
                    break;
                default :
                    /* Do Nothing*/
                    break;
                }
            }
            else
            {
                /* Report an PDUR with E_NOT_OK to Abort*/
                PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);

                /* End of the reception*/
                rxRuntimeParam->state = IDLE;
                rxRuntimeParam->mode = CANTP_RX_WAIT;
            }


        }

    }
}

