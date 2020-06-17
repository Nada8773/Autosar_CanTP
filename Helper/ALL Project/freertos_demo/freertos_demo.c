//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "led_task.h"
#include "switch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "Platform_Types.h"
#include "Compiler.h"
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "CanTp.h"

/* Task Priority*/
#define TASK1_PRIORITY      (5)
#define TASK2_PRIORITY      (4)



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
    //
    // Set the clocking to run at 50 MHz from the PLL.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);


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
            CanTp_Transmit(1,&Frame);

            /* Receive ***********/
           /*CanTp_RxIndication(1,&FF);
           CanTp_RxIndication(1,&CF);*/

            flag = 0;
        }
        vTaskDelay(5000);    /* waiting 5 sec*/
    }

}


