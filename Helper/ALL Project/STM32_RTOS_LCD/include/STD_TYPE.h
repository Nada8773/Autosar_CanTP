
#ifndef STD_TYPE_H
#define STD_TYPE_H

typedef  unsigned char           u8 ;
typedef  unsigned short int      u16;
typedef  unsigned long int       u32;

typedef  signed char             s8 ;
typedef  signed short int        s16;
typedef  signed long int         s32;

typedef  float                   f32;
typedef  double                  f64;
typedef  long double             f96;


#define  ErrorStatus            u8
#define  STD_TYPES_ERROR_OK     (ErrorStatus)1
#define  STD_TYPES_ERROR_NOK    (ErrorStatus)2


/*typedef u8          Std_ReturnType;

#define E_OK        (Std_ReturnType)0
#define E_NOT_OK    (Std_ReturnType)1
#define E_BUSY      (Std_ReturnType)2*/
#define NULL ((void *)0)

#endif


