
#include "STD_TYPE.h"

typedef u32       ClockSource_ReturnType;

#define ClockSourceType  (u32)

/***********The bit field in RCC_CR Register */



#define RCC_CR_HSI        (u32)0x00000001
#define RCC_CR_HSIRDY     (u32)0x00000002

#define RCC_CR_HSE        (u32)0x00010000
#define RCC_CR_HSERDY     (u32)0x00020000

#define RCC_CR_HSEBYP     (u32)0x00030000
#define RCC_CR_CSSON      (u32)0x00040000

#define RCC_CR_PLL        (u32)0x01000000
#define RCC_CR_PLLRDY     (u32)0x02010000


/***********The bit field in RCC_CFGR Register */

/* System Clock System */
#define RCC_CFGR_SW_HSI         (u32)0x00000000
#define RCC_CFGR_SW_HSE         (u32)0x00000001
#define RCC_CFGR_SW_PLL         (u32)0x00000002

/* System Clock System Status*/
#define RCC_CFGR_SWS_HSI            (u32)0x0
#define RCC_CFGR_SWS_HSE     		(u32)0x4
#define RCC_CFGR_SWS_PLL      		(u32)0x8



/********************   PLL Multiplication Factor */
#define RCC_CFGR_PLLMUL_2             (u32)0x00000000
#define RCC_CFGR_PLLMUL_3        	  (u32)0x00040000
#define RCC_CFGR_PLLMUL_4        	  (u32)0x00080000
#define RCC_CFGR_PLLMUL_5             (u32)0x000C0000
#define RCC_CFGR_PLLMUL_6             (u32)0x00100000
#define RCC_CFGR_PLLMUL_7             (u32)0x00140000
#define RCC_CFGR_PLLMUL_8        	  (u32)0x00180000
#define RCC_CFGR_PLLMUL_9        	  (u32)0x001C0000
#define RCC_CFGR_PLLMUL_10       	  (u32)0x00200000
#define RCC_CFGR_PLLMUL_11       	  (u32)0x00240000
#define RCC_CFGR_PLLMUL_12        	  (u32)0x00280000
#define RCC_CFGR_PLLMUL_13        	  (u32)0x002C0000
#define RCC_CFGR_PLLMUL_14        	  (u32)0x00300000
#define RCC_CFGR_PLLMUL_15        	  (u32)0x00340000
#define RCC_CFGR_PLLMUL_16      	  (u32)0x00380000

/********************   HSE Divider for PLL entry & PLL entry clock source*/
#define RCC_CFGR_PLLXTPRESRC_HSE_divided_1        (u32)0x00010000
#define RCC_CFGR_PLLXTPRESRC_HSE_divided_2        (u32)0x00030000
#define RCC_CFGR_PLLXTPRESRC_HSI_divided_2        (u32)0x00000000

/*********************  PPRE1 APB low-speed prescaler (APB1)   */
#define RCC_CFGR_PPRE1_div_1            (u32)0x00000000
#define RCC_CFGR_PPRE1_div_2            (u32)0x00000400
#define RCC_CFGR_PPRE1_div_4            (u32)0x00000500
#define RCC_CFGR_PPRE1_div_8            (u32)0x00000600
#define RCC_CFGR_PPRE1_div_16           (u32)0x00000700

/*********************  PPRE2 APB high-speed prescaler (APB2)   */
#define RCC_CFGR_PPRE2_div_1 			(u32)0x00000000
#define RCC_CFGR_PPRE2_div_2 			(u32)0x00002000
#define RCC_CFGR_PPRE2_div_4 			(u32)0x00002800
#define RCC_CFGR_PPRE2_div_8			(u32)0x00003000
#define RCC_CFGR_PPRE2_div_16			(u32)0x00003800


/*********************  HPRE AHB prescaler   */
#define RCC_CFGR_HPRE_div_1            (u32)0x00000000
#define RCC_CFGR_HPRE_div_2            (u32)0x00000080
#define RCC_CFGR_HPRE_div_4            (u32)0x00000090
#define RCC_CFGR_HPRE_div_8            (u32)0x000000A0
#define RCC_CFGR_HPRE_div_16           (u32)0x000000B0
#define RCC_CFGR_HPRE_div_64           (u32)0x000000C0
#define RCC_CFGR_HPRE_div_128          (u32)0x000000D0
#define RCC_CFGR_HPRE_div_256          (u32)0x000000E0
#define RCC_CFGR_HPRE_div_512          (u32)0x000000F0

/**********************  ADC prescaler  */
#define RCC_CFGR_ADCPRE_div_2      (u32)0x0
#define RCC_CFGR_ADCPRE_div_4      (u32)0x4000
#define RCC_CFGR_ADCPRE_div_6      (u32)0x8000
#define RCC_CFGR_ADCPRE_div_8      (u32)0xc000


/********************** MCO  Microcontroller clock output */
#define RCC_CFGR_MCO_NOCLK    (u32)0x00000000
#define RCC_CFGR_MCO_SYSCLK   (u32)0x4000000
#define RCC_CFGR_MCO_HSI      (u32)0x5000000
#define RCC_CFGR_MCO_HSE      (u32)0x6000000
#define RCC_CFGR_MCO_PLL      (u32)0x7000000


/************************* APB2ENR **************/
#define RCC_APB2ENR_AFIOEN_Enable      (u32)0x1
#define RCC_APB2ENR_AFIOEN_Disable     (u32)0x0

#define RCC_APB2ENR_IOPAEN_PORTA      (u32)0x4
#define RCC_APB2ENR_IOPBEN_PORTB      (u32)0x8
#define RCC_APB2ENR_IOPCEN_PORTC      (u32)0x10
#define RCC_APB2ENR_IOPDEN_PORTD      (u32)0x20
#define RCC_APB2ENR_IOPEEN_PORTE      (u32)0x40
#define RCC_APB2ENR_IOPFEN_PORTF      (u32)0x80
#define RCC_APB2ENR_IOPGEN_PORTG      (u32)0x100
#define RCC_APB2ENR_ADC1EN      	  (u32)0x200
#define RCC_APB2ENR_ADC2EN      	  (u32)0x400
#define RCC_APB2ENR_TIM1EN      	  (u32)0x800
#define RCC_APB2ENR_SPI1EN      	  (u32)0x1000
#define RCC_APB2ENR_TIM8EN      	  (u32)0x2000
#define RCC_APB2ENR_TIM9EN      	  (u32)0x80000
#define RCC_APB2ENR_TIM10EN     	  (u32)0x100000
#define RCC_APB2ENR_TIM11EN    		  (u32)0x200000
#define RCC_APB2ENR_USART1EN     	  (u32)0x4000
#define RCC_APB2ENR_ADC3EN      	  (u32)0x8000
#define RCC_APB2ENR_ADC3EN     		  (u32)0x8000

/*********************** APB1ENR ****************/
#define RCC_APB1ENR_USART2EN          (u32)0x20000
#define RCC_APB1ENR_USART3EN          (u32)0x40000
#define RCC_APB1ENR_UART4EN           (u32)0x80000
#define RCC_APB1ENR_UART5EN           (u32)0x100000
#define RCC_APB1ENR_CAN1EN            (u32)0x02000000
#define RCC_APB1ENR_CAN2EN            (u32)0x04000000

/********************** AHBENR ******************/
#define RCC_AHBENR_DMA1EN            (u32)0x01
#define RCC_AHBENR_DMA2EN            (u32)0x02

/********************* State ****************/
#define ON  1
#define OFF 0


/*#define Status u32
#define Ok     (Status)1
#define Nok    (Status)0
*/
/************** functions Prototype ************************************************************/
/*  Select System Clock :
 *   RCC_CFGR_SW_HSI
 *   RCC_CFGR_SW_HSE
 *   RCC_CFGR_SW_PLL
 */
extern void Select_SystemClock(u32 SystemClock);

/*Set the clock
 * this function take two argument
 * clock :-
 *    RCC_CR_HSI
 *    RCC_CR_HSE
 *    RCC_CR_PLL
 * State :-
 *      ON  /  OFF
 */
extern void RCC_SetClock(u32 Clock ,u8 State);



/*
 * Return clock source
 * HSI  = RCC_CFGR_SWS_HSI
 * PLL  = RCC_CFGR_SWS_PLL
 * HSE  = RCC_CFGR_SWS_HSE
 */
ClockSource_ReturnType RCC_CheckSystemClock(void);

/* Configure PLL
 * this function take two argument
 * RCC_Source :-
 *    RCC_CFGR_PLLXTPRESRC_HSE_divided_1
 *    RCC_CFGR_PLLXTPRESRC_HSE_divided_2
 *    RCC_CFGR_PLLXTPRESRC_HSI_divided_2
 *    RCC_CR_PLLON
 * RCC_PLLMUL :-
 *    RCC_CFGR_PLLMUL_2
      RCC_CFGR_PLLMUL_3
      RCC_CFGR_PLLMUL_4
      RCC_CFGR_PLLMUL_5
	  RCC_CFGR_PLLMUL_6
	  RCC_CFGR_PLLMUL_7
	  RCC_CFGR_PLLMUL_8
	  RCC_CFGR_PLLMUL_9
      RCC_CFGR_PLLMUL_10
	  RCC_CFGR_PLLMUL_11
	  RCC_CFGR_PLLMUL_12
	  RCC_CFGR_PLLMUL_13
	  RCC_CFGR_PLLMUL_14
	  RCC_CFGR_PLLMUL_15
	  RCC_CFGR_PLLMUL_16
 */
extern void RCC_PLLConfiguration(u32 RCC_PLLSource , u32 RCC_PLLMUL);



/*  Set  Prescaler
 * input argument
  RCC_CFGR_PPRE2_div_1      -> HCLK not divided
  RCC_CFGR_PPRE2_div_2      -> HCLK divided by 2
  RCC_CFGR_PPRE2_div_4      -> HCLK divided by 4
  RCC_CFGR_PPRE2_div_8		-> HCLK divided by 8
  RCC_CFGR_PPRE2_div_16		-> HCLK divided by 16
 *
 */
extern void RCC_PPRE2_SetPrescaler(u32 Prescaler);

/*  Set  Prescaler
 * input argument
  RCC_CFGR_PPRE1_div_1      -> HCLK not divided
  RCC_CFGR_PPRE1_div_2      -> HCLK divided by 2
  RCC_CFGR_PPRE1_div_4      -> HCLK divided by 4
  RCC_CFGR_PPRE1_div_8		-> HCLK divided by 8
  RCC_CFGR_PPRE1_div_16		-> HCLK divided by 16
 *
 */
extern void RCC_PPRE1_SetPrescaler(u32 Prescaler);

/*  Set  Prescaler
 * input argument
  RCC_CFGR_HPRE_div_1      -> HCLK not divided
  RCC_CFGR_HPRE_div_2      -> HCLK divided by 2
  RCC_CFGR_HPRE_div_4      -> HCLK divided by 4
  RCC_CFGR_HPRE_div_8	   -> HCLK divided by 8
  RCC_CFGR_HPRE_div_16	   -> HCLK divided by 16
  RCC_CFGR_HPRE_div_64     -> HCLK divided by 64
  RCC_CFGR_HPRE_div_128    -> HCLK divided by 128
  RCC_CFGR_HPRE_div_256    -> HCLK divided by 256
  RCC_CFGR_HPRE_div_512    -> HCLK divided by 512

 *
 */
extern void RCC_HPRE_SetPrescaler(u32 Prescaler);


/*
 *  ADC prescaler function take one argument
		RCC_CFGR_ADCPRE_div_2 : PCLK2 divided by 2
		RCC_CFGR_ADCPRE_div_4 : PCLK2 divided by 4
		RCC_CFGR_ADCPRE_div_6 : PCLK2 divided by 6
		RCC_CFGR_ADCPRE_div_8 : PCLK2 divided by 8
 */
extern void RCC_ADCPRE_SetPrescaler(u32 Prescaler);


/*
 * Select MCO: Microcontroller clock output
 * take one argument from
    RCC_CFGR_MCO_NOCLK      -> No clock
    RCC_CFGR_MCO_SYSCLK     -> System clock (SYSCLK) selected
    RCC_CFGR_MCO_HSI        -> HSI clock selected
    RCC_CFGR_MCO_HSE        -> HSE clock selected
    RCC_CFGR_MCO_PLL        -> PLL clock selected
 */
extern void RCC_SelectMCO(u32 Clock);




/*
  RCC_APB2ENR_AFIOEN_Enable     AFIOEN: Alternate function IO clock enable
 RCC_APB2ENR_IOPAEN_PORTA       IOPAEN: IO port A clock enable
 RCC_APB2ENR_IOPBEN_PORTB       IOPBEN: IO port B clock enable
 RCC_APB2ENR_IOPCEN_PORTC       IOPCEN: IO port C clock enable
 RCC_APB2ENR_IOPDEN_PORTD       IOPDEN: IO port D clock enable
 RCC_APB2ENR_IOPEEN_PORTE       IOPEEN: IO port E clock enable
 RCC_APB2ENR_IOPFEN_PORTF       IOPFEN: IO port F clock enable
 RCC_APB2ENR_IOPGEN_PORTG       IOPGEN: IO port G clock enable
 RCC_APB2ENR_ADC1EN      	    ADC1EN: ADC 1 interface clock enable
 RCC_APB2ENR_ADC2EN      	   ADC2EN: ADC 2 interface clock enable
 RCC_APB2ENR_TIM1EN      	   TIM1EN: TIM1 timer clock enable
 RCC_APB2ENR_SPI1EN      	   TIM8EN: TIM8 Timer clock enable
 RCC_APB2ENR_TIM8EN      	   TIM8EN: TIM8 Timer clock enable
 RCC_APB2ENR_TIM9EN      	   TIM9EN: TIM9 Timer clock enable
 RCC_APB2ENR_TIM10EN     	   TIM10EN: TIM10 timer clock enable
 RCC_APB2ENR_TIM11EN    	   TIM11EN: TIM11 Timer clock enable
 RCC_APB2ENR_USART1EN     	   USART1EN: USART1 clock enable
 RCC_APB2ENR_ADC3EN      	   ADC3EN: ADC3 interface clock enable

 */
extern void RCC_EnablePeripheral_APB2(u32 Peripheral);

/*
 RCC_APB1ENR_USART2EN  ->  USART2 clock enabled
 RCC_APB1ENR_USART3EN  ->  USART3 clock enabled
 RCC_APB1ENR_UART4EN   ->  UART4 clock enabled
 RCC_APB1ENR_UART5EN   ->  UART5 clock enabled
 RCC_APB1ENR_CAN1EN
 RCC_APB2ENR_CAN1EN
 */
extern void RCC_EnablePeripheral_APB1(u32 Peripheral);

/*
 RCC_AHBENR_DMA1EN  ->  DMA1 clock enable
 RCC_AHBENR_DMA2EN  ->  DMA2 clock enable
 */
extern void RCC_EnablePeripheral_AHB(u32 Peripheral);


