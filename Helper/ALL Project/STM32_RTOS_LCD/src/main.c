/*
 * main.c
 */
#pragma diag_push
#pragma CHECK_MISRA("none")


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
#define TASK1_PRIORITY      (4)
#define TASK2_PRIORITY      (5)



/* Private Function Prototype */
static void Task_1( void *pvParameters );
static void Task_2( void *pvParameters );



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
    xTaskCreate( Task_1, "Task_1", 1000, NULL, TASK1_PRIORITY, NULL );

    /* Create Task 2 */
    xTaskCreate( Task_2, "Task_2", 1000, NULL, TASK2_PRIORITY, NULL );


    vTaskStartScheduler();

    while(1)
    {
    }
}


static void Task_1( void *pvParameters ) // every 1 sec
{

    while(1)
    {
        CanTp_MainFunction();

        vTaskDelay(1000);    /* waiting 1 sec*/
    }
}

//TASK(T2) // every 5 sec
static void Task_2( void *pvParameters )
{
    // Transmit();

    uint8 Array[]= {'M','o','h','a','m','e','d','F','a','r','a','g','i','s','g','o','o','d'};
    PduInfoType Frame;

    Frame.SduDataPtr = Array;
    Frame.SduLength=18;



    uint8 Array1[] = {0x10,0x0C,'M','o','h','a','m','e'};       //{SF,FF
    uint8 Array2[] = {0x21, 'd' ,'F','a','r','a','g'};          /* segment number = 1 */
    PduInfoType FF;
    PduInfoType CF;

    FF.SduDataPtr = Array1;
    FF.SduLength=8;
    /* Lazm ykon >= 8 3shan my7slsh return */
    CF.SduDataPtr = Array2;
    CF.SduLength=7;

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


