
#ifndef PDUR_H_
#define PDUR_H_

BufReq_ReturnType PduR_CanTpCopyTxData( PduIdType id,  PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr );
void PduR_CanTpTxConfirmation(PduIdType CanTpTxPduId, Std_ReturnType result);
BufReq_ReturnType PduR_CanTpStartOfReception(PduIdType id,PduInfoType *info,PduLengthType length,PduLengthType *Buffersize);
BufReq_ReturnType PduR_CanTpCopyRxData(PduIdType id,PduInfoType *info,PduLengthType *Buffersize);
void PduR_CanTpRxIndication(PduIdType CanTpRxPduId,uint8 Result);

#endif /* PDUR_H_ */