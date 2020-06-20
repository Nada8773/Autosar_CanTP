#include "Platform_Types.h"
#include "Compiler.h"
#include "Std_Types.h"
#include "ComStack_Types.h"

#include "CanTp_Cbk.h"
#include "CanIf.h"

Std_ReturnType CanIf_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr)
{

    CanTp_TxConfirmation(1,E_OK);
    CanTp_RxIndication(1,PduInfoPtr);
    return E_OK;
}
