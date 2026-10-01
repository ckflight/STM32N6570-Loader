
#include "main.h"
#include "extmem_manager.h"

XSPI_HandleTypeDef hxspi1;
XSPI_HandleTypeDef hxspi2;

void SystemClock_Config2(void);
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);

static void MX_XSPI1_Init(void);
static void MX_XSPI2_Init(void);



/*
 * STM32N6570-DK FSBL - Load and Run
 *
 * External NOR Flash:
 *   0x70000000 : FSBL
 *   0x70100000 : Application
 *
 * FSBL loads the application from external NOR to internal SRAM
 * and starts it with BOOT_Application().
 *
 * Flash programming:
 *   fsbl_trusted.bin -> 0x70000000
 *   app_trusted.bin  -> 0x70100000
 *
 * Signing:
 *   STM32_SigningTool_CLI -bin "${ProjName}.bin" -nk -of 0x80000000 -t fsbl -o "${ProjName}-Trusted.bin" -hv 2.3 -align
 */
volatile uint32_t clock_freq = 0;

int main(void)
{
	// These lines enable attaching debugger to app in flash boot.
	// Power on the circuit then run the STM32N6570-ThreadX-USB-CDC-AI-Face-Detection-Ethernet-FSBL Attach debugger
	__HAL_RCC_BSEC_CLK_ENABLE();
	BSEC->AP_UNLOCK = 0xB4;
	BSEC->DBGCR     = 0xB451B400;

	HAL_Init();

	SystemClock_Config();

	clock_freq = HAL_RCC_GetCpuClockFreq();
	clock_freq = HAL_RCC_GetHCLKFreq();
	clock_freq = HAL_RCC_GetPCLK1Freq();
	clock_freq = HAL_RCC_GetPCLK2Freq();

	PeriphCommonClock_Config();

	MX_XSPI2_Init();

	MX_EXTMEM_MANAGER_Init();

	if (BOOT_OK != BOOT_Application())
	{
	Error_Handler();
	}

	// The code does not enter here since it jumps to the application memory location
	while (1){}

}

void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};

    /*
     * FSBL'de PLL1 aktif ve CPU/SYS clock tarafından kullanılıyor.
     * PLL'leri yeniden configure etmeden önce CPU ve SYSCLK'i
     * geçici olarak HSI'ya geçir.
     */
    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);

    if ((RCC_ClkInitStruct.CPUCLKSource == RCC_CPUCLKSOURCE_IC1) ||
        (RCC_ClkInitStruct.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11))
    {
        RCC_ClkInitStruct.ClockType =
            RCC_CLOCKTYPE_CPUCLK |
            RCC_CLOCKTYPE_SYSCLK;

        RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;

        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }
    }

    /*
     * AI application's original oscillator/PLL configuration
     */
    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI |
        RCC_OSCILLATORTYPE_HSE;

    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;

    /* PLL1 = 800 MHz */
    RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL1.PLLM = 2;
    RCC_OscInitStruct.PLL1.PLLN = 25;
    RCC_OscInitStruct.PLL1.PLLFractional = 0;
    RCC_OscInitStruct.PLL1.PLLP1 = 1;
    RCC_OscInitStruct.PLL1.PLLP2 = 1;

    /* PLL2 */
    RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL2.PLLM = 8;
    RCC_OscInitStruct.PLL2.PLLN = 125;
    RCC_OscInitStruct.PLL2.PLLFractional = 0;
    RCC_OscInitStruct.PLL2.PLLP1 = 1;
    RCC_OscInitStruct.PLL2.PLLP2 = 1;

    /* PLL3 */
    RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL3.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL3.PLLM = 8;
    RCC_OscInitStruct.PLL3.PLLN = 225;
    RCC_OscInitStruct.PLL3.PLLFractional = 0;
    RCC_OscInitStruct.PLL3.PLLP1 = 1;
    RCC_OscInitStruct.PLL3.PLLP2 = 2;

    /* PLL4 */
    RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL4.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL4.PLLM = 8;
    RCC_OscInitStruct.PLL4.PLLN = 225;
    RCC_OscInitStruct.PLL4.PLLFractional = 0;
    RCC_OscInitStruct.PLL4.PLLP1 = 6;
    RCC_OscInitStruct.PLL4.PLLP2 = 6;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /*
     * Final application clocks
     *
     * CPUCLK = IC1 = PLL1 / 1 = 800 MHz
     * SYSCLK = IC2/IC6/IC11
     * HCLK   = SYSCLK / 2 = 200 MHz
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_CPUCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2 |
        RCC_CLOCKTYPE_PCLK4 |
        RCC_CLOCKTYPE_PCLK5;

    RCC_ClkInitStruct.CPUCLKSource =
        RCC_CPUCLKSOURCE_IC1;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_IC2_IC6_IC11;

    /* IC1 = PLL1 / 1 -> CPU = 800 MHz */
    RCC_ClkInitStruct.IC1Selection.ClockSelection =
        RCC_ICCLKSOURCE_PLL1;
    RCC_ClkInitStruct.IC1Selection.ClockDivider = 1;

    /* IC2 = PLL1 / 2 -> 400 MHz */
    RCC_ClkInitStruct.IC2Selection.ClockSelection =
        RCC_ICCLKSOURCE_PLL1;
    RCC_ClkInitStruct.IC2Selection.ClockDivider = 2;

    /* IC6 = PLL2 / 1 */
    RCC_ClkInitStruct.IC6Selection.ClockSelection =
        RCC_ICCLKSOURCE_PLL2;
    RCC_ClkInitStruct.IC6Selection.ClockDivider = 1;

    /* IC11 = PLL3 / 1 */
    RCC_ClkInitStruct.IC11Selection.ClockSelection =
        RCC_ICCLKSOURCE_PLL3;
    RCC_ClkInitStruct.IC11Selection.ClockDivider = 1;

    /* HCLK = 400 / 2 = 200 MHz */
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;

    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
    RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /*
     * XSPI clocks
     */
    RCC_PeriphCLKInitStruct.PeriphClockSelection =
        RCC_PERIPHCLK_XSPI1 |
        RCC_PERIPHCLK_XSPI2;

    RCC_PeriphCLKInitStruct.Xspi1ClockSelection =
        RCC_XSPI1CLKSOURCE_HCLK;

    RCC_PeriphCLKInitStruct.Xspi2ClockSelection =
        RCC_XSPI2CLKSOURCE_HCLK;

    if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPCLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_XSPI1_Init(void)
{
    XSPIM_CfgTypeDef sXspiManagerCfg = {0};
    uint32_t xspi_clk;

    hxspi1.Instance = XSPI1;

    hxspi1.Init.FifoThresholdByte       = 8;
    hxspi1.Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
    hxspi1.Init.MemoryType              = HAL_XSPI_MEMTYPE_APMEM_16BITS;
    hxspi1.Init.MemorySize              = HAL_XSPI_SIZE_256MB;
    hxspi1.Init.ChipSelectHighTimeCycle = 5;
    hxspi1.Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
    hxspi1.Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;
    hxspi1.Init.WrapSize                = HAL_XSPI_WRAP_NOT_SUPPORTED;
    hxspi1.Init.ClockPrescaler          = 3;
    hxspi1.Init.SampleShifting          = HAL_XSPI_SAMPLE_SHIFT_NONE;
    hxspi1.Init.DelayHoldQuarterCycle   = HAL_XSPI_DHQC_ENABLE;
    hxspi1.Init.ChipSelectBoundary      = HAL_XSPI_BONDARYOF_16KB;
    hxspi1.Init.MaxTran                 = 0;

    xspi_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI1);

    hxspi1.Init.Refresh =
        ((2U * (xspi_clk / hxspi1.Init.ClockPrescaler)) / 1000000U) - 4U;

#if defined(OCTOSPI_DCR1_DLYBYP)
    hxspi1.Init.DelayBlockBypass = HAL_XSPI_DELAY_BLOCK_BYPASS;
#endif

    hxspi1.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;

    if (HAL_XSPI_Init(&hxspi1) != HAL_OK)
    {
        Error_Handler();
    }

    sXspiManagerCfg.nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
    sXspiManagerCfg.IOPort      = HAL_XSPIM_IOPORT_1;
    sXspiManagerCfg.Req2AckTime = 1;

    if (HAL_XSPIM_Config(&hxspi1,
                         &sXspiManagerCfg,
                         HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_XSPI2_Init(void)
{

  XSPIM_CfgTypeDef sXspiManagerCfg = {0};

  /* XSPI2 parameter configuration*/
  hxspi2.Instance 						= XSPI2;
  hxspi2.Init.FifoThresholdByte 		= 4;
  hxspi2.Init.MemoryMode 				= HAL_XSPI_SINGLE_MEM;
  hxspi2.Init.MemoryType 				= HAL_XSPI_MEMTYPE_MACRONIX;
  hxspi2.Init.MemorySize 				= HAL_XSPI_SIZE_1GB;
  hxspi2.Init.ChipSelectHighTimeCycle 	= 1;
  hxspi2.Init.FreeRunningClock 			= HAL_XSPI_FREERUNCLK_DISABLE;
  hxspi2.Init.ClockMode 				= HAL_XSPI_CLOCK_MODE_0;
  hxspi2.Init.WrapSize 					= HAL_XSPI_WRAP_NOT_SUPPORTED;
  hxspi2.Init.ClockPrescaler 			= 0;
  hxspi2.Init.SampleShifting 			= HAL_XSPI_SAMPLE_SHIFT_NONE;
  hxspi2.Init.DelayHoldQuarterCycle		= HAL_XSPI_DHQC_DISABLE;
  hxspi2.Init.ChipSelectBoundary 		= HAL_XSPI_BONDARYOF_NONE;
  hxspi2.Init.MaxTran 					= 0;
  hxspi2.Init.Refresh 					= 0;
  hxspi2.Init.MemorySelect 				= HAL_XSPI_CSSEL_NCS1;
  if (HAL_XSPI_Init(&hxspi2) != HAL_OK)
  {
    Error_Handler();
  }
  sXspiManagerCfg.nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
  sXspiManagerCfg.IOPort = HAL_XSPIM_IOPORT_2;
  sXspiManagerCfg.Req2AckTime = 1;
  if (HAL_XSPIM_Config(&hxspi2, &sXspiManagerCfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
