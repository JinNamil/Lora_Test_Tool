#include <stdarg.h>
#include <string.h>
#include "MS500.h"
#define LT_FLASH_BASE_ADDR         (0x30000000)
#define LT_FLASH_SAVE_ADDR         (0x30220000)

int ltFlashErase64KB(unsigned int addr);
int ltFlashWriteAddr(unsigned int addr, char* in, unsigned int inLen);
int ltFlashInit(void);
int ltFlashDeinit(void);