
#ifndef __STM32_EXTMEM_CONF__H__
#define __STM32_EXTMEM_CONF__H__

#ifdef __cplusplus
 extern "C" {
#endif


#define EXTMEM_DRIVER_NOR_SFDP   1
#define EXTMEM_DRIVER_PSRAM      0
#define EXTMEM_DRIVER_SDCARD     0
#define EXTMEM_DRIVER_USER       0

#define EXTMEM_SAL_XSPI   		1
#define EXTMEM_SAL_SD     		0

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"
#include "stm32_extmem.h"
#include "stm32_extmem_type.h"
#include "boot/stm32_boot_lrun.h"

extern XSPI_HandleTypeDef hxspi1;
extern XSPI_HandleTypeDef hxspi2;

enum {
    EXTMEMORY_1 = 0,   /* NOR   - XSPI2 */
    EXTMEMORY_2 = 1    /* PSRAM - XSPI1 */
};


#define EXTMEM_HEADER_OFFSET 				0x400
#define EXTMEM_LRUN_SOURCE 					EXTMEMORY_1
#define EXTMEM_LRUN_SOURCE_ADDRESS  		0x00100000u
#define EXTMEM_LRUN_SOURCE_SIZE     		0x80000u	//0x10000u
#define EXTMEM_LRUN_DESTINATION_INTERNAL  	1
#define EXTMEM_LRUN_DESTINATION_ADDRESS 	0x34000000u


extern EXTMEM_DefinitionTypeDef extmem_list_config[1];

#if defined(EXTMEM_C)
EXTMEM_DefinitionTypeDef extmem_list_config[1];
#endif



#ifdef __cplusplus
}
#endif

#endif /* __STM32_EXTMEM_CONF__H__ */
