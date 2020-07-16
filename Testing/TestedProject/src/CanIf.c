#include "Platform_Types.h"
#include "Compiler.h"
#include "Std_Types.h"
#include "ComStack_Types.h"

#include "CanTp_Cbk.h"
#include "CanIf.h"


/*
 * The following implementation is just a stub function to debug and test CanTp module
 * that needs using this CanIf API
 */

Std_ReturnType CanIf_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr)
{

    CanTp_TxConfirmation(1,E_OK);
    CanTp_RxIndication(1,PduInfoPtr);
    return E_OK;
}
