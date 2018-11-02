#include "ewbm_lt_comm.h"
#include "MS500.h"
#include "hal_uart.h"
#include "hal_gpio.h"

#include <stdarg.h>
#include <string.h>



int temp = TEST_WAIT;
int type = 1;
int target = DEFAULT;
int gRecvLen = 0;
uint8_t gBuffer[64] = { 0, };

static void uartRxInterrupt(void* param)
{
	uint8_t readed;
	
	if(target == DEFAULT)
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
	temp = TEST_START;
}

int main(void)
{
	int ret = -1;
	int target_cmp_error = 0;
	
    PINMAP_Pin pins[] = {PA8, PA9, PB4, PB5, PB6, PB7, PIN_NON};
	PINMAP_Pin gpio[] = {PA0, PA1, PA2, PA6, PA10, PA12, PA13, PB10, PB11, PB12, PB13, PB14, PC6, PIN_NON};	//PA2 is OPEN Drain
	
    PINMAP_SetMultiFunction(pins, PINMAP_MODE_UART);
	PINMAP_SetMultiFunction(gpio, PINMAP_MODE_GPIO);
    
	GPIO_Init(GPIO1, GPIO_MODE_INPUT, GPIO_PIN_0);
	GPIO_Init(GPIO1, GPIO_MODE_OPEN_DRAIN, GPIO_PIN_1);
	GPIO_Init(GPIO1, GPIO_MODE_OPEN_DRAIN, GPIO_PIN_2);
	GPIO_Init(GPIO1, GPIO_MODE_INPUT, GPIO_PIN_6);
	GPIO_Init(GPIO1, GPIO_MODE_OUTPUT, GPIO_PIN_10);
	GPIO_Init(GPIO1, GPIO_MODE_OUTPUT, GPIO_PIN_12);
	GPIO_Init(GPIO1, GPIO_MODE_OUTPUT, GPIO_PIN_13);
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
		if(temp == TEST_START)
		{	
			PRINTF("-----------------------------------------------\r\n");
			PRINTF("                  TEST START\r\n");
			PRINTF("-----------------------------------------------\r\n\r\n\r\n");
			target = TARGET_1;
			PRINTF("-----------------------------------------------\r\n");
			PRINTF("                 BOOT TEST Start\r\n");
			PRINTF("-----------------------------------------------\r\n\r\n\r\n");
			ret = LoraBootStatus();
			if(ret != BOOT_PASS)
			{
				if(ret == BOOT_TARGET1_ERROR)
				{
					PRINTF("TARGET1 BOOT FAIL\r\n");
					
					LoraLedSetting(TARGET_1, BOOT_TEST);
				}
				else if(ret == BOOT_TARGET2_ERROR)
				{
					PRINTF("TARGET2 BOOT FAIL\r\n");
					
					LoraLedSetting(TARGET_2, BOOT_TEST);
				}
			}
			else if(ret == BOOT_PASS)
				LoraLedSetting(TARGET_1_2, BOOT_TEST);
				
			temp = TEST_WAIT;	
			PRINTF("-----------------------------------------------\r\n");
			PRINTF("               COMMAND TEST START\r\n");
			PRINTF("-----------------------------------------------\r\n\r\n\r\n");
			PRINTF("[TARGET 1] START\r\n");
			ret = LoraTestStart(target);
			if(ret == TARGET_DEVEUI_ERROR)
			{
				target_cmp_error = TARGET1_DEVEUI_ERROR;
				PRINTF("[TARGET 1] FAIL\r\n");
			}
			else if(ret == TARGET_VER_ERROR)
			{
				target_cmp_error = TARGET1_VER_ERROR;
				PRINTF("[TARGET 1] FAIL\r\n");
			}
			
			if(ret == TARGET_SUCCESS && target_cmp_error != TARGET1_DEVEUI_ERROR && target_cmp_error != TARGET1_VER_ERROR)
			{	
				PRINTF("[TARGET 1] COMPLETE\r\n");
			}
			else if(target_cmp_error != TARGET1_DEVEUI_ERROR && target_cmp_error != TARGET1_VER_ERROR)
			{
				PRINTF("\r\n\r\n%d COMMAND EDIT\r\n", ret);
				PRINTF("[TARGET 1] EDIT COMPLETE\r\n\r\n");
			}
			
			target = TARGET_2;
			PRINTF("\r\n\r\n");
			DELAY_SleepMS(100);
			
			PRINTF("[TARGET 2] START\r\n");
			ret = LoraTestStart(target);
			
			//target_cmp_error = 1 : target 1 DEVEUI error
			//target_cmp_error = 2 : target 1 VER error
			//ret = -1 : target 2 DEVEUI error
			//ret = -2 : target 2 VER error
			if(target_cmp_error == TARGET1_DEVEUI_ERROR || target_cmp_error == TARGET1_VER_ERROR || ret == TARGET2_DEVEUI_ERROR || ret == TARGET2_VER_ERROR)
			{
				GPIO_WritePin(GPIO2, GPIO_PIN_12, 0);
				GPIO_WritePin(GPIO2, GPIO_PIN_13, 0);
				GPIO_WritePin(GPIO2, GPIO_PIN_14, 0);
				
				//target1 VER + target2 VER
				if((target_cmp_error == TARGET1_VER_ERROR && ret == TARGET2_VER_ERROR))
				{
					PRINTF("[TARGET 2] FAIL\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
					LoraLedSetting(TARGET_1_2, CMP_TEST);
				}
				//target1 VER + target2 DEVEUI
				if((target_cmp_error == TARGET1_VER_ERROR && ret == TARGET2_DEVEUI_ERROR))
				{
					PRINTF("[TARGET 2] FAIL\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
					LoraLedSetting(TARGET_1_2, CMP_TEST);
				}
				//target1 VER + target2 pass
				if((target_cmp_error == TARGET1_VER_ERROR && ret == TARGET_SUCCESS))
				{
					PRINTF("[TARGET 2] COMPLETE\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
					LoraLedSetting(TARGET_1, CMP_TEST);
				}
				//target1 DEVEUI + target2 DEVEUI
				if((target_cmp_error == TARGET1_DEVEUI_ERROR && ret == TARGET2_DEVEUI_ERROR))
				{
					PRINTF("[TARGET 2] FAIL\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
					LoraLedSetting(TARGET_1_2, CMP_TEST);
				}
				//target1 DEVEUI + target2 VER
				if((target_cmp_error == TARGET1_DEVEUI_ERROR && ret == TARGET2_VER_ERROR))
				{
					PRINTF("[TARGET 2] FAIL\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
					LoraLedSetting(TARGET_1_2, CMP_TEST);
				}
				//target1 DEVEUI + target2 pass
				if((target_cmp_error == TARGET1_DEVEUI_ERROR && ret == TARGET_SUCCESS))
				{
					PRINTF("[TARGET 2] COMPLETE\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
					LoraLedSetting(TARGET_1, CMP_TEST);
				}
				//target1 pass + target2 DEVEUI
				else if(target_cmp_error == TARGET_SUCCESS && ret == TARGET2_DEVEUI_ERROR)
				{
					PRINTF("[TARGET 2] FAIL\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
					LoraLedSetting(TARGET_2, CMP_TEST);
				}
				//target1 pass + target2 VER
				else if(target_cmp_error == TARGET_SUCCESS && ret == TARGET2_VER_ERROR)
				{
					PRINTF("[TARGET 2] FAIL\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
					LoraLedSetting(TARGET_2, CMP_TEST);
				}
				//target1 pass + target2 pass
				else if(target_cmp_error == TARGET_SUCCESS && ret != TARGET2_DEVEUI_ERROR && ret != TARGET2_VER_ERROR)
				{
					PRINTF("[TARGET 2] COMPLETE\r\n");
					PRINTF("\r\nERROR CODE: %d%d\r\n", target_cmp_error, ret);
				}
				target_cmp_error = 0;
			}
			
			if(ret != TARGET2_DEVEUI_ERROR && ret != TARGET2_VER_ERROR)
			{
				if(ret == TARGET_SUCCESS)
				{				
					PRINTF("[TARGET 2] COMPLETE\r\n\r\n");
				}
				else
				{
					PRINTF("[TARGET 2] EDIT COMPLETE\r\n");
					PRINTF("\r\n\r\n%d COMMAND EDIT\r\n",ret);
				}
			}
			LoraLedSetting(TARGET_1_2, CMP_TEST_SUCCESS);
			PRINTF("-----------------------------------------------\r\n");
			PRINTF("               COMMAND TEST COMPLETE\r\n");
			PRINTF("-----------------------------------------------\r\n\r\n\r\n");
			
			
			PRINTF("-----------------------------------------------\r\n");
			PRINTF("                 RF TEST START\r\n");
			PRINTF("-----------------------------------------------\r\n\r\n\r\n");
			ret = LoraTxRxTest(TARGET_1, TxRxTest_1);
			if(ret != TARGET_SUCCESS)
			{
				LoraLedSetting(TARGET_1, RF_TEST);
			}
			DELAY_SleepMS(500);
			
			ret = LoraTxRxTest(TARGET_2, TxRxTest_1);
			if(ret != TARGET_SUCCESS)
			{
				LoraLedSetting(TARGET_2, RF_TEST);
			}
			
			/* lora all reset */
			LoraLedSetting(TARGET_1_2, RESET);
			
			//txrx test change
			ret = LoraTxRxTest(TARGET_1, TxRxTest_2);
			if(ret != TARGET_SUCCESS)
			{
				while(1)
				{
					LoraLedSetting(TARGET_1, RF_TEST);
				}
			}
			DELAY_SleepMS(500);
			
			ret = LoraTxRxTest(TARGET_2, TxRxTest_2);
			if(ret != TARGET_SUCCESS)
			{
				LoraLedSetting(TARGET_2, RF_TEST);
			}
			PRINTF("-----------------------------------------------\r\n");
			PRINTF("                 RF TEST COMPLETE\r\n");
			PRINTF("-----------------------------------------------\r\n\r\n\r\n");
			LoraLedSetting(TARGET_1_2, RF_TEST_SUCCESS);
			LoraLedSetting(TARGET_1_2, RESET);
			
			PRINTF("-----------------------------------------------\r\n");
			PRINTF("                   TEST FINISH\r\n");
			PRINTF("-----------------------------------------------\r\n\r\n\r\n");
			target = DEFAULT;
			ret = -1;
			type = 1;
			
		}
		else if(temp == TEST_WAIT)
		{
			target = DEFAULT;
			PRINTF("\r\n-----------------------\r\n");
			PRINTF("Please Typing...\r\n");
			PRINTF("-----------------------\r\n");

			while( gBuffer[gRecvLen] != 0x0A && gBuffer[gRecvLen-1] != 0x0D )
			{
				if(temp == TEST_START)
				{
					goto main;
				}
				__NOP();
			}
			
			ret = LoraCommandEdit(gBuffer);
			
main:
			gRecvLen = 0;
			memset(gBuffer, 0, sizeof(gBuffer));
			
			if(ret == EditStart)
				temp = TEST_START;
		}
		if(ret == EditType)
			PRINTF("TYPE CHANGE\r\n");
		__NOP();
    }
}
