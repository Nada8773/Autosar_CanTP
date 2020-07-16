#ifndef COMMUNICATIONSTACK_PDUR_INCLUDE_PDUR_CBK_H_
#define COMMUNICATIONSTACK_PDUR_INCLUDE_PDUR_CBK_H_

#include "ComStack_Types.h"

BufReq_ReturnType PduR_CanTpCopyTxData( PduIdType id,  PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr );
BufReq_ReturnType PduR_CanTpStartOfReception(PduIdType id,PduInfoType *info,PduLengthType length,PduLengthType *bufferSizePtr);
BufReq_ReturnType PduR_CanTpCopyRxData(PduIdType id,PduInfoType *info,PduLengthType *bufferSizePtr);
void PduR_CanTpTxConfirmation(PduIdType CanTpTxPduId, Std_ReturnType result);
void PduR_CanTpRxIndication(PduIdType CanTpRxPduId,Std_ReturnType result);



void PduR_CanIfTxConfirmation(PduIdType CanTpTxPduId, Std_ReturnType result);
void PduR_CanIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr);





#endif /* COMMUNICATIONSTACK_PDUR_INCLUDE_PDUR_CBK_H_ */
