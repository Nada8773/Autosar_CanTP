#ifndef PDUR_TYPES_H
#define PDUR_TYPES_H


typedef uint8 PduRSrcPduRRefType ;
typedef uint8 PduRDestPduRRefType;

typedef struct
{
    PduRSrcPduRRefType          PduRSrcPduRRef;      /*index*/
    PduRDestPduRRefType         PduRDestPduRRef;    /*index We support only one destination*/

}PduRRoutingPath_s;


#endif

