
#include "extmem_manager.h"
#include <string.h>

void MX_EXTMEM_MANAGER_Init(void)
{
    memset(extmem_list_config, 0x0, sizeof(extmem_list_config));

    /* NOR - XSPI2 */
    extmem_list_config[EXTMEMORY_1].MemType    = EXTMEM_NOR_SFDP;
    extmem_list_config[EXTMEMORY_1].Handle     = (void *)&hxspi2;
    extmem_list_config[EXTMEMORY_1].ConfigType = EXTMEM_LINK_CONFIG_8LINES;

    EXTMEM_Init(EXTMEMORY_1, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2));

    /* PSRAM - XSPI1 */
// EXTMEM_DRIVER_PSRAM_Init is not implemented
//    extmem_list_config[EXTMEMORY_2].MemType    = EXTMEM_PSRAM;
//    extmem_list_config[EXTMEMORY_2].Handle     = (void *)&hxspi1;
//    extmem_list_config[EXTMEMORY_2].ConfigType = EXTMEM_LINK_CONFIG_16LINES;
//
//    EXTMEM_Init(EXTMEMORY_2, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI1));
}
