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
#include "CanTp_Type.h"


/* Private Function Prototype */
static void ReceiveSingleFrame(const CanTpRxNSdu_s *rxConfig, RunTimeInfo_s *rxRuntime, const PduInfoType *rxPduData);
static void ReceiveFirstFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, const PduInfoType *CanTpPduData);
static void ReceiveConsecutiveFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, const PduInfoType *CanTpPduData);
static void SendFlowControlFrame(const CanTpRxNSdu_s *rxNSduConfig, RunTimeInfo_s *rxRuntimeParam, BufReq_ReturnType flowControlStatus);
static void SetPaddingValue( uint8* Local_NSduData, uint8 Start_IndexCount, uint8 CanTp_PaddingByte);


void CanTP_StartNewRx(RunTimeInfo_s *rxRuntimeParam)
{
    rxRuntimeParam->state           = IDLE          ;
    rxRuntimeParam->mode            = CANTP_RX_WAIT ;
    rxRuntimeParam->transferTotal   = 0; // ->  rxRuntime->TotalNSduLength = 0;
    rxRuntimeParam->Buffersize      = 0;
    rxRuntimeParam->CanTpRxWftCount = 0;
    /*rxRuntime->NasNarPending = FALSE;
    rxRuntime->framesHandledCount = 0;
    rxRuntime->nextFlowControlCount = 0;
    rxRuntime->pdurBufferCount = 0;
    rxRuntime->TotalNSduLength = 0;
    rxRuntime->transferCount = 0;
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


    /*********************************** TODO Add that to RXindication *************************************************************/
    /* [SWS_CanTp_00345] If frames with a payload <= 8 (either CAN 2.0 frames or small CAN FD frames)
                        are used for a Rx N-SDU and CanTpRxPaddingActivation is equal to CANTP_ON,
                        then CanTp receives by means of CanTp_RxIndication() call an SF Rx N-PDU belonging
                        to that N-SDU, with a length smaller than eight bytes (i.e. PduInfoPtr.SduLength < 8),
                        CanTp shall reject the reception. The runtime error code CANTP_E_PADDING shall be
                        reported to the Default Error Tracer */
    if( (CanTpPduData->SduLength < 8) && (rxNSduConfig->CanTpTxPaddingActivation == CANTP_ON) )
    {
        /*  reject the reception */
        /* Report Det */
    }
    else
    {
        /* Call ReceiveSingleFrame() ; */
    }

    /****************************************************************************************************************************/


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



    /* TODO add to struct */
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

    if ( rxRuntimeParam->Buffersize == 0 )
    {
        /* [SWS_CanTp_00079] When receiving an SF or an FF N-PDU, the CanTp module
             shall notify the upper layer (PDU Router) about this reception using the
             PduR_CanTpStartOfReception function */
        /* TODO Change ID */
        BufferRequest_Status = PduR_CanTpStartOfReception( ID_IPDU, &rxPduData, rxRuntimeParam->transferTotal,  \
                                                           &rxRuntimeParam->Buffersize );
    }

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

    BufferRequest_Status = PduR_CanTpStartOfReception( ID_IPDU, &rxPduData,  rxRuntimeParam->transferTotal,  \
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
                if( rxRuntimeParam->Buffersize < (rxRuntimeParam->transferTotal - rxPduData->SduLength) )
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

        if( Local_BSValue > rxNSduConfig->CanTpBs )
        {
            Local_BSValue = rxNSduConfig->CanTpBs ;
        }
        rxRuntimeParam->BS                               = Local_BSValue ;
        rxRuntimeParam->nextFlowControlCount             = Local_BSValue ; /* Why ?? Remove it !! */

        txFlowControlData.SduDataPtr[Local_IndexCount++] = rxRuntimeParam->BS    ; /* Block Size */
        txFlowControlData.SduDataPtr[Local_IndexCount++] = rxRuntimeParam->STmin ;

        txFlowControlData.SduLength                      = Local_IndexCount;
        break;

    case FLOW_CONTROL_WAIT_FRAME:

        /*[SWS_CanTp_00315] The CanTp module shall start a timeout observation for N_Bs time
                             at confirmation of the FF transmission, last CF of a block transmission and
                             at each indication of FC with FS=WT (i.e. time until reception of the next FC) */
        rxRuntimeParam->stateTimeoutCount                = (rxNSduConfig->CanTpNbs);
        txFlowControlData.SduDataPtr[Local_IndexCount++] = ISO15765_TPCI_FC | ISO15765_FLOW_CONTROL_STATUS_WAIT;
        txFlowControlData.SduLength                      = Local_IndexCount;

        /************************** TODO Add to to Receive FC ***********************************************/
        /*  [SWS_CanTp_00223] The CanTp module shall send a maximum of WFTmax
                             consecutive FC(WAIT) N-PDU. If this number is reached,
                             the CanTp module shall abort the reception of this N-SDU
                             (the receiver did not send any FC N-PDU, so the N_Bs timer
                             expires on the sender side and then the transmission is aborted)
                             and a receiving indication with E_NOT_OK occurs */
        /* ISO-15765-2 -> WFTmax shall only be used on the receiving network
                          entity during message reception
                       -> if the Value is set to zero then flow control shall relely upon
                          flow control continue to send FC CTS only and FC  wait shall not be used*/

        /* if( (rxNSduConfig->CanTpRxWftMax < rxRuntimeParam->CanTpRxWftCount) && (rxNSduConfig->CanTpRxWftMax !=0) )
        {
        }*/

        /**************************************************************************************************************/

        break;

    case FLOW_CONTROL_OVERFLOW_FRAME:
        /* Overflow (OVFLW) -> it cause  the sender to abort the transmission of a segmented
                               message */
        txFlowControlData.SduDataPtr[Local_IndexCount++] = ISO15765_TPCI_FC | ISO15765_FLOW_CONTROL_STATUS_OVFLW;
        txFlowControlData.SduLength                      = Local_IndexCount;
        break;
    }

    /*TODO ADD Macro For CanTpPaddingByte */
    /*[SWS_CanTp_00347]  If CanTpRxPaddingActivation is equal to CANTP_ON for  an Rx N-SDU,
                         the CanTp module shall transmit FC N-PDUs with a length of eight
                         bytes. Unused bytes in N-PDU shall be updated with CANTP_PADDING_BYTE*/
#if rxNSduConfig->CanTpTxPaddingActivation == CANTP_ON
    SetPaddingValue( &Local_NSduData[0], Local_IndexCount, CANTP_PADDING_BYTE);
    txFlowControlData.SduLength = MAX_FRAME_BYTES ;
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
static void SetPaddingValue( uint8* NSduData, uint8 Start_IndexCount, uint8 CanTp_PaddingByte)
{
    uint8 Local_IndexCount ;
    for(Local_IndexCount = Start_IndexCount ; Local_IndexCount < MAX_FRAME_BYTES ; Local_IndexCount++)
    {
        NSduData[Local_IndexCount] = CanTp_PaddingByte ;
    }
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
                    if( rxRuntimeParam->nextFlowControlCount == 0 ) /* Last Consecutive Frame */
                    {
                        /* SWS_CanTp_00166] At the reception of a FF or last CF of a block, the CanTp
                                            shall start a time-out N_Br before calling PduR_CanTpStartOfReception
                                            or PduR_CanTpCopyRxData. */
                        rxRuntimeParam->stateTimeoutCount = (rxNSduConfig->CanTpNbr);

                        /* [page 37] Last CF Received: this consecutive frame is the last
                                     (Total length information was, as parameter, in the first frame).
                                      CanTp shall notify PDU Router with sPduR_CanTpRxIndication callback.*/
                        PduR_CanTpRxIndication(rxNSduConfig->CanTpRxNPduId, E_OK);


                        /* [SWS_CanTp_00224] When the Rx buffer is large enough for the next block
                                            (directly after the First Frame or the last Consecutive Frame
                                            of a block, or after repeated calls to PduR_CanTpCopyRxData()
                                            according to SWS_CanTp_00222),the CanTp module shall send
                                            a Flow Control N-PDU with ClearToSend status (FC(CTS)) and
                                            shall then expect the reception of Consecutive Frame N-PDUs.*/

                        if( rxRuntimeParam->Buffersize != 0 ) /* There's Block Size need To be Received */
                        {
                            /* send a Flow Control N-PDU with ClearToSend status  */
                            SendFlowControlFrame( rxNSduConfig, rxRuntimeParam, FLOW_CONTROL_CTS_FRAME);
                        }
                        else /* End of Reception */
                        {
                            PduR_CanTpRxIndication(p_n_sdu->rx.cfg->nSduId, E_OK);
                            rxRuntimeParam->state = IDLE;
                            rxRuntimeParam->mode = CANTP_RX_WAIT;
                        }


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

