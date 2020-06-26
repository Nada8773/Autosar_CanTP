#include "CanIf.h"
#include "CanIf_Lcfg.h"
#include "Canif_cfg.h"

VAR(CanIfRxPduCfg_s ,AUTOMATIC) CanIfRxPduCfg[MAX_NUM_RX_PDU] = {
                                                                 {
                                                                  .CanIfRxPduCanIdType = STANDARD_CAN,
                                                                  .CanIfRxPduCanId      = 1           ,
                                                                  .CanIfRxPduDataLength = 4           ,
                                                                  .CanIfRxPduUserRxIndicationName=NULL_PTR,
                                                                  .CanIfRxPduUserRxIndicationUL = PDUR,
                                                                  .CanIfRxPduHrhIdRef = 0             ,
                                                                 },
                                                                 {
                                                                  .CanIfRxPduCanIdType = STANDARD_CAN,
                                                                  .CanIfRxPduCanId = 2 ,
                                                                  .CanIfRxPduDataLength = 8,
                                                                  .CanIfRxPduUserRxIndicationName=NULL_PTR,
                                                                  .CanIfRxPduUserRxIndicationUL = CAN_TP,
                                                                  .CanIfRxPduHrhIdRef = 1,
                                                                 },
                                                                 {
                                                                  .CanIfRxPduCanIdType = STANDARD_CAN,
                                                                  .CanIfRxPduCanId = 3,
                                                                  .CanIfRxPduDataLength = 8,
                                                                  .CanIfRxPduUserRxIndicationName=NULL_PTR,
                                                                  .CanIfRxPduUserRxIndicationUL = PDUR,
                                                                  .CanIfRxPduHrhIdRef = 2,
                                                                 },
                                                                 {
                                                                  .CanIfRxPduCanIdType = STANDARD_CAN,
                                                                  .CanIfRxPduCanId = 4,
                                                                  .CanIfRxPduDataLength = 8,
                                                                  .CanIfRxPduUserRxIndicationName=NULL_PTR,
                                                                  .CanIfRxPduUserRxIndicationUL = CAN_TP,
                                                                  .CanIfRxPduHrhIdRef = 3,
                                                                 }
};

VAR(CanIfTxPduCfg_s ,AUTOMATIC) CanIfTxPduCfg[MAX_NUM_TX_PDU] = {
                                                                 {
                                                                  .CanIfTxPduCanId = 1,
                                                                  .CanIfTxPduCanIdType = STANDARD_CAN,
                                                                  .CanIfTxPduUserTxConfirmationUL = CAN_TP,
                                                                  .CanIfHthCfgRef = 0
                                                                 },
                                                                 {
                                                                  .CanIfTxPduCanId = 2,
                                                                  .CanIfTxPduCanIdType = STANDARD_CAN,
                                                                  .CanIfTxPduUserTxConfirmationUL = PDUR,
                                                                  .CanIfHthCfgRef = 1
                                                                 },
                                                                 {
                                                                  .CanIfTxPduCanId = 3,
                                                                  .CanIfTxPduCanIdType = STANDARD_CAN,
                                                                  .CanIfTxPduUserTxConfirmationUL = CAN_TP,
                                                                  .CanIfHthCfgRef = 2
                                                                 },
                                                                 {
                                                                  .CanIfTxPduCanId = 4,
                                                                  .CanIfTxPduCanIdType = STANDARD_CAN,
                                                                  .CanIfTxPduUserTxConfirmationUL = PDUR,
                                                                  .CanIfHthCfgRef = 3
                                                                 }

};



VAR(CanIfHrhCfg_s ,AUTOMATIC) CanIfHrhCfg[MAX_NUM_HRH]={
                                                        {

                                                         .CanIfHrhCanCtrlIdRef = 0,
                                                         .CanIfHrhIdSymRef     = 4
                                                        },
                                                        {
                                                         .CanIfHrhCanCtrlIdRef = 1,
                                                         .CanIfHrhIdSymRef     = 5
                                                        },
                                                        {
                                                         .CanIfHrhCanCtrlIdRef = 2,
                                                         .CanIfHrhIdSymRef     = 6
                                                        },
                                                        {
                                                         .CanIfHrhCanCtrlIdRef = 4,
                                                         .CanIfHrhIdSymRef     = 7
                                                        }
};



VAR(CanIfHthCfg_s ,AUTOMATIC) CanIfHthCfg[MAX_NUM_HTH] = {
                                                          {
                                                           .CanIfHthCanCtrlIdRef = 3,
                                                           .CanIfHthIdSymRef = 0
                                                          },
                                                          {
                                                           .CanIfHthCanCtrlIdRef = 3,
                                                           .CanIfHthIdSymRef = 1
                                                          },
                                                          {
                                                           .CanIfHthCanCtrlIdRef = 3,
                                                           .CanIfHthIdSymRef = 2
                                                          },
                                                          {
                                                           .CanIfHthCanCtrlIdRef = 3,
                                                           .CanIfHthIdSymRef = 3
                                                          }
};


VAR(CanIfCtrlCfg_s,AUTOMATIC) CanIfCtrlCfg[MAX_CANIF_CTRL] =
                                           {
    {
      .CanIfCtrlId = 0,
      .CanIfCtrlCanCtrlRef = 1,
    },
    {
     .CanIfCtrlId = 1,
     .CanIfCtrlCanCtrlRef = 1,
    },
    {
     .CanIfCtrlId = 2,
     .CanIfCtrlCanCtrlRef = 1,
    },
    {
     .CanIfCtrlId = 3,
     .CanIfCtrlCanCtrlRef = 1,
    },
    {
     .CanIfCtrlId = 4,
     .CanIfCtrlCanCtrlRef = 1,
    }
                 };
