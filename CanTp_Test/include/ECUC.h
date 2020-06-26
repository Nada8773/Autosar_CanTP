#ifndef CONFIGURATION_ECUC_H_
#define CONFIGURATION_ECUC_H_

#include "CanIf_Cfg.h"
#include "CanTp_Cfg.h"
#include "Com_Cfg.h"



extern VAR( PduIdType,mem_class ) CanIf_UpperLayerUser_LookUpTable_Rx[ MAX_NUM_RX_PDU ] ;
extern VAR( PduIdType,mem_class ) CanIf_UpperLayerUser_LookUpTable_Tx[MAX_NUM_TX_PDU  ] ;
extern VAR( PduIdType,mem_class ) CanTp_LowerLayerUser_LookUpTable_Tx[NUMBER_OF_TXNPDU] ;
extern VAR( PduIdType,mem_class ) CanTp_LowerLayerUser_LookUpTable_FC[NUMBER_OF_RXNPDU] ;
extern VAR( PduIdType,mem_class ) CanTp_UpperLayerUser_LookUpTable_Tx[COM_NUM_OF_IPDU]  ;
extern VAR( PduIdType,mem_class ) CanTp_UpperLayerUser_LookUpTable_Rx[COM_NUM_OF_IPDU]  ;
extern VAR( PduIdType,mem_class ) Pdur_LowerLayerUser_LookUpTable[COM_NUM_OF_IPDU]      ;



#endif /* CONFIGURATION_ECUC_H_ */
