#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MS500.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "ewbm_lt_comm.h"
#include "ewbm_lt_flash.h"

unsigned char* LoraCommandValue;
uint8_t* readed = {0,};
unsigned int readLen = 0;
int gLoraRecvLen = 0;
unsigned char gLoraBuffer[64] = { 0, };
unsigned char CommReceiveBuffer[Lora_command_MAX] = {0, };

commandlist DefaultLoraCommandvalue = {
										.PNM = "1\r\n",                                                  
										.NJM = "1\r\n",             
										.CLASS = "A\r\n",           
										.AJOIN = "0\r\n",           
										.NWKSKEY = "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\r\n",         
										.APPSKEY = "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\r\n",         
										.DADDR = "00:00:00:00\r\n",           
										.REGION = "6\r\n",          
										.ADR = "0\r\n",             
										.DR = "0\r\n",              
										.MCHTXP = "\r\n",           
										.CRYPTO = "0\r\n",          
										.VER = "1.2.1",             
										.SAG = "0.000000\r\n",             
										.CFM = "0\r\n",             
										.ECHO = "0\r\n",            
										.DEVEUI = "00:01:01:01:01:01:01:01\r\n",          
										.TXST = "922500000 14 7 10 1\r\n",
										.RXST = "922500000 7\r\n"
									  };


commandlist LoraCommandvalue = {0,};

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
int LoraCommandMade(int num, char* value)
{
	switch(num)
	{
		case PNM:
			memcpy(value, LoraCommandvalue.PNM, strlen(LoraCommandvalue.PNM));
			break;

		case NJM:
			memcpy(value, LoraCommandvalue.NJM, strlen(LoraCommandvalue.NJM));
			break;

		case CLASS:
			memcpy(value, LoraCommandvalue.CLASS, strlen(LoraCommandvalue.CLASS));
			break;

		case AJOIN:
			memcpy(value, LoraCommandvalue.AJOIN, strlen(LoraCommandvalue.AJOIN));
			break;

		case NWKSKEY:
			memcpy(value, LoraCommandvalue.NWKSKEY, strlen(LoraCommandvalue.NWKSKEY));
			break;

		case APPSKEY:
			memcpy(value, LoraCommandvalue.APPSKEY, strlen(LoraCommandvalue.APPSKEY));
			break;

		case DADDR:
			memcpy(value, LoraCommandvalue.DADDR, strlen(LoraCommandvalue.DADDR));
			break;

		case REGION:
			memcpy(value, LoraCommandvalue.REGION, strlen(LoraCommandvalue.REGION));
			break;

		case ADR:
			memcpy(value, LoraCommandvalue.ADR, strlen(LoraCommandvalue.ADR));
			break;

		case DR:
			memcpy(value, LoraCommandvalue.DR, strlen(LoraCommandvalue.DR));
			break;

		case MCHTXP:
			memcpy(value, LoraCommandvalue.MCHTXP, strlen(LoraCommandvalue.MCHTXP));
			break;

		case CRYPTO:
			memcpy(value, LoraCommandvalue.CRYPTO, strlen(LoraCommandvalue.CRYPTO));
			break;

		case VER:
			memcpy(value, LoraCommandvalue.VER, strlen(LoraCommandvalue.VER));
			break;

		case SAG:
			memcpy(value, LoraCommandvalue.SAG, strlen(LoraCommandvalue.SAG));
			break;

		case CFM:
			memcpy(value, LoraCommandvalue.CFM, strlen(LoraCommandvalue.CFM));
			break;

		case ECHO:
			memcpy(value, LoraCommandvalue.ECHO, strlen(LoraCommandvalue.ECHO));
			break;

		case DEVEUI:
			memcpy(value, LoraCommandvalue.DEVEUI, strlen(LoraCommandvalue.DEVEUI));
			break;
		
		case TXST:
			memcpy(value, LoraCommandvalue.TXST, strlen(DefaultLoraCommandvalue.TXST));
			break;
		
		case RXST:
			memcpy(value, LoraCommandvalue.RXST, strlen(DefaultLoraCommandvalue.RXST));
			break;
			
		default:
			break;
	}
	return 0;
}

int LoraCommCompare(char* command, int list_num, char* value)
{
	int ret = -1;
	
	if(memcmp(command, value, strlen(value)) == 0)
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

int LoraInFlash(int offset, unsigned char* in)
{
	int ret = -1;
	int off = 0;

	char value[128] = {0, };
	memcpy(&LoraCommandvalue, (unsigned char*)LT_FLASH_SAVE_ADDR, sizeof(commandlist));
	
	ret = LoraCommandChange(offset, in);
	if(offset == TXST)
	{
		ret = LoraCommandMade(RXST, value);
		memcpy(value, in, 9);
		memcpy(value+10, in+13, 1);
		ret = LoraCommandChange(RXST, value);
	}
	else if(offset == RXST)
	{
		ret = LoraCommandMade(TXST, value);
		memcpy(value, in, 9);
		memcpy(value+13, in+10, 1);
		ret = LoraCommandChange(TXST, value);
	}

	ret = ltFlashWriteAddr(LT_FLASH_SAVE_ADDR, (char*)&LoraCommandvalue, sizeof(struct commandlist));
	if(ret != 0)
		PRINTF("ERROR!\r\n");
	
	memset(value, 0, strlen(value));
	return ret;
}
int LoraCommandEdit(int RecvLen, char* RecvBuffer)
{
	char* result = {0,};
	int ret = -1;
	int CommNum = -1;
	int txrx = 0;
	int offset = 0;
	char txrxResult[64] = {0,};
	DELAY_SleepMS(100);
	char value[128] = {0, };
	memcpy(&LoraCommandvalue, (unsigned char*)LT_FLASH_SAVE_ADDR, sizeof(commandlist));
	
	result = strtok(RecvBuffer, " ");

	for(int i=0; i<Lora_command_MAX; i++)
	{
		if(memcmp(LoraCommandName[i], result, strlen(LoraCommandName[i])) == 0)
		{
			CommNum = i;
			goto LoraCommandEdit;
		}
		offset += strlen(LoraCommandList[i]);
	}
LoraCommandEdit:
	
	/* TxRx compare */
	if(memcmp(LoraTxRxName[0], RecvBuffer, strlen(LoraTxRxName[0])) == 0)
	{
		memcpy(txrxResult, RecvBuffer+(strlen(LoraTxRxName[0])+1), strlen(LoraTxRxValue[0]));
		
		ret = LoraInFlash(TXST, txrxResult);
		if(ret != 0)
			PRINTF("ERROR %s,#%d\r\n",__func__,__LINE__);
			
		offset += strlen(LoraTxRxValue[0]);
		
		PRINTF("TXST Default Edit Change Complete\r\n");
		
		free(LoraCommandValue);
		return EditDefault;
	}
	else if(memcmp(LoraTxRxName[1], RecvBuffer, strlen(LoraTxRxName[1])) == 0)
	{
		memcpy(txrxResult, RecvBuffer+(strlen(LoraTxRxName[1])+1), strlen(LoraTxRxValue[1]));
		
		offset += strlen(LoraTxRxValue[0]);	
		
		ret = LoraInFlash(RXST, txrxResult);
		if(ret != 0)
			PRINTF("ERROR %s,#%d\r\n",__func__,__LINE__);
		
		offset += strlen(LoraTxRxValue[1]);
		
		PRINTF("RXST Default Edit Change Complete\r\n");
		
		free(LoraCommandValue);
		return EditDefault;
	}
	
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
	else if(memcmp(RecvBuffer, "DEFAULT", 7) == 0)
	{
		ret = LoraFlashInit();
		PRINTF("-----------------RESET COMMAND-----------------\r\n");
		return EditDefault;
	}
	if(CommNum == -1)
	{
		PRINTF("NOT Command\r\n");
		return EditDefault;
	}
	
	
	PRINTF("Command Name: %s\r\n", result);
	result = strtok(NULL, " ");
	PRINTF("Command Value: %s\r\n", result);
	ret = LoraInFlash(CommNum, result);
	if(ret != 0)
		PRINTF("ERROR %s,#%d\r\n",__func__,__LINE__);
		
	PRINTF("Default Edit Complete\r\n");
	return EditDefault;
}

int LoraCommRead(int num, uint8_t** read)
{
	int ret = -1;
	
	if(num >= Lora_command_MAX)
	{
		PRINTF("%s:%d\r\n",__func__,__LINE__);
		ret = -1;
		return ret;
	}
	
	*read = (uint8_t*)LoraCommand[num];
	ret = 0;
	return ret;
}

int LoraFlashInit(void)
{
	int ret = -1;
	
	ret = ltFlashWriteAddr(LT_FLASH_SAVE_ADDR, (char*)&DefaultLoraCommandvalue, sizeof(commandlist));
	if(ret != 0)
		PRINTF("ERROR!\r\n");
	
	return ret;
}

int LoraCommandChange(int num, char* value)
{
	switch(num)
	{
		case PNM:
			memset(LoraCommandvalue.PNM, 0, strlen(LoraCommandvalue.PNM));
			memcpy(LoraCommandvalue.PNM, value, strlen(DefaultLoraCommandvalue.PNM));
			break;

		case NJM:
			memset(LoraCommandvalue.NJM, 0, strlen(LoraCommandvalue.NJM));
			memcpy(LoraCommandvalue.NJM, value, strlen(DefaultLoraCommandvalue.NJM));
			break;

		case CLASS:
			memset(LoraCommandvalue.CLASS, 0, strlen(LoraCommandvalue.CLASS));
			memcpy(LoraCommandvalue.CLASS, value, strlen(DefaultLoraCommandvalue.CLASS));
			break;

		case AJOIN:
			memset(LoraCommandvalue.AJOIN, 0, strlen(LoraCommandvalue.AJOIN));
			memcpy(LoraCommandvalue.AJOIN, value, strlen(DefaultLoraCommandvalue.AJOIN));
			break;

		case NWKSKEY:
			memset(LoraCommandvalue.NWKSKEY, 0, strlen(LoraCommandvalue.NWKSKEY));
			memcpy(LoraCommandvalue.NWKSKEY, value, strlen(DefaultLoraCommandvalue.NWKSKEY));
			break;

		case APPSKEY:
			memset(LoraCommandvalue.APPSKEY, 0, strlen(LoraCommandvalue.APPSKEY));
			memcpy(LoraCommandvalue.APPSKEY, value, strlen(DefaultLoraCommandvalue.APPSKEY));
			break;

		case DADDR:
			memset(LoraCommandvalue.DADDR, 0, strlen(LoraCommandvalue.DADDR));
			memcpy(LoraCommandvalue.DADDR, value, strlen(DefaultLoraCommandvalue.DADDR));
			break;

		case REGION:
			memset(LoraCommandvalue.REGION, 0, strlen(LoraCommandvalue.REGION));
			memcpy(LoraCommandvalue.REGION, value, strlen(DefaultLoraCommandvalue.REGION));
			break;

		case ADR:
			memset(LoraCommandvalue.ADR, 0, strlen(LoraCommandvalue.ADR));
			memcpy(LoraCommandvalue.ADR, value, strlen(DefaultLoraCommandvalue.ADR));
			break;

		case DR:
			memset(LoraCommandvalue.DR, 0, strlen(LoraCommandvalue.DR));
			memcpy(LoraCommandvalue.DR, value, strlen(DefaultLoraCommandvalue.DR));
			break;

		case MCHTXP:
			memset(LoraCommandvalue.MCHTXP, 0, strlen(LoraCommandvalue.MCHTXP));
			memcpy(LoraCommandvalue.MCHTXP, value, strlen(DefaultLoraCommandvalue.MCHTXP));
			break;

		case CRYPTO:
			memset(LoraCommandvalue.CRYPTO, 0, strlen(LoraCommandvalue.CRYPTO));
			memcpy(LoraCommandvalue.CRYPTO, value, strlen(DefaultLoraCommandvalue.CRYPTO));
			break;

		case VER:
			memset(LoraCommandvalue.VER, 0, strlen(LoraCommandvalue.VER));
			memcpy(LoraCommandvalue.VER, value, strlen(DefaultLoraCommandvalue.VER));
			break;

		case SAG:
			memset(LoraCommandvalue.SAG, 0, strlen(LoraCommandvalue.SAG));
			memcpy(LoraCommandvalue.SAG, value, strlen(DefaultLoraCommandvalue.SAG));
			break;

		case CFM:
			memset(LoraCommandvalue.CFM, 0, strlen(LoraCommandvalue.CFM));
			memcpy(LoraCommandvalue.CFM, value, strlen(DefaultLoraCommandvalue.CFM));
			break;

		case ECHO:
			memset(LoraCommandvalue.ECHO, 0, strlen(LoraCommandvalue.ECHO));
			memcpy(LoraCommandvalue.ECHO, value, strlen(DefaultLoraCommandvalue.ECHO));
			break;

		case DEVEUI:
			memset(LoraCommandvalue.DEVEUI, 0, strlen(LoraCommandvalue.DEVEUI));
			memcpy(LoraCommandvalue.DEVEUI, value, strlen(DefaultLoraCommandvalue.DEVEUI));
			break;
		
		case TXST:
			memset(LoraCommandvalue.TXST, 0, strlen(LoraCommandvalue.TXST));
			memcpy(LoraCommandvalue.TXST, value, strlen(DefaultLoraCommandvalue.TXST));
			break;
		
		case RXST:
			memset(LoraCommandvalue.RXST, 0, strlen(LoraCommandvalue.RXST));
			memcpy(LoraCommandvalue.RXST, value, strlen(DefaultLoraCommandvalue.RXST));
			break;
			
		default:
			break;
	}
	
	return 0;
}

int LoraTestInit(void)
{
	int ret = -1;
	ret = ltFlashInit();
	char value[128] = {0,};
	memcpy(&LoraCommandvalue, (unsigned char*)LT_FLASH_SAVE_ADDR, sizeof(commandlist));
	
	PRINTF("------------------Lora Init--------------------\r\n");
	for(int i = 0; i < Lora_command_MAX; i++)
	{
		LoraCommandMade(i, value);
		
		if(i == 12)
			PRINTF("%s: %s\r\n", LoraCommandName[i], value);
		else
			PRINTF("%s: %s", LoraCommandName[i], value);
			
		memset(value, 0, strlen(value));
	}
	
	
	for(int j = 0; j < 2; j++)
	{
		switch(j)
		{
			case 0:
				memcpy(value, LoraCommandvalue.TXST, strlen(LoraCommandvalue.TXST));
				break;
			case 1:
				memcpy(value, LoraCommandvalue.RXST, strlen(LoraCommandvalue.RXST));
				break;
		}
		PRINTF("%s: %s", LoraTxRxName[j], value);
		
		memset(value, 0, strlen(value));
	}
	
	UART_InterruptSetCallback(UART1, UART_INT_MASK_RX, LoraCommReceive_t, NULL);
	UART_InterruptEnable(UART1, UART_INT_MASK_RX);
	
	UART_InterruptSetCallback(UART3, UART_INT_MASK_RX, LoraCommReceive, NULL);
	UART_InterruptEnable(UART3, UART_INT_MASK_RX);
	return 0;
}

int LoraTestCmp(int j, int type)
{
	int ret = -1;
	int fail_count = 0;
	unsigned char ltCommReceiveTemp[64] = {0,};
	unsigned char ptr[64] = {0, };
	char value[128] = {0,};
	
	memcpy(&LoraCommandvalue, (unsigned char*)LT_FLASH_SAVE_ADDR, sizeof(commandlist));
	
	LoraCommandMade(j, value);
	
	ret = LoraCommCompare(gLoraBuffer, j, value);
	if(ret == 0)
	{
		DELAY_SleepMS(10);
		memcpy(ltCommReceiveTemp, gLoraBuffer, strlen(value));
		if(j == DEVEUI)
		{
			PRINTF("%s ERROR!!\r\n", LoraCommandName[j]);
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
			PRINTF("%s ERROR!!\r\n", LoraCommandName[j]);
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
		
		
		memcpy(ltCommReceiveTemp, gLoraBuffer, strlen(value));
		PRINTF("\r\n[Current Value]:%s\r\n%s Value Change: %s\r\n", ltCommReceiveTemp, LoraCommandName[j], value);
		
		fail_count++;
	}
	if(fail_count != 0)
	{
		PRINTF("%s is change\r\n", LoraCommandName[j]);
		sprintf(ptr, "%s %s", LoraCommand[j], value);
		
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
	memset(value, 0, strlen(value));
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