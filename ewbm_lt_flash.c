#include <stdarg.h>
#include <string.h>
#include "MS500.h"
#include "ewbm_lt_flash.h"
#include "hal_flash.h"

int ltFlashWriteAddr(unsigned int addr, char* in, unsigned int inLen)
{
    int ret = -1;
	
	FLASH_SectorErase((addr-LT_FLASH_BASE_ADDR)>>12);
	
    FLASH_Write((uint32_t)addr, (uint8_t*)in, inLen);

    ret = memcmp(in, (void*)addr, inLen);
    if ( ret != 0 )
    {
		PRINTF("==============\r\n%s\r\n\r\n%s==============",(unsigned char*)addr, in);
        PRINTF("%s#%d", __FUNCTION__, __LINE__);
        PRINTF("%s#%d", __FUNCTION__, __LINE__);
        ret = -1;
        return ret;
    }

    ret = 0;
    return ret;
}

int ltFlashInit(void)
{
    int ret = -1;
    FLASH_SetMode(FLASH_MODE_QUAD);
    ret = 0;
    return ret;
}

int ltFlashDeinit(void)
{
    int ret = -1;
    ret = 0;
    return ret;
}