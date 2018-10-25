#include "ewbm_lt_comm.h"
#include "MS500.h"
#include "hal_uart.h"
#include "hal_gpio.h"

#include <stdarg.h>
#include <string.h>



int temp = 0;
int type = 1;
int target = TARGET_3;
uint8_t edit_t;
int gRecvLen = 0;
unsigned char gBuffer[Lora_command_MAX] = { 0, };

static void uartRxInterrupt(void* param)
{
	uint8_t readed;
	
	if(target == TARGET_3)
		gBuffer[gRecvLen++] = UART_Read(UART2);
	else
		readed = UART_Read(UART2);
	
	if(target == TARGET_1)
		UART_Write(UART3, readed);
	else if(target == TARGET_2)
		UART_Write(UART1, readed);
		
}

static void gpioInterrupt(void* param)
{
//	PRINTF("%s:%d\r\n",__func__,__LINE__);
	GPIO_WritePin(GPIO2, GPIO_PIN_11, 0);
	GPIO_WritePin(GPIO2, GPIO_PIN_12, 0);
	GPIO_WritePin(GPIO2, GPIO_PIN_13, 0);
	GPIO_WritePin(GPIO2, GPIO_PIN_14, 0);
	temp = 1;
}

int main(void)
{
	int ret = -1;
	int edit;
	int change = 0;
	int time = 0;
    PINMAP_Pin pins[] = {PA8, PA9, PB4, PB5, PB6, PB7, PIN_NON};
	PINMAP_Pin gpio[] = {PA0, PA1, PA2, PA6, PA10, PB10, PB11, PB12, PB13, PB14, PC6, PIN_NON};	//PA2 is OPEN Drain
	
    PINMAP_SetMultiFunction(pins, PINMAP_MODE_UART);
	PINMAP_SetMultiFunction(gpio, PINMAP_MODE_GPIO);
    
	GPIO_Init(GPIO1, GPIO_MODE_INPUT, GPIO_PIN_0);
	GPIO_Init(GPIO1, GPIO_MODE_OPEN_DRAIN, GPIO_PIN_1);
	GPIO_Init(GPIO1, GPIO_MODE_OPEN_DRAIN, GPIO_PIN_2);
	GPIO_Init(GPIO1, GPIO_MODE_INPUT, GPIO_PIN_6);
	GPIO_Init(GPIO1, GPIO_MODE_OUTPUT, GPIO_PIN_10);
	GPIO_Init(GPIO2, GPIO_MODE_INPUT, GPIO_PIN_10);
	GPIO_Init(GPIO2, GPIO_MODE_OUTPUT, GPIO_PIN_11);
	GPIO_Init(GPIO2, GPIO_MODE_OUTPUT, GPIO_PIN_12);
	GPIO_Init(GPIO2, GPIO_MODE_OUTPUT, GPIO_PIN_13);
	GPIO_Init(GPIO2, GPIO_MODE_OUTPUT, GPIO_PIN_14);
	GPIO_Init(GPIO3, GPIO_MODE_INPUT, GPIO_PIN_6);
	
	PRINTF_Init(UART2, 115200);
	UART_Init(UART1, 115200);
	UART_Init(UART2, 115200);
	UART_Init(UART3, 115200);
	
	LoraTestInit();
	
	UART_InterruptSetCallback(UART2, UART_INT_MASK_RX, uartRxInterrupt, NULL);
	UART_InterruptEnable(UART2, UART_INT_MASK_RX);
	
	GPIO_InterruptSetCallback(GPIO1, GPIO_PIN_6, gpioInterrupt, NULL);
	GPIO_InterruptEnable(GPIO1, GPIO_INT_MODE_RISING_EDGE, GPIO_PIN_6);
	
	GPIO_InterruptSetCallback(GPIO1, GPIO_PIN_0, gpioInterrupt, NULL);
	GPIO_InterruptEnable(GPIO1, GPIO_INT_MODE_RISING_EDGE, GPIO_PIN_0);
	
	while ( 1 )
    {
		if(temp == 1)
		{	
			target = TARGET_1;
			ret = LoraBootStatus();
			if(ret != 0)
			{
				PRINTF("Boot Fail!!\r\n");
				while(1)
				{
					GPIO_TogglePin(GPIO2, GPIO_PIN_11);
					DELAY_SleepMS(200);
				}
			}
			else if(ret == 0)
				GPIO_WritePin(GPIO2, GPIO_PIN_11, 1);
				
			temp = 0;	
			ret = LoraTestStart(target);
			edit = ret;
			if(edit == 0)
			{	
				PRINTF("[TARGET 1] Complete\r\n");
			}
			else
			{
				PRINTF("\r\n\r\n%d command edit!!\r\n",edit);
				PRINTF("[TARGET 1] Edit Complete\r\n\r\n");
			}
			
			target = TARGET_2;
			
			PRINTF("\r\n\r\n");
			DELAY_SleepMS(100);
			
			ret = LoraTestStart(target);
			edit = ret;
			if(edit == 0)
			{				
				PRINTF("[TARGET 2] Complete\r\n\r\n");
			}
			else
			{
				PRINTF("\r\n\r\n%d command edit!!\r\n",edit);
				PRINTF("[TARGET 2] Edit Complete\r\n");
			}
			
			GPIO_WritePin(GPIO2, GPIO_PIN_12, 1);
				
			ret = LoraTxRxTest(TARGET_1, TxRxTest_1);
			if(ret != 0)
			{
				while(1)
				{
					GPIO_TogglePin(GPIO2, GPIO_PIN_13);
					DELAY_SleepMS(200);
				}
			}
			DELAY_SleepMS(500);
			
			ret = LoraTxRxTest(TARGET_2, TxRxTest_1);
			if(ret != 0)
			{
				while(1)
				{
					GPIO_TogglePin(GPIO2, GPIO_PIN_13);
					DELAY_SleepMS(200);
				}
			}
			
			/* lora all reset */
			GPIO_WritePin(GPIO1, GPIO_PIN_1, 1);
			GPIO_WritePin(GPIO1, GPIO_PIN_1, 0);
			DELAY_SleepMS(1);
			GPIO_WritePin(GPIO1, GPIO_PIN_1, 1);
			
			GPIO_WritePin(GPIO1, GPIO_PIN_2, 1);
			GPIO_WritePin(GPIO1, GPIO_PIN_2, 0);
			DELAY_SleepMS(1);
			GPIO_WritePin(GPIO1, GPIO_PIN_2, 1);
			
			DELAY_SleepMS(1000);
			
			//txrx test change
			ret = LoraTxRxTest(TARGET_1, TxRxTest_2);
			if(ret != 0)
			{
				while(1)
				{
					GPIO_TogglePin(GPIO2, GPIO_PIN_13);
					DELAY_SleepMS(200);
				}
			}
			DELAY_SleepMS(500);
			
			ret = LoraTxRxTest(TARGET_2, TxRxTest_2);
			if(ret != 0)
			{
				while(1)
				{
					GPIO_TogglePin(GPIO2, GPIO_PIN_13);
					DELAY_SleepMS(200);
				}
			}
			GPIO_WritePin(GPIO2, GPIO_PIN_13, 1);
			
			GPIO_WritePin(GPIO1, GPIO_PIN_1, 1);
			GPIO_WritePin(GPIO1, GPIO_PIN_1, 0);
			DELAY_SleepMS(1);
			GPIO_WritePin(GPIO1, GPIO_PIN_1, 1);
			
			GPIO_WritePin(GPIO1, GPIO_PIN_2, 1);
			GPIO_WritePin(GPIO1, GPIO_PIN_2, 0);
			DELAY_SleepMS(1);
			GPIO_WritePin(GPIO1, GPIO_PIN_2, 1);
			
			target = TARGET_3;
			ret = -1;
			type = 1;
			
		}
		else if(temp == 0)
		{
			target = TARGET_3;
			PRINTF("\r\n-----------------------\r\n");
			PRINTF("Please Typing...\r\n");
			PRINTF("-----------------------\r\n");
			while ( gRecvLen < 3 )
			{
				if(temp == 1)
				{
					goto main;
				}
				__NOP();
			}
			
			ret = LoraCommandEdit(gRecvLen, gBuffer);
			
main:
			gRecvLen = 0;
			memset(gBuffer, 0, strlen(gBuffer));
			
			if(ret == EditStart)
				temp = 1;
		}
		if(ret == EditType)
			PRINTF("TYPE CHANGE\r\n");
		__NOP();
//		DELAY_SleepIO(10);
    }
}
