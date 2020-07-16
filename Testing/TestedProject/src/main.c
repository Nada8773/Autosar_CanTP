/*
 * main.c
 */
//#pragma diag_push
//#pragma CHECK_MISRA("none")


#include "Std_Types.h"

//#include "OS/includes/Std_Types.h"
#include <stdbool.h>
#include <stdint.h>

#include "RCC.h"
//#include <freertos/InitConsole.h>

//#include <PORTF.h>
#include "Platform_Types.h"
#include "Compiler.h"
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "CanTp.h"


/* Kernel */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
//#include "semphr.h"
#include "diag/Trace.h"

/* Task Priority*/
#include "Platform_Types.h"
#include "Compiler.h"
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "CanTp.h"

/* Task Priority*/
#define MAIN_PRIORITY      	   (2)
#define TRANSMIT_PRIORITY      (1)



/* Private Function Prototype */
static void MainTask( void *pvParameters );
static void TransmitTask( void *pvParameters );



//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

int flag=1;

int
main(void)
{
	   // PORTF_Init();
		RCC_SetClock( RCC_CR_HSE ,ON);
		Select_SystemClock(RCC_CFGR_SW_HSE);
		RCC_EnablePeripheral_APB2(RCC_APB2ENR_IOPBEN_PORTB);
		RCC_EnablePeripheral_APB1(RCC_APB1ENR_CAN1EN);



    CanTp_Init(NULL_PTR);

    /* Create Task 1 */
    xTaskCreate( MainTask, "MainTask", 1000, NULL, MAIN_PRIORITY, NULL );

    /* Create Task 2 */
    xTaskCreate( TransmitTask, "TransmitTask", 1000, NULL, TRANSMIT_PRIORITY, NULL );


    vTaskStartScheduler();

    while(1)
    {
    }
}


static void MainTask( void *pvParameters ) // every 1 sec
{

    while(1)
    {
        CanTp_MainFunction();

        vTaskDelay(1000);    /* waiting 1 sec*/
    }
}

static void TransmitTask( void *pvParameters )// every 5 sec
{

    uint8 Array[]= {'I','T','I','_','4','0','_','C','A','N','T','P','_','T','e','a','m'};
    PduInfoType Frame;

    Frame.SduDataPtr = Array;
    Frame.SduLength=17;

    while(1)
    {
        if (flag == 1)
        {
            /* Transmit **********/
            CanTp_Transmit((uint16)1, &Frame);

            /* Receive ***********/

            /*CanTp_RxIndication(1,&FF);
           CanTp_RxIndication(1,&CF);*/

            flag = 0;
        }
        vTaskDelay(5000);    /* waiting 5 sec*/
    }

}


