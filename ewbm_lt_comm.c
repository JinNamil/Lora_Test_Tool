#include <stdarg.h>
#include <string.h>
#include "MS500.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "ewbm_lt_comm.h"

uint8_t* readed = {0,};
unsigned int readLen = 0;
int gLoraRecvLen = 0;
int gLoraRecvLen_t = 0;
unsigned char gLoraBuffer[64] = { 0, };
unsigned char CommReceiveBuffer[Lora_command_MAX] = {0, };
static void ltCommReceive(void* param)
{
    gLoraBuffer[gLoraRecvLen++] = UART_Read(UART3);
}

static void ltCommReceive_t(void* param)
{
    gLoraBuffer[gLoraRecvLen++] = UART_Read(UART1);
}

static int ltCommSend(unsigned char* in, unsigned int inLen)
{
	int ret = -1;
	for(int i = 0; i<inLen+2; i++)
	{
		UART_Write(UART3, in[i]);
	}
	
	UART_Write(UART3, '\r');
	UART_Write(UART3, '\n');
	
	ret = 0;
    return ret;
}

static int ltCommSend_t(unsigned char* in, unsigned int inLen)
{
	int ret = -1;
	for(int i = 0; i<inLen+2; i++)
	{
		UART_Write(UART1, in[i]);
	}
	
	UART_Write(UART1, '\r');
	UART_Write(UART1, '\n');
	
	ret = 0;
    return ret;
}

int Comm_compare(char* command, int list_num)
{
	int ret = -1;
	
	if(memcmp(command, LoraCommandList[list_num], strlen(LoraCommandList[list_num])) == 0)
		ret = 0;
	else
		ret = -1;
		
	return ret;
}

int Lora_command_init(int RecvLen, char* RecvBuffer, int num)
{
	char* result = {0,};
	int CommNum = -1;
	DELAY_SleepMS(100);
	
	if(num == 1)
	{
		result = strtok(RecvBuffer, " ");

		for(int i=0; i<Lora_command_MAX; i++)
		{
			if(memcmp(LoraCommandName[i], result, strlen(LoraCommandName[i])) == 0)
			{
				CommNum = i;
				goto Lora_command_init;
			}
		}
Lora_command_init:
		
		
		if(memcmp(RecvBuffer, "TYPE", 4) == 0)
		{
			PRINTF("-----------------TYPE change-----------------\r\n");
			return 0;
		}
		else if(memcmp(RecvBuffer, "START", 5) == 0)
		{
			PRINTF("-----------------START-----------------\r\n");
			return 3;
		}
		if(CommNum == -1)
		{
			PRINTF("NOT Command\r\n");
			return 1;
		}
		
		PRINTF("Command Name: %s\r\n", result);
		result = strtok(NULL, " ");
		PRINTF("Command Value: %s\r\n", result);
		
		memset(LoraCommandList[CommNum], 0, strlen(LoraCommandList[CommNum]));
		sprintf(LoraCommandList[CommNum], "%s", result);
//		PRINTF("Complete Command Edit Value is %s\r\n", LoraCommandList[CommNum]);
		PRINTF("Default Edit Complete\r\n");
		return 1;
	}
	

//	if(memcmp(RecvBuffer, "exit", 4) == 0)
//	{
//		PRINTF("------------------EXIT------------------\r\n");
//		return 0;
//	}
//	else if(memcmp(RecvBuffer, "change", 6) == 0)
//	{
//		return 2;
//	}
//	else if(memcmp(RecvBuffer, "edit", 4) == 0)
//	{
//		PRINTF("Please Edit Typing...(CommandName CommandValue)\r\n");
//		return 3;
//	}
	
	
}

int lora_comm_send(int num, uint8_t** read)
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

int lora_test_init(void)
{
	int ret = -1;
	
	UART_InterruptSetCallback(UART1, UART_INT_MASK_RX, ltCommReceive_t, NULL);
	UART_InterruptEnable(UART1, UART_INT_MASK_RX);
	
	UART_InterruptSetCallback(UART3, UART_INT_MASK_RX, ltCommReceive, NULL);
	UART_InterruptEnable(UART3, UART_INT_MASK_RX);
 
	
	PRINTF("------------------Lora Init--------------------\r\n");
	for(int a = 0; a< Lora_command_MAX; a++ )
	{
		PRINTF("%s: %s",LoraCommandName[a], LoraCommandList[a]);
	}
	PRINTF("------------------Lora Init--------------------\r\n\r\n\r\n");
		
	return 0;
}

int lora_test_cmp(int j, int type)
{
	int ret = 0;
	int fail_count = 0;
	unsigned char ltCommReceiveTemp[64] = {0,};
	unsigned char ptr[64] = {0, };
	
	ret = Comm_compare(gLoraBuffer, j);
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
				GPIO_TogglePin(GPIO2, GPIO_PIN_11);
				GPIO_TogglePin(GPIO2, GPIO_PIN_12);
				GPIO_TogglePin(GPIO2, GPIO_PIN_13);
				GPIO_TogglePin(GPIO2, GPIO_PIN_14);
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
				GPIO_TogglePin(GPIO2, GPIO_PIN_11);
				GPIO_TogglePin(GPIO2, GPIO_PIN_12);
				GPIO_TogglePin(GPIO2, GPIO_PIN_13);
				GPIO_TogglePin(GPIO2, GPIO_PIN_14);
				DELAY_SleepMS(200);
			}
		}
		else if(j == DEVEUI)
		{
			PRINTF("%s <OK>\r\n",LoraCommandName[j]);
			goto lora_test_cmp;
		}
		
		
		memcpy(ltCommReceiveTemp, gLoraBuffer, strlen(LoraCommandList[j]));
		PRINTF("\r\n%s [FAIL]\r\n[Current Value]:%s\r\nTyping Please: %s %s\r\n",LoraCommandName[j], ltCommReceiveTemp, LoraCommand[j], LoraCommandList[j]);
		
		fail_count++;
	}
	if(fail_count != 0)
	{
		PRINTF("%s is change\r\n", LoraCommandName[j]);
		sprintf(ptr, "%s %s", LoraCommand[j], LoraCommandList[j]);
//		strcat(LoraCommand[j], ltCommReceiveTemp);
		if(type == 0)
		{
			ret = ltCommSend(ptr, strlen(ptr));
		}
		else
		{
			ret = ltCommSend_t(ptr, strlen(ptr));
		}
		
		while ( gLoraRecvLen < 5 )
			__NOP();
			
		DELAY_SleepMS(100);
	}
	
lora_test_cmp:
	memset(gLoraBuffer, 0, strlen(gLoraBuffer));
	memset(ltCommReceiveTemp, 0, strlen(ltCommReceiveTemp));
	
	return fail_count;
}

int lora_test_edit(int commandNum, int count)
{
	int edit_count = 0;
	int ret;
	memset(gLoraBuffer, 0, sizeof(gLoraBuffer));
	
	for(int i = 0; i<10; i++)
	{
		GPIO_TogglePin(GPIO2, GPIO_PIN_11);
		GPIO_TogglePin(GPIO2, GPIO_PIN_12);
		GPIO_TogglePin(GPIO2, GPIO_PIN_13);
		GPIO_TogglePin(GPIO2, GPIO_PIN_14);
		DELAY_SleepMS(200);
	}
	
	do{
		gLoraRecvLen = 0;
		PRINTF("Please edit message...\r\n");
		while ( gLoraRecvLen < strlen(LoraCommandList[commandNum]) )
				__NOP();
				
		PRINTF("--------VALUE---------\r\n");
		PRINTF("%s", gLoraBuffer);
		PRINTF("----------------------\r\n");
		memset(gLoraBuffer, 0, strlen(gLoraBuffer));
		edit_count++;
	}while(edit_count != count);
	
	ret = 0;
	return ret;
}

int lora_txrx_test(int tar, int change)
{
	char* ptr;
	int ret = -1;
	gLoraRecvLen = 0;
	memset(gLoraBuffer, 0, strlen(gLoraBuffer));
	if(change == 0)
	{
		if(tar == 0)
		{
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			ret = ltCommSend_t(LoraTxRxTest[0], strlen(LoraTxRxTest[0]));
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
			
			while ( gLoraRecvLen <  74 )
				__NOP();
				
			DELAY_SleepMS(1000);
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, strlen(gLoraBuffer));
			
			while ( gLoraRecvLen < 18 )
				__NOP();
				
			DELAY_SleepMS(100);
				
	//		PRINTF("buffer: %sbufferLen: %d\r\n", gLoraBuffer, gLoraRecvLen);
			ptr = strtok(gLoraBuffer, " ");
			ptr = strtok(NULL, " ");
			if(memcmp(ptr, "transmitted\r\n", strlen(ptr)) == 0)
			{
				PRINTF("TX Test COMPLETE\r\n");
				ret = 0;
			}
			else
			{
				PRINTF("TX Message: %s\r\n", ptr);
				PRINTF("TX Test FAIL\r\n");
				ret = -1;
			}
			
			
			UART_InterruptDisable(UART1, UART_INT_MASK_RX);
			
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
		}
		else if(tar == 1)
		{
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			ptr = NULL;
			
			ret = ltCommSend(LoraTxRxTest[1], strlen(LoraTxRxTest[1]));
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);

			while ( gLoraRecvLen < 5 )
				__NOP();
			
			
			DELAY_SleepMS(1000);
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			while ( gLoraRecvLen < 20 )
				__NOP();
				
			DELAY_SleepMS(100);
			
	//		PRINTF("buffer: %s\r\nbufferLen: %d\r\n", gLoraBuffer, gLoraRecvLen);
			ptr = strtok(gLoraBuffer, " ");
			ptr = strtok(NULL, " ");
			ptr = strtok(NULL, " ");
			if(memcmp(ptr, "error\r\n", strlen(ptr)) != 0 || memcmp(ptr, NULL, strlen(ptr)) != 0)
			{
				PRINTF("RX Test COMPLETE\r\n");
				ret = 0;
			}
			else
			{
				PRINTF("RX Test FAIL\r\n");
				PRINTF("RX Buffer:%s\r\n", ptr);
				ret = -1;
			}
			UART_InterruptEnable(UART1, UART_INT_MASK_RX);
			
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
		}
	}
	else if(change == 1)
	{
		if(tar == 0)
		{
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			ret = ltCommSend(LoraTxRxTest[0], strlen(LoraTxRxTest[0]));
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
			
			while ( gLoraRecvLen <  74 )
				__NOP();
				
			DELAY_SleepMS(1000);
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, strlen(gLoraBuffer));
			
			while ( gLoraRecvLen < 18 )
				__NOP();
				
			DELAY_SleepMS(100);
				
	//		PRINTF("buffer: %sbufferLen: %d\r\n", gLoraBuffer, gLoraRecvLen);
			ptr = strtok(gLoraBuffer, " ");
			ptr = strtok(NULL, " ");
			if(memcmp(ptr, "transmitted\r\n", strlen(ptr)) == 0)
			{
				PRINTF("TX Test COMPLETE\r\n");
				ret = 0;
			}
			else
			{
				PRINTF("TX Message: %s\r\n", ptr);
				PRINTF("TX Test FAIL\r\n");
				ret = -1;
			}
			
			
			UART_InterruptDisable(UART3, UART_INT_MASK_RX);
			
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
		}
		else if(tar == 1)
		{
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			ptr = NULL;
			
			ret = ltCommSend_t(LoraTxRxTest[1], strlen(LoraTxRxTest[1]));
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);

			while ( gLoraRecvLen < 5 )
				__NOP();
			
			
			DELAY_SleepMS(1000);
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
			
			while ( gLoraRecvLen < 20 )
				__NOP();
				
			DELAY_SleepMS(100);
			
	//		PRINTF("buffer: %s\r\nbufferLen: %d\r\n", gLoraBuffer, gLoraRecvLen);
			ptr = strtok(gLoraBuffer, " ");
			ptr = strtok(NULL, " ");
			ptr = strtok(NULL, " ");
			if(memcmp(ptr, "error\r\n", strlen(ptr)) != 0 || memcmp(ptr, NULL, strlen(ptr)) != 0)
			{
				PRINTF("RX Test COMPLETE\r\n");
				ret = 0;
			}
			else
			{
				PRINTF("RX Test FAIL\r\n");
				PRINTF("RX Buffer:%s\r\n", ptr);
				ret = -1;
			}
			UART_InterruptEnable(UART3, UART_INT_MASK_RX);
			
			gLoraRecvLen = 0;
			memset(gLoraBuffer, 0, 64);
		}
	}
	return ret;
}
int lora_test_comm(int type)
{
	int ret = -1;
	int fail_num = 0;
	uint8_t* ptr = {0,};
	
	for(int j = 0; j< Lora_command_MAX; j++ )
	{
		ret = lora_comm_send(j, &readed);
		if(ret != 0)
		{
			PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
			return ret;
		}
		readLen = strlen(readed);
		
		gLoraRecvLen = 0;		
		
		if(type == 0)
		{
			ret = ltCommSend(readed, readLen);
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
		}
		else
		{
			ret = ltCommSend_t(readed, readLen);
			if(ret != 0)
				PRINTF("%s:%d:%d\r\n",__func__,__LINE__,ret);
		}
        while ( gLoraRecvLen < strlen(LoraCommandList[j]) )
            __NOP();
			
		DELAY_SleepMS(100);
		ret = lora_test_cmp(j, type);
		if(ret == 1)
			fail_num++;
//			goto lora_test_comm;
	}
//lora_test_comm:	
	return fail_num;
}