#include "PduR.h"
#include "PduR_types.h"
#include "PduR_Cfg.h"
#include "PduR_Lcfg.h"
#include "ComStack_Types.h"




#define		IS_VALID_PDUID(TxPduId)		(TxPduId < MAX_NUM_ROUTES)
#define		IS_VALID_RESULT(result)		(result == E_OK || result == E_NOT_OK)

#define 	PDUR_INDICATION_CANIF		0
#define 	PDUR_INDICATION_CANTP		1





P2FUNC(void, PDUR_CODE, Gen_FunctionsTransmitLookUpTable[NUM_OF_MODULES]) (PduIdType TxPduId, Std_ReturnType result);
P2FUNC(void, PDUR_CODE, Gen_FunctionsRxIndicationLookUpTable[NUM_OF_MODULES]) (PduIdType TxPduId, Std_ReturnType result);
P2FUNC(void, PDUR_CODE, Gen_FunctionsTxConfirmationLookUpTable[NUM_OF_MODULES]) (PduIdType TxPduId, Std_ReturnType result);
P2FUNC(void, PDUR_CODE, Gen_FunctionsTxCopyLookUpTable[NUM_OF_MODULES]) (PduIdType TxPduId, Std_ReturnType result);
P2FUNC(void, PDUR_CODE, Gen_FunctionsRxCopyLookUpTable[NUM_OF_MODULES]) (PduIdType TxPduId, Std_ReturnType result);

/*PduRDestPdu container*/
/* PduRDestPduDataProvision not emplemented (non TP gateway)
 * always equals to PDUR_DIRECT
 */
/*PduRDestPduHandleId is the index*/
/*PduRTransmissionConfirmation (not needed , check BSWm config)*/



#if 0
PduRDestPduRefType PduRDestPduRef [NUMBER_OF_DEST]; /*macros for modules*/
/*PduRRoutingPathGroup container*/
/*id is index*/
boolean PduRIsEnabledAtInit[NUMBER_OF_GROUPS];
#define PDUR_CONFIGURATION_ID	0
#endif




#define COM_BSW  0
#if 0

static handelId_type globalModuleId[NUMBER_OF_PATHs][NUMBER_OF_MODULES]; /*#CAN_IF 0 #CAN_TP 1 #COM 2*/

void PduR_Init( const PduR_PBConfigType* ConfigPtr )
{
    for(PduId)
    {
        for(destinations)
        {
            for(module_pdur_lookuptable)
            {
                if(module_pdur_lookuptable[i]==pdur_pduId)
                    globalModuleId[PduId][destination]=moduleId;
            }
        }
    }
}


void PduR_EnableRouting( PduR_RoutingPathGroupIdType id )
{
}

void PduR_DisableRouting( PduR_RoutingPathGroupIdType id, boolean initialize )
{

}


Std_ReturnType PduR_ComTransmit( PduIdType TxPduId, const PduInfoType* PduInfoPtr )
{
    PduR_GeneralTransmit[config[TxPduId].dest](globalModuleId[TxPduId][[TxPduId].dest],PduInfoPtr);
}/*same goes for all userUp*/



#endif




void PDUr_TxConfirmation(PduIdType TxPduId , Std_ReturnType result)
{
    VAR(uint8,AUTOMATIC) route = 0;
    if( IS_VALID_PDUID(TxPduId) )
    {
        if(IS_VALID_RESULT(result))
        {
     //       Com_TxConfirmation(,result)
        }
        else
        {

        }
    }
    else
    {

    }
}

void PduR_CanIfRxIndication(PduIdType RxPduId,const PduInfoType* PduInfoPtr)
{
    VAR(uint8,AUTOMATIC) route = 0;

    if( IS_VALID_PDUID(RxPduId) )
    {
        if(PduRRoutingPath[RxPduId].PduRSrcPduRRef == PDUR_INDICATION_CANIF)
        {



        }
        else
        {

        }
    }
    else
    {

    }
}


