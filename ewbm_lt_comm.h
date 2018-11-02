#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "MS500.h"

#define VERSION 2

#pragma pack(push, 1)
typedef struct commandlist {
    char PNM[128];          
    char NJM[128];          
    char CLASS[128];         
    char AJOIN[128];         
    char NWKSKEY[128];         
    char APPSKEY[128];         
    char DADDR[128];         
    char REGION[128];         
    char ADR[128];         
    char DR[128];         
    char MCHTXP[128];         
    char CRYPTO[128];         
    char VER[128];         
    char SAG[128];         
    char CFM[128];         
    char ECHO[128];         
    char DEVEUI[128];
    char TXST[128];
	char RXST[128];
} commandlist;
#pragma pack(pop)

typedef enum LoraTxRxNameNum{
	TXST = 50,
	RXST = 51
}LoraTxRxNameNum;

typedef enum LoraCommandCmp{
	TARGET_SUCCESS = 0,
	TARGET1_DEVEUI_ERROR = 1,
	TARGET1_VER_ERROR = 2,
	TARGET2_DEVEUI_ERROR = -1,
	TARGET2_VER_ERROR = -2,
	TARGET_DEVEUI_ERROR = -1,
	TARGET_VER_ERROR = -2
}LoraCommandCmp;

typedef enum LoraTemp{
	TEST_WAIT = 0,
	TEST_START
}LoraTemp;
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
	DEFAULT,
	TARGET_1_2
}Target_t;

typedef enum TestName{
	CMP_TEST = 0,
	RF_TEST,
	BOOT_TEST,
	RESET,
	RF_TEST_SUCCESS,
	CMP_TEST_SUCCESS
}TestName;

typedef enum BootResult{
	BOOT_PASS = 0,
	BOOT_TARGET1_ERROR,
	BOOT_TARGET2_ERROR
}BootResult;

static char LoraSaveCommand[128] = {0,};

static char* LoraSaveTxRx[128] = {0,};

static const char* LoraCommandName[128] = {
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

static const char* LoraTxRxName[128] = {
	"TXST",
	"RXST"
};

static const char* LoraCommandList[128] = {
    "1",
    "1",
    "A",
	"0",
    "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00",
    "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00",
	"00:00:00:00",
    "6",
	"0",
	"0",
	"",
	"",
	"1.2.1",
	"0.000000",
	"0",
	"0",
	"00:01:01:01:01:01:01:01"
};


static const char* LoraCommand[128] = {
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

static const char* LoraTxRxTestCommand[128] = {
	"AT+TXST",
	"AT+RXST"
};

static const char* LoraTxRxValue[128] = {
	"922500000 14 7 10 1",	//21
	"922500000 7"	//13
};

static int LoraCommSend(unsigned char* in, unsigned int inLen, int target);
int LoraTestCmp(int j, int target);
int LoraTestInit(void);
int LoraCommCompare(char* command, int list_num, char* value);
int LoraLedSetting(int target, int test);
int LoraBootStatus(void);
int LoraCommRead(int num, uint8_t** read);
int LoraTestStart(int target);
int LoraCommandEdit(char* RecvBuffer);
int LoraTxRxTest(int tar, int change);
int LoraFlashInit(void);
int LoraCommandMade(int num, char* value);
int LoraCommandChange(int num, char* value);