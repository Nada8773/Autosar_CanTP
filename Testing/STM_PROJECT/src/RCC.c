#include "Std_Type.h"

#include "RCC.h"

/************************* RCC Registers **********************************/
#define BASE_Address_RCC_Peripheral   (u32)0x40021000
typedef struct
{
	u32 CR;
	u32 CFGR;
	u32 CIR;
	u32 APB2RSTR;
	u32 APB1RSTR;
	u32 AHBENR;
	u32 APB2ENR;
	u32 APB1ENR;
	u32 BDCR;
	u32 CSR;
}RCC_Peripheral;

volatile RCC_Peripheral*   const RCC       = (RCC_Peripheral*)BASE_Address_RCC_Peripheral;


/****************************** MASKS *******************/

#define RCC_CFGR_SW_Mask          (u32)0xFFFFFFFC
#define RCC_CFGR_SWS_Mask         (u32)0x0000000C
#define RCC_CFGR_PLL_Mask         (u32)0xFFFFFC0F
#define RCC_CFGR_HPRE_Mask        (u32)0xffffff0f
#define RCC_CFGR_PPRE1_Mask       (u32)0xfffff8ff
#define RCC_CFGR_PPRE2_Mask       (u32)0xffffc7ff
#define RCC_CFGR_MCO_Mask      	  (u32)0xf8ffffff
#define RCC_CFGR_ADCPRE_Mask      (u32)0xffff3fff



/****************************** Static Function ******************/

/* wait the lock ready flag
 * this function take one argument
 * clock :-
 *     RCC_CR_HSI
 *     RCC_CR_HSE
 *     RCC_CR_PLL
 */

static void RCC_WaitFlagReady(u32 Clock);



/*  Select System Clock :
 *   RCC_CFGR_SW_HSI
 *   RCC_CFGR_SW_HSE
 *   RCC_CFGR_SW_PLL
 */
extern void Select_SystemClock(u32 SystemClock)
{
	u32 Local_Temp=0;
	Local_Temp=RCC->CFGR;
	Local_Temp &=RCC_CFGR_SW_Mask;
	Local_Temp |= SystemClock;
	RCC->CFGR =Local_Temp;
}


/*
 * this function take two argument
 * clock :-
 *     RCC_CR_HSI
 *     RCC_CR_HSE
 *     RCC_CR_HSEBYP
 *     RCC_CR_PLL
 * State :-
 *      ON  /  OFF
 */
extern void  RCC_SetClock(u32 Clock ,u8 State)
{
	switch(State)
		{
		case ON:
			RCC->CR |=Clock;
			RCC_WaitFlagReady(Clock);
			break;

		case OFF:
			RCC->CR &=~Clock;
			break;
		}
}

/* wait the lock ready flag
 * this function take one argument
 * clock :-
 *     RCC_CR_HSI
 *     RCC_CR_HSE
 *     RCC_CR_PLL
 */

static void RCC_WaitFlagReady(u32 Clock)
{
	switch(Clock)
	{
	   case  RCC_CR_HSI :
		   while (!(RCC->CR & RCC_CR_HSIRDY));
		   break;
	   case  RCC_CR_HSE :
		   while (!(RCC->CR & RCC_CR_HSERDY));
		   break;
	   case  RCC_CR_PLL :
		   while (!(RCC->CR & RCC_CR_PLLRDY));
		   break;
	}
}



/*
 * Return clock source
 * RCC_CFGR_SWS_HSI  -> HSI oscillator used as system clock
 * RCC_CFGR_SWS_PLL  -> PLL used as system clock
 * RCC_CFGR_SWS_HSE  -> HSE oscillator used as system clock
 */
ClockSource_ReturnType RCC_CheckSystemClock(void)
{
	return (RCC->CFGR & RCC_CFGR_SWS_Mask);
}


/*
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
extern void RCC_PLLConfiguration(u32 RCC_PLLSource , u32 RCC_PLLMUL)
{
	    u32 Local_Temp=0;
		Local_Temp=RCC->CFGR;
		Local_Temp &=RCC_CFGR_PLL_Mask;
		RCC->CFGR |= RCC_PLLSource | RCC_PLLMUL ;
		RCC->CFGR =Local_Temp;
}

/*  Set  Prescaler
 * input argument
  RCC_CFGR_PPRE2_div_1      -> HCLK not divided
  RCC_CFGR_PPRE2_div_2      -> HCLK divided by 2
  RCC_CFGR_PPRE2_div_4      -> HCLK divided by 4
  RCC_CFGR_PPRE2_div_8		-> HCLK divided by 8
  RCC_CFGR_PPRE2_div_16		-> HCLK divided by 16
 *
 */
extern void RCC_PPRE2_SetPrescaler(u32 Prescaler)
{
    u32 Local_Temp=0;
	Local_Temp=RCC->CFGR;
	Local_Temp &=RCC_CFGR_PPRE2_Mask;
	RCC->CFGR |= Prescaler ;
	RCC->CFGR =Local_Temp;
}

/*  Set  Prescaler
 * input argument
  RCC_CFGR_PPRE1_div_1      -> HCLK not divided
  RCC_CFGR_PPRE1_div_2      -> HCLK divided by 2
  RCC_CFGR_PPRE1_div_4      -> HCLK divided by 4
  RCC_CFGR_PPRE1_div_8		-> HCLK divided by 8
  RCC_CFGR_PPRE1_div_16		-> HCLK divided by 16
 *
 */
extern void RCC_PPRE1_SetPrescaler(u32 Prescaler)
{
    u32 Local_Temp=0;
	Local_Temp=RCC->CFGR;
	Local_Temp &=RCC_CFGR_PPRE1_Mask;
	RCC->CFGR |= Prescaler ;
	RCC->CFGR =Local_Temp;
}


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
 */
extern void RCC_HPRE_SetPrescaler(u32 Prescaler)
{
    u32 Local_Temp=0;
	Local_Temp=RCC->CFGR;
	Local_Temp &=RCC_CFGR_HPRE_Mask;
	RCC->CFGR |= Prescaler ;
	RCC->CFGR =Local_Temp;
}

/*
 *  ADC prescaler function take one argument
		RCC_CFGR_ADCPRE_div_2 : PCLK2 divided by 2
		RCC_CFGR_ADCPRE_div_4 : PCLK2 divided by 4
		RCC_CFGR_ADCPRE_div_6 : PCLK2 divided by 6
		RCC_CFGR_ADCPRE_div_8 : PCLK2 divided by 8
 */
extern void RCC_ADCPRE_SetPrescaler(u32 Prescaler)
{
    u32 Local_Temp=0;
	Local_Temp=RCC->CFGR;
	Local_Temp &=RCC_CFGR_ADCPRE_Mask;
	RCC->CFGR |= Prescaler ;
	RCC->CFGR =Local_Temp;
}

/*
 * Select MCO: Microcontroller clock output
 * take one argument from
    RCC_CFGR_MCO_NOCLK      -> No clock
    RCC_CFGR_MCO_SYSCLK     -> System clock (SYSCLK) selected
    RCC_CFGR_MCO_HSI        -> HSI clock selected
    RCC_CFGR_MCO_HSE        -> HSE clock selected
    RCC_CFGR_MCO_PLL        -> PLL clock selected
 */
extern void RCC_SelectMCO(u32 Clock)
{
    u32 Local_Temp=0;
	Local_Temp=RCC->CFGR;
	Local_Temp &=RCC_CFGR_MCO_Mask;
	RCC->CFGR |= Clock ;
	RCC->CFGR =Local_Temp;
}

/* one  input argument from
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
 RCC_APB2ENR_USARTEN     	   USART1EN: USART1 clock enable
 RCC_APB2ENR_ADC3EN      	   ADC3EN: ADC3 interface clock enable

 */
extern void RCC_EnablePeripheral_APB2(u32 Peripheral)
{

	RCC->APB2ENR |=Peripheral ;
}

extern void RCC_EnablePeripheral_APB1(u32 Peripheral)
{

	RCC->APB1ENR |=Peripheral ;
}

extern void RCC_EnablePeripheral_AHB(u32 Peripheral)
{
	RCC->AHBENR |=Peripheral ;
}

