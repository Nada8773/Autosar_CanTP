 /****************************************************************
 Author  : Nada Mohamed 
 Data    : 4 Feb 2020
 Version : v01
 ******************************************************************/
 
 #ifndef STK_INTERFACE_H
 #define STK_INTERFACE_H

void STK_voidInitialize(void);
void STK_voidStart(u32 Copy_u32Value);
void STK_voidSetCallBack(void(*Copy_ptr)(void));
void  STK_voidBusyDelay(u32 Copy_microseconds); /* with frequency 8 only   8/8 = 1Mhz*/

#endif
