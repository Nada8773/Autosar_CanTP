

#include "PduR.h"

BufReq_ReturnType PduR_CanTpCopyTxData( PduIdType id,  PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr )
{
	static uint8 count = 0;
	uint8 Array[] = {'I','T','I','_','4','0','_','C','a','N','T','p','_','T','e','a','m'};

	if (count == 0)
	{
		info->SduDataPtr = Array;
		*availableDataPtr = 17 - info->SduLength ;
	}

	if (count == 1)
	{
		info->SduDataPtr = &Array[6];
		*availableDataPtr = 11 - info->SduLength ;

	}

	if (count == 2)
	{
		info->SduDataPtr = &Array[13];
		*availableDataPtr = 4 - info->SduLength ;

	}

	count++;
	return BUFREQ_OK;
}

void PduR_CanTpTxConfirmation(PduIdType CanTpTxPduId, Std_ReturnType result)
{

}

BufReq_ReturnType PduR_CanTpStartOfReception(PduIdType id,PduInfoType *info,PduLengthType length,PduLengthType *Buffersize)
{
    *Buffersize = 40;          // any number != 0
    return BUFREQ_OK;
}

BufReq_ReturnType PduR_CanTpCopyRxData(PduIdType id,PduInfoType *info,PduLengthType *Buffersize)
{

    *Buffersize = *Buffersize - info->SduLength;

    return BUFREQ_OK;
}

void PduR_CanTpRxIndication(PduIdType CanTpRxPduId,uint8 Result)
{

}