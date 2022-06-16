/* FreeRTOS includes. */

// Sagar Patel 

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "stdio.h"

#include "stdlib.h"

#define TIMER_ID	1
#define DELAY_1_10_SECOND 100UL

///-----------------------------------------------------------/
/* The Tx and Rx tasks as described at the top of this file. */
static void vTimerCallback( TimerHandle_t pxTimer );
///-----------------------------------------------------------/

// Timer Handle
static TimerHandle_t xTimer = NULL;

volatile  long lCount=0;
volatile long count_sec=0;  // For every second print
volatile  long timer_val=0;
int timer_started = 0x0;
int reset = 0x0;
int temp=-1; // Flag for detecting start or stop 
static TaskHandle_t xMyKeyControl;

static void prvMyKeyControl( void *pvParameters )
{
	while(1)
	{
		char pressed = getchar();
		int asic =pressed;
		if( asic == 13 && temp==-1) 
		{
			count_sec=0;
			timer_started = 0x1;
			temp=0;
			xil_printf("Stopwatch Started.....\r\n");
		}
		else
		{
			timer_started=0x00;
			temp=-1;
			xil_printf("Stopwatch Stopped.....\r\n");
		}
		if (asic == 114)
		{
			timer_started=0x0;
			reset=0x1;
			xil_printf("Stopwatch Reset.....\r\n");
		}
	}
	vTaskDelete(NULL);
}

int main( void )
{
	const TickType_t x1_10seconds = pdMS_TO_TICKS( DELAY_1_10_SECOND );

	setvbuf(stdin, NULL, _IONBF, 0);

	xil_printf( "\r\nECPS209-FreeRTOS-Timer (Enter:Start/Stop r:Reset)\r\n" );

	xTaskCreate( prvMyKeyControl,
				 ( const char * ) "MyKeyControl",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 3,
				 &xMyKeyControl );

	xTimer = xTimerCreate( (const char *) "Timer",
							x1_10seconds,
							pdTRUE,
							(void *) TIMER_ID,
							vTimerCallback);

	xTimerStart( xTimer, 0 );

	vTaskStartScheduler();

	for( ;; );
}


static void vTimerCallback( TimerHandle_t pxTimer )
{
	// Printing every second
	if((count_sec==10))
	{
		count_sec=0;
		xil_printf("%02d:%02d:%02d\r\n", (timer_val/600)%60,(timer_val/10)%60 ,(timer_val%10)*10);
	}
	count_sec++;

	if(timer_started)
	{
		timer_val = ++lCount;
	}

	if(reset)
	{
		timer_val = 0;
		reset=0x0;
		lCount=0;
		count_sec=0;
	}
}
