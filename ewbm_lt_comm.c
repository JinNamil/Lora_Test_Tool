#include <stdarg.h>
#include <string.h>
#include "MS500.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "ewbm_lt_comm.h"

uint8_t* readed = {0,};
unsigned int readLen = 0;
int gLoraRecvLen = 0;
unsigned char gLoraBuffer[64] = { 0, };
unsigned char CommReceiveBuffer[Lora_command_MAX] = {0, };

static void LoraCommReceive(void* param)
{
    gLoraBuffer[gLoraRecvLen++] = UART_Read(UART3);
}

static void LoraCommReceive_t(void* param)
{
    gLoraBuffer[gLoraRecvLen++] = UART_Read(UART1);
}

static int LoraCommSend(unsigned char* in, unsigned int inLen, int target)
{
	int ret = -1;
	
	if(target == TARGET_1)
	{
		for(int i = 0; i<inLen+2; i++)
		{
			UART_Write(UART3, in[i]);
		}
		UART_Write(UART3, '\r');
		UART_Write(UART3, '\n');
	}
	else if(target == TARGET_2)
	{
		for(int i = 0; i<inLen+2; i++)
		{
			UART_Write(UART1, in[i]);
		}
		UART_Write(UART1, '\r');
		UART_Write(UART1, '\n');
	}
	
	ret = 0;
    return ret;
}

int LoraCommCompare(char* command, int list_num)
{
	int ret = -1;
	
	if(memcmp(command, LoraCommandList[list_num], strlen(LoraCommandList[list_num])) == 0)
		ret = 0;
	else
		ret = -1;
		
	return ret;
}

int LoraBootStatus(void)
{
	int status_1;
	int status_2;

	GPIO_WritePin(GPIO1, GPIO_PIN_1, 1);
	GPIO_WritePin(GPIO1, GPIO_PIN_1, 0);
	DELAY_SleepMS(1);
	GPIO_WritePin(GPIO1, GPIO_PIN_1, 1);
	
	GPIO_WritePin(GPIO1, GPIO_PIN_2, 1);
	GPIO_WritePin(GPIO1, GPIO_PIN_2, 0);
	DELAY_SleepMS(1);
	GPIO_WritePin(GPIO1, GPIO_PIN_2, 1);
	
	DELAY_SleepMS(500);
	status_1 = GPIO_ReadPin(GPIO2, GPIO_PIN_10);
	status_2 = GPIO_ReadPin(GPIO3, GPIO_PIN_6);
	
	if(status_1 == 1 && status_2 == 1)
	{
		PRINTF("-----------------------------------------------\r\n");
		PRINTF("                 Boot Complete\r\n");
		PRINTF("-----------------------------------------------\r\n\r\n\r\n");
		status_1 = 0;
		status_2 = 0;
		
		return 0;
	}
	return -1;
}

int LoraCommandEdit(int RecvLen, char* RecvBuffer)
{
	char* result = {0,};
	int CommNum = -1;
	int txrx = 0;
	char txrxResult[64] = {0,};
	DELAY_SleepMS(100);
	
	/* TxRx compare */
	if(memcmp(LoraTxRxName[0], RecvBuffer, strlen(LoraTxRxName[0])) == 0)
	{
		memcpy(txrxResult, RecvBuffer+(strlen(LoraTxRxName[0])+1), strlen(LoraTxRxValue[0]));
		memset(LoraTxRxValue[0], 0, strlen(LoraTxRxValue[0]));
		sprintf(LoraTxRxValue[0], "%s", txrxResult);
		memcpy(LoraTxRxValue[1], LoraTxRxValue[0], 9);
		memcpy(LoraTxRxValue[1]+10, LoraTxRxValue[0]+13, 1);
		PRINTF("TXST Value: %s\r\n", LoraTxRxValue[0]);
		PRINTF("RXST Value: %s\r\n", LoraTxRxValue[1]);
		PRINTF("TXST RXST Default Edit Change Complete\r\n");
		return EditDefault;
	}
	else if(memcmp(LoraTxRxName[1], RecvBuffer, strlen(LoraTxRxName[1])) == 0)
	{
		memcpy(txrxResult, RecvBuffer+(strlen(LoraTxRxName[1])+1), strlen(LoraTxRxValue[1]));
		memset(LoraTxRxValue[1], 0, strlen(LoraTxRxValue[1]));
		sprintf(LoraTxRxValue[1], "%s", txrxResult);
		memcpy(LoraTxRxValue[0], LoraTxRxValue[1], 9);
		memcpy(LoraTxRxValue[0]+13, LoraTxRxValue[1]+10, 1);
		PRINTF("TXST Value: %s\r\n", LoraTxRxValue[0]);
		PRINTF("RXST Value: %s\r\n", LoraTxRxValue[1]);
		PRINTF("RXST TXST Default Edit Change Complete\r\n");
		return EditDefault;
	}
	
	result = strtok(RecvBuffer, " ");

	for(int i=0; i<Lora_command_MAX; i++)
	{
		if(memcmp(LoraCommandName[i], result, strlen(LoraCommandName[i])) == 0)
		{
			CommNum = i;
			goto LoraCommandEdit;
		}
	}
LoraCommandEdit:
	
	/* TYPE and START Command */
	if(memcmp(RecvBuffer, "TYPE", 4) == 0)
	{
		PRINTF("-----------------TYPE change-----------------\r\n");
		return EditType;
	}
	else if(memcmp(RecvBuffer, "START", 5) == 0)
	{
		PRINTF("-----------------START-----------------\r\n");
		return EditStart;
	}
	
	if(CommNum == -1)
	{
		PRINTF("NOT Command\r\n");
		return EditDefault;
	}
	
	
	PRINTF("Command Name: %s\r\n", result);
	result = strtok(NULL, " ");
	PRINTF("Command Value: %s\r\n", result);
	
	memset(LoraCommandList[CommNum], 0, strlen(LoraCommandList[CommNum]));
	sprintf(LoraCommandList[CommNum], "%s", result);
	PRINTF("Default Edit Complete\r\n");
	return EditDefault;
}

int LoraCommRead(int num, uint8_t** read)
{
	int ret = -1;
	uint8_t* send_mes = {0,};
	
	if(num >= Lora_command_MAX)
	{
		PRINTF("%s:%d\r\n",__func__,__LINE__);
		ret = -1;
		return ret;
	}
	
	send_mes = LoraCommand[num];
	
	*read = send_mes;
	ret = 0;
	return ret;
}

int LoraTestInit(void)
{
	int ret = -1;
	
	UART_InterruptSetCallback(UART1, UART_INT_MASK_RX, LoraCommReceive_t, NULL);
	UART_InterruptEnable(UART1, UART_INT_MASK_RX);
	
	UART_InterruptSetCallback(UART3, UART_INT_MASK_RX, LoraCommReceive, NULL);
	UART_InterruptEnable(UART3, UART_INT_MASK_RX);
 
	
	PRINTF("------------------Lora Init--------------------\r\n");
	for(int a = 0; a< Lora_command_MAX; a++ )
	{
		PRINTF("%s: %s",LoraCommandName[a], LoraCommandList[a]);
	}
	for(int i = 0; i<2; i++)
	{
		PRINTF("%s: %s",LoraTxRxName[i], LoraTxRxValue[i]);
	}
	PRINTF("------------------Lora Init--------------------\r\n");
		
	return 0;
}

int LoraTestCmp(int j, int type)
{
	int ret = -1;
	int fail_count = 0;
	unsigned char ltCommReceiveTemp[64] = {0,};
	unsigned char ptr[64] = {0, };
	
	ret = LoraCommCompare(gLoraBuffer, j);
	if(ret == 0)
	{
		DELAY_SleepMS(10);
		memcpy(ltCommReceiveTemp, gLoraBuffer, strlen(LoraCommandList[j]));
		if(j == DEVEUI)
		{
			PRINTF("ERROR!!\r\n");
			GPIO_WritePin(GPIO2, GPIO_PIN_11, 0);
			GPIO_WritePin(GPIO2, GPIO_PIN_12, 0);
			GPIO_WritePin(GPIO2, GPIO_PIN_13, 0);
			GPIO_WritePin(GPIO2, GPIO_PIN_14, 0);
			
			while(1)
			{
				GPIO_TogglePin(GPIO2, GPIO_PIN_12);
				DELAY_SleepMS(200);
			}
		}
		PRINTF("%s <OK>\r\n",LoraCommandName[j]);
	}
	else
	{
		DELAY_SleepMS(10);
		if(j == VER)
		{
			PRINTF("ERROR!!\r\n");
			GPIO_WritePin(GPIO2, GPIO_PIN_11, 0);
			GPIO_WritePin(GPIO2, GPIO_PIN_12, 0);
			GPIO_WritePin(GPIO2, GPIO_PIN_13, 0);
			GPIO_WritePin(GPIO2, GPIO_PIN_14, 0);
			
			while(1)
			{
				GPIO_TogglePin(GPIO2, GPIO_PIN_12);
				DELAY_SleepMS(200);
			}
		}
		else if(j == DEVEUI)
		{
			PRINTF("%s <OK>\r\n",LoraCommandName[j]);
			goto LoraTestCmp;
		}
		
		
		memcpy(ltCommReceiveTemp, gLoraBuffer, strlen(LoraCommandList[j]));
		PRINTF("\r\n[Current Value]:%s\r\n%s Value Change: %s\r\n", ltCommReceiveTemp, LoraCommandName[j], LoraCommandList[j]);
		
		fail_count++;
	}
	if(fail_count != 0)
	{
		PRINTF("%s is change\r\n", LoraCommandName[j]);
		sprintf(ptr, "%s %s", LoraCommand[j], LoraCommandList[j]);
		
		if(type == 0)
		{
			ret = LoraCommSend(ptr, strlen(ptr), TARGET_1);
		}
		else
		{
			ret = LoraCommSend(ptr, strlen(ptr), TARGET_2);
		}
		
		while ( gLoraRecvLen < 5 )
			__NOP();
			
		DELAY_SleepMS(100);
	}
	
LoraTestCmp:
	memset(gLoraBuffer, 0, strlen(gLoraBuffer));
	memset(ltCommReceiveTemp, 0, strlen(ltCommReceiveTemp));
	
	return fail_count;
}

int LoraTxRxTest(int tar, int change)
{
	int timeout = 0;
	char* ptr;
	char result[64] = {0, };
	int ret = -1;
	gLoraRecvLen = 0;
	memset(gLoraBuffer, 0, strlen(gLoraBuffer));
	if(change == TxRxTest_1)
	{
		if(tar == TARGET_1)
		{
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			sprintf(result, "%s %s", LoraTxRxTestCommand[0], LoraTxRxValue[0]);
			ret = LoraCommSend(result, strlen(result), TARGET_2);
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
			
			while ( gLoraRecvLen <  74 )
			{
				DELAY_SleepMS(1);
				timeout++;
				if(timeout > 5000)
				{
					PRINTF("time out!!\r\n");
					timeout = 0;
					return -1;
				}
				__NOP();
			}
			
			DELAY_SleepMS(1000);
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, strlen(gLoraBuffer));
			
			while ( gLoraRecvLen < 18 )
			{
				DELAY_SleepMS(1);
				timeout++;
				if(timeout > 5000)
				{
					PRINTF("time out!!\r\n");
					timeout = 0;
					return -1;
				}
				__NOP();
			}
				
			DELAY_SleepMS(100);
				
			ptr = strtok(gLoraBuffer, " ");
			ptr = strtok(NULL, " ");
			if(memcmp(ptr, "transmitted\r\n", strlen(ptr)) == 0)
			{
				PRINTF("(TARGET2)TX Test COMPLETE\r\n");
				ret = 0;
			}
			else
			{
				PRINTF("(TARGET2)TX Message: %s\r\n", ptr);
				PRINTF("(TARGET2)TX Test FAIL\r\n");
				ret = -1;
			}
			
			UART_InterruptDisable(UART1, UART_INT_MASK_RX);
			
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			memset(result, 0, strlen(result));
		}
		else if(tar == TARGET_2)
		{
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			ptr = NULL;
			sprintf(result, "%s %s", LoraTxRxTestCommand[1], LoraTxRxValue[1]);
			ret = LoraCommSend(result, strlen(result), TARGET_1);
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);

			while ( gLoraRecvLen < 5 )
			{
				DELAY_SleepMS(1);
				timeout++;
				if(timeout > 5000)
				{
					PRINTF("time out!!\r\n");
					timeout = 0;
					return -1;
				}
				__NOP();
			}
			
			
			DELAY_SleepMS(1000);
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			while ( gLoraRecvLen < 20 )
			{
				DELAY_SleepMS(1);
				timeout++;
				if(timeout > 5000)
				{
					PRINTF("time out!!\r\n");
					timeout = 0;
					return -1;
				}
				__NOP();
			}
				
			DELAY_SleepMS(100);
			
			ptr = strtok(gLoraBuffer, " ");
			ptr = strtok(NULL, " ");
			ptr = strtok(NULL, " ");
			if(memcmp(ptr, "error\r\n", strlen(ptr)) != 0 || memcmp(ptr, NULL, strlen(ptr)) != 0)
			{
				PRINTF("(TARGET1)RX Test COMPLETE\r\n");
				ret = 0;
			}
			else
			{
				PRINTF("(TARGET1)RX Test FAIL\r\n");
				PRINTF("(TARGET1)RX Buffer:%s\r\n", ptr);
				ret = -1;
			}
			UART_InterruptEnable(UART1, UART_INT_MASK_RX);
			
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			memset(result, 0, strlen(result));
		}
	}
	else if(change == TxRxTest_2)
	{
		if(tar == TARGET_1)
		{
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			sprintf(result, "%s %s", LoraTxRxTestCommand[0], LoraTxRxValue[0]);
			ret = LoraCommSend(result, strlen(result), TARGET_1);
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
			
			while ( gLoraRecvLen <  74 )
			{
				DELAY_SleepMS(1);
				timeout++;
				if(timeout > 5000)
				{
					PRINTF("time out!!\r\n");
					timeout = 0;
					return -1;
				}
				__NOP();
			}
				
			DELAY_SleepMS(1000);
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, strlen(gLoraBuffer));
			
			while ( gLoraRecvLen < 18 )
			{
				DELAY_SleepMS(1);
				timeout++;
				if(timeout > 5000)
				{
					PRINTF("time out!!\r\n");
					timeout = 0;
					return -1;
				}
				__NOP();
			}
				
			DELAY_SleepMS(100);
				
			ptr = strtok(gLoraBuffer, " ");
			ptr = strtok(NULL, " ");
			if(memcmp(ptr, "transmitted\r\n", strlen(ptr)) == 0)
			{
				PRINTF("(TARGET1)TX Test COMPLETE\r\n");
				ret = 0;
			}
			else
			{
				PRINTF("(TARGET1)TX Message: %s\r\n", ptr);
				PRINTF("(TARGET1)TX Test FAIL\r\n");
				ret = -1;
			}
			
			
			UART_InterruptDisable(UART3, UART_INT_MASK_RX);
			
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			memset(result, 0, strlen(result));
		}
		else if(tar == TARGET_2)
		{
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			ptr = NULL;
			
			sprintf(result, "%s %s", LoraTxRxTestCommand[1], LoraTxRxValue[1]);
			ret = LoraCommSend(result, strlen(result), TARGET_2);
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);

			while ( gLoraRecvLen < 5 )
			{
				DELAY_SleepMS(1);
				timeout++;
				if(timeout > 5000)
				{
					PRINTF("time out!!\r\n");
					timeout = 0;
					return -1;
				}
				__NOP();
			}
			
			
			DELAY_SleepMS(1000);
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			while ( gLoraRecvLen < 20 )
			{
				DELAY_SleepMS(1);
				timeout++;
				if(timeout > 5000)
				{
					PRINTF("time out!!\r\n");
					timeout = 0;
					return -1;
				}
				__NOP();
			}
				
			DELAY_SleepMS(100);
			
			ptr = strtok(gLoraBuffer, " ");
			ptr = strtok(NULL, " ");
			ptr = strtok(NULL, " ");
			if(memcmp(ptr, "error\r\n", strlen(ptr)) != 0 || memcmp(ptr, NULL, strlen(ptr)) != 0)
			{
				PRINTF("(TARGET2)RX Test COMPLETE\r\n");
				ret = 0;
			}
			else
			{
				PRINTF("(TARGET2)RX Test FAIL\r\n");
				PRINTF("(TARGET2)RX Buffer:%s\r\n", ptr);
				ret = -1;
			}
			UART_InterruptEnable(UART3, UART_INT_MASK_RX);
			
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			memset(result, 0, strlen(result));
		}
	}
	return ret;
}
int LoraTestStart(int type)
{
	int ret = -1;
	int fail_num = 0;
	uint8_t* ptr = {0,};
	
	for(int j = 0; j< Lora_command_MAX; j++ )
	{
		ret = LoraCommRead(j, &readed);
		if(ret != 0)
		{
			PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
			return ret;
		}
		readLen = strlen(readed);
		
		gLoraRecvLen = 0;		
		
		if(type == 0)
		{
			ret = LoraCommSend(readed, readLen, TARGET_1);
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
		}
		else
		{
			ret = LoraCommSend(readed, readLen, TARGET_2);
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
		}
        while ( gLoraRecvLen < strlen(LoraCommandList[j]) )
            __NOP();
			
		DELAY_SleepMS(100);
		ret = LoraTestCmp(j, type);
		if(ret == 1)
			fail_num++;
	}
	return fail_num;
}