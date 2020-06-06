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

#define SEGMENT_NUMBER_MASK                                                  (uint8)0x0F

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

typedef uint8 ChannelModeType;
#define CANTP_MODE_FULL_DUPLEX                                     (ChannelModeType)0x00
#define CANTP_MODE_HALF_DUPLEX                                     (ChannelModeType)0x01

#if 0
typedef uint8 TaType;
#define CANTP_FUNCTIONAL                                                    (TaType)0x00
#define CANTP_PHYSICAL                                                      (TaType)0x01
#endif

#if 0
typedef uint8 AddressingFormatType;
#define CANTP_EXTENDED                                        (AddressingFormatType)0x00
#define CANTP_MIXED                                           (AddressingFormatType)0x01
#define CANTP_MIXED29BIT                                      (AddressingFormatType)0x02
#define CANTP_NORMALFIXED                                     (AddressingFormatType)0x03
#define CANTP_STANDARD                                        (AddressingFormatType)0x04
#endif

#define ISO15765_TPCI_MASK      		                                     (uint8)0x30
#define ISO15765_TPCI_SF        		                                     (uint8)0x00  /* Single Frame */
#define ISO15765_TPCI_FF        		                                     (uint8)0x10  /* First Frame */
#define ISO15765_TPCI_CF        		                                     (uint8)0x20  /* Consecutive Frame */
#define ISO15765_TPCI_FC        		                                     (uint8)0x30  /* Flow Control */
#define ISO15765_TPCI_DL        		                                     (uint8)0x07  /* Single frame data length mask */

#define ISO15765_TPCI_FS_MASK   			                                 (uint8)0x0F  /* FlowControl status mask */
#define ISO15765_FLOW_CONTROL_STATUS_CTS                                     (uint8)0x00  /* FC Clear/Continue To Send Status */
#define ISO15765_FLOW_CONTROL_STATUS_WAIT                                    (uint8)0x01  /* FC Waiting Status */
#define ISO15765_FLOW_CONTROL_STATUS_OVFLW                                   (uint8)0x02  /* FC OverFlows Status */

extern MaxSegSize;

typedef struct
{
    const float64                CanTpNas;
    const float64                CanTpNbs;
    const float64                CanTpNcs;
          boolean                CanTpTc;
          uint16                 CanTpTxNSduId;
    const uint8                  CanTpTxPaddingActivation;
          uint8                  CanTpTxTaType;
          uint16                 CanTpTxNPduConfirmationPduId;
          uint16                 CanTpRxFcNPduId;

          uint8                  CanTpTxChannel;	//Should be removed and channel list is created instead
}CanTpTxNSdu;

typedef struct
{
	const uint8                  CanTpBs ;
	const float64                CanTpSTmin;
	const uint16                 CanTpRxWftMax;
    const float64                CanTpNar;
    const float64                CanTpNbr;
    const float64                CanTpNcr;
          uint16                 CanTpRxNSduId;
          uint16                 CanTpRxNPduId;
    const uint8                  CanTpRxPaddingActivation;
          uint8                  CanTpRxTaType;
	      uint16                 CanTpTxFcNPduConfirmationPduId;
          uint16                 CanTpRxFcNPduId;

          uint8                  CanTpRxChannel;  //Should be removed and channel list is created instead
}CanTpRxNSduType;

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
	PduLengthType                 IFdata[MaxSegSize];
	CanTp_TransferInstanceMode    mode;
	uint32                        Buffersize;

}RunTimeInfo;


/**************************** Function-Like Macros ******************************************/

#define TIMER_DECREMENT(timer) \
        if (timer > 0) { \
            timer = timer - 1; \
        } \

#define COUNT_DECREMENT(count) \
        if (count > 0) { \
            count = count - 1; \
        } \

/**************************** AUTOSAR-Compliant APIs' Prototypes ******************************************/

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
