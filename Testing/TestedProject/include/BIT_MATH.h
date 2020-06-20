
 /****************************************************************
 * Author  : Nada Mohamed 
 * Data    : 18 December ‎2019
 * Version : v01
 *****************************************************************/


#ifndef BIT_MATH_H
#define BIT_MATH_H

#define SET_BIT(Var,Bit_No)       Var |=(1<<Bit_No)
#define CLR_BIT(Var,Bit_No)       Var &=~(1<<Bit_No)
#define TOGGLE_BIT(Var,Bit_No)    Var ^=(1<<Bit_No)
#define GET_BIT(Var,Bit_No)      ((Var >>Bit_No)&1)
#define ASSIGN_BIT(VAR ,BIT_NO ,VAL)   (VAR=VAR &~(1<<BIT_NO)) |(VAL<<BIT_NO)
#endif
