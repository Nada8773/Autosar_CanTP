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

#ifndef CanFD
#define MAX_SEGMENT_DATA_SIZE       8
#else   MAX_SEGMENT_DATA_SIZE      64
#endif

typedef uint8 CanTp_TransferInstanceMode;
#define CANTP_RX_WAIT				                    (CanTp_TransferInstanceMode)0x00
#define CANTP_RX_PROCESSING                             (CanTp_TransferInstanceMode)0x01
#define CANTP_TX_WAIT                                   (CanTp_TransferInstanceMode)0x02
#define CANTP_TX_PROCESSING                             (CanTp_TransferInstanceMode)0x03

typedef uint8 FrameType;
#define INVALID_FRAME                                                    (FrameType)0x00
#define SINGLE_FRAME                                                     (FrameType)0x01
#define FIRST_FRAME                                                      (FrameType)0x02
#define CONSECUTIVE_FRAME                                                (FrameType)0x03
#define FLOW_CONTROL_CTS_FRAME                                           (FrameType)0x04
#define FLOW_CONTROL_WAIT_FRAME                                          (FrameType)0x05
#define FLOW_CONTROL_OVERFLOW_FRAME                                      (FrameType)0x06

typedef uint8 TransferStateTypes;
#define UNINITIALIZED                                           (TransferStateTypes)0x00
#define IDLE                                                    (TransferStateTypes)0x01
#define SF_OR_FF_RECEIVED_WAITING_PDUR_BUFFER                   (TransferStateTypes)0x02
#define RX_WAIT_CONSECUTIVE_FRAME                               (TransferStateTypes)0x03
#define RX_WAIT_SDU_BUFFER                                      (TransferStateTypes)0x04
#define TX_WAIT_STMIN                                           (TransferStateTypes)0x05
#define TX_WAIT_TRANSMIT										(TransferStateTypes)0x06
#define TX_WAIT_FLOW_CONTROL									(TransferStateTypes)0x07
#define TX_WAIT_TX_CONFIRMATION									(TransferStateTypes)0x08

typedef uint8 AddressingFormatType;
#define CANTP_EXTENDED                                        (AddressingFormatType)0x00
#define CANTP_MIXED                                           (AddressingFormatType)0x01
#define CANTP_MIXED29BIT                                      (AddressingFormatType)0x02
#define CANTP_NORMALFIXED                                     (AddressingFormatType)0x03
#define CANTP_STANDARD                                        (AddressingFormatType)0x04

typedef uint8 ChannelModeType;
#define CANTP_MODE_FULL_DUPLEX                                     (ChannelModeType)0x00
#define CANTP_MODE_HALF_DUPLEX                                     (ChannelModeType)0x01

typedef uint8 TaType;
#define CANTP_FUNCTIONAL
#define CANTP_PHYSICAL


typedef struct
{
    const float64 CanTpNas;
    const float64 CanTpNbs;
    const float64 CanTpNcs;
          boolean CanTpTc;
          uint8   CanTpTxAddressingFormat;
          uint16  CanTpTxNSduId;
    const uint8   CanTpTxPaddingActivation;
          uint8   CanTpTxTaType;
          uint16  CanTpTxNPduConfirmationPduId;
          uint16  CanTpRxFcNPduId;

          uint8  CanTpTxChannel;	//Should be removed and channel list is created instead
}CanTpTxNSdu;

typedef struct
{
	const uint8   CanTpBs ;
	const float64 CanTpSTmin;
	const uint16  CanTpRxWftMax;
    const float64 CanTpNar;
    const float64 CanTpNbr;
    const float64 CanTpNcr;
          uint8   CanTpTxAddressingFormat;
          uint16  CanTpRxNSduId;
          uint16  CanTpRxNPduId;
    const uint8   CanTpRxPaddingActivation;
          uint8   CanTpRxTaType;
	      uint16  CanTpTxFcNPduConfirmationPduId;
          uint16  CanTpRxFcNPduId;

          uint8   CanTpRxChannel;  //Should be removed and channel list is created instead
}CanTp_RXNSduType;

typedef struct
{
	boolean                       initRun;
	uint8                         internalState;
	uint16                        nextFlowControlCount;
	uint16                        framesHandledCount;
	uint32                        stateTimeoutCount;
	uint8                         STmin;
	uint8                         BS;
	boolean                       NasNarPending;
	uint32                        NasNarTimeoutCount;
	TransferStateTypes            state;
	uint32                        pdurBufferCount;  //i see no significance for this parameter
	uint32                        transferTotal;
	uint32                        transferCount;
	uint32                        availableDataSize;
	PduLengthType                 IFByteCount;
	PduLengthType                 IFdata[MAX_SEGMENT_DATA_SIZE];
	CanTp_TransferInstanceMode    mode;
	uint32                        Buffersize;

}RunTimeInfo;

/**************************** Functions Prototype ******************************************/

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
