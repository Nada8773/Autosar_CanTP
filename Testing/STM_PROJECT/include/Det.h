 /*******************************************************************************************
 *                                                                                          *
 * File Name   : Det.h                                                                      *
 *                                                                                          *
 * Author      : ITI AUTOSAR CanTP Team                                                     *
 *                                                                                          *
 * Platform    : TivaC                                                                      *
 *                                                                                          *
 * Date        : 6 Jun 2020                                                                 *
 *                                                                                          *
 * Version     : 4.0.0                                                                      *
 *                                                                                          *
 * Description : specifies the AUTOSAR Default Error Tracer(DET) header file Release 4.3.1  *
 *                                                                                          *
 ********************************************************************************************/

#ifndef DET_H_
#define DET_H_

#include "Std_Types.h"

/**************************** Functions Prototype ******************************************/

Std_ReturnType Det_ReportError(uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId);


#endif /* DET_H_ */
