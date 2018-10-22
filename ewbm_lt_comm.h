#include <stdarg.h>
#include <string.h>
#include "MS500.h"

typedef enum LoraCommandNameNum {
    PNM=0,
	NJM,
	CLASS,
	AJOIN,
	NWKSKEY,
	APPSKEY,
	DADDR,
	REGION,
	ADR,
	DR,
	MCHTXP,
	CRYPTO,
	VER,
	SAG,
	CFM,
	ECHO,
	DEVEUI,
	Lora_command_MAX
} LoraCommandNameNum;

typedef enum Target_t{
	TARGET_1 = 0,
	TARGET_2,
	TARGET_3
}Target_t;
static const char LoraCommandName[Lora_command_MAX][64] = {
	"PNM",
	"NJM",
	"CLASS",
	"AJOIN",
	"NWKSKEY",
	"APPSKEY",
	"DADDR",
	"REGION",
	"ADR",
	"DR",
	"MCHTXP",
	"CRYPTO",
	"VER",
	"SAG",
	"CFM",
	"ECHO",
	"DEVEUI"
};

static char LoraCommandList[Lora_command_MAX][128] = {
    "1\r\n",
    "1\r\n",
    "A\r\n",
	"0\r\n",
    "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\r\n",
    "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\r\n",
	"00:00:00:00\r\n",
    "6\r\n",
	"0\r\n",
	"0\r\n",
	"\r\n",
	"0\r\n",
	"1.2.1-1be8089-Darthvader\r\n",
	"0.000000\r\n",
	"0\r\n",
	"0\r\n",
	"00:01:01:01:01:01:01:01\r\n"
};

static char LoraCommand[Lora_command_MAX][64] = {
	"AT+PNM",
	"AT+NJM",
	"AT+CLASS",
	"AT+AJOIN",
	"AT+NWKSKEY",
	"AT+APPSKEY",
	"AT+DADDR",
	"AT+REGION",
	"AT+ADR",
	"AT+DR",
	"AT+MCHTXP",
	"AT+CRYPTO",
	"AT+VER",
	"AT+SAG",
	"AT+CFM",
	"AT+ECHO",
	"AT+DEVEUI"
};

static char LoraTxRxTest[2][64] = {
	"AT+TXST 922500000 14 7 10 1",
	"AT+RXST 922500000 7"
};
static char LoraCommandSave[Lora_command_MAX][64] = {0,};
static int LoraCommanTemp;
static int ltCommSend(unsigned char* in, unsigned int inLen);
int lora_test_cmp(int j, int type);
int lora_test_cmp_t(int j);
int lora_test_init(void);
int Comm_compare(char* command, int list_num);
int lora_comm_edit(int num, uint8_t** read);
int lora_comm_send(int num, uint8_t** read);
int TotalCmp(void);
int lora_test_comm(int type);
//int Lora_command_init(void);
int test(void);
int Lora_command_init(int RecvLen, char* RecvBuffer, int num);
int lora_txrx_test(int tar, int change);
int lora_test_edit(int commandNum, int count);