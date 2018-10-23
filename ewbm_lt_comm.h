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

typedef enum TxRx_t{
	TxRxTest_1 = 0,
	TxRxTest_2
}TxRx_t;

typedef enum Edit_t{
	EditType = 0,
	EditDefault,
	EditStart
}Edit_t;

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

static const char LoraTxRxName[2][64] = {
	"TXST",
	"RXST"
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

static char LoraTxRxTestCommand[2][64] = {
	"AT+TXST",
	"AT+RXST"
};

static char LoraTxRxValue[2][64] = {
	"922500000 14 7 10 1\r\n",
	"922500000 7\r\n"
};

static int LoraCommSend(unsigned char* in, unsigned int inLen, int target);
int LoraTestCmp(int j, int type);
int LoraTestInit(void);
int LoraCommCompare(char* command, int list_num);
int LoraBootStatus(void);
int LoraCommRead(int num, uint8_t** read);
int LoraTestStart(int type);
int LoraCommandEdit(int RecvLen, char* RecvBuffer);
int LoraTxRxTest(int tar, int change);