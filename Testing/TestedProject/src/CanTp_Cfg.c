
#include "Platform_Types.h"
#include "Compiler.h"
#include "Std_Types.h"


#include "ComStack_Types.h"
#include "CanTp_Types.h"
#include "CanTp_Cfg.h"

const CanTpTxNSdu_s CanTpTxNSdu[] =
{
 {
  .CanTpNas 			      = 				8,
  .CanTpNbs 				  = 				9,
  .CanTpNcs 				  = 				10,
  .CanTpTc			      = 				TRUE,
  .CanTpTxAddressingFormat  =  			    CANTP_STANDARD,
  .CanTpTxNSduId 			  =					0,
  .CanTpTxPaddingActivation = 				CANTP_OFF,
  .CanTpTxTaType 			  = 				CANTP_PHYSICAL,


 },

 {
  .CanTpNas				  =					 8,
  .CanTpNbs 				  =					 9,
  .CanTpNcs 				  =					 10,
  .CanTpTc				  =					 TRUE,
  .CanTpTxAddressingFormat  =  				 CANTP_STANDARD,
  .CanTpTxNSduId            =					 1,
  .CanTpTxPaddingActivation =  				 CANTP_OFF,
  .CanTpTxTaType 			  =					 CANTP_PHYSICAL,


 },

};


const CanTpRxNSdu_s CanTpRxNSdu[] =
{
 {
  //.CanTpRxNSdu				 = 				 0,    // why is this written here ??? do you have any idea ??
  .CanTpBs					 =				 10,
  .CanTpNar					 = 				10,
  .CanTpNbr					 = 				20,
  .CanTpNcr 					 = 				30,
  .CanTpRxAddressingFormat 	 = 				CANTP_STANDARD,    // THIS IS AN ENUMERATOR IT CAN HAVE ONE	OF THE FOLLOWING VALUES {CANTP_EXTENDED , CANTP_MIXED , CANTP_MIXED29BIT , CANTP_NORMALFIXED ,  CANTP_STANDARD}
  .CanTpRxNSduId 				 =			 	0,     //  id =0
  .CanTpRxPaddingActivation 	 =				CANTP_OFF,		 // THIS IS AN ENUMERATOR IT AN HAVE ONEOF THE FOLLOWING VALUES {CANTP_OFF, CANTP_ON}
  .CanTpRxTaType				 =				CANTP_PHYSICAL,            // you can put either one of these values on the ENUM {CANTP_FUNCTIONAL, CANTP_PHYSICAL}
  .CanTpRxWftMax				 = 				5,
  .CanTpSTmin					 = 				TRUE,


 },

 {
  //.CanTpRxNSdu 				=				 0,    // why is this written here ??? do you have any idea ??
  .CanTpBs 					= 				 20,
  .CanTpNar 					=				3,
  .CanTpNbr				    = 				4,
  .CanTpNcr					= 				6,
  .CanTpRxAddressingFormat    =  				CANTP_STANDARD,    // THIS IS AN ENUMERATOR IT CAN HAVE ONE	OF THE FOLLOWING VALUES {CANTP_EXTENDED , CANTP_MIXED , CANTP_MIXED29BIT , CANTP_NORMALFIXED ,  CANTP_STANDARD}
  .CanTpRxNSduId 				= 				1,
  .CanTpRxPaddingActivation   = 				CANTP_ON,		 // THIS IS AN ENUMERATOR IT AN HAVE ONEOF THE FOLLOWING VALUES {CANTP_OFF, CANTP_ON}

  .CanTpRxTaType				= 				CANTP_PHYSICAL,            // you can put either one of these values on the ENUM {CANTP_FUNCTIONAL, CANTP_PHYSICAL}
  .CanTpRxWftMax				= 				5,
  .CanTpSTmin 				= 				TRUE,


 }

};



ChannelInfo_s ChannelInfo[] =
{
 {
  .Mode         	 =			 CANTP_MODE_FULL_DUPLEX,  // it can also take this value (CANTP_MODE_HALF_DUPLEX)
  .StIdx			 = 				 0,
  .Length			 = 				 1,
 },
 {
  .Mode            =           CANTP_MODE_FULL_DUPLEX,  // it can also take this value (CANTP_MODE_HALF_DUPLEX)
  .StIdx           =               1,
  .Length          =               1,
 }
};


