
#include "main.h"

void HAL_MspInit(void)
{

  HAL_PWREx_EnableVddA();

  HAL_PWREx_EnableVddIO2();

  HAL_PWREx_EnableVddIO3();

  HAL_PWREx_EnableVddIO4();

  HAL_PWREx_EnableVddIO5();

}

void HAL_XSPI_MspInit(XSPI_HandleTypeDef *hxspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* ============================================================
     * XSPI1 - APS256XX PSRAM
     * ============================================================ */
    if (hxspi->Instance == XSPI1)
    {
        /* PSRAM I/O supply = 1.8 V */
        __HAL_RCC_PWR_CLK_ENABLE();

        HAL_PWREx_EnableVddIO2();
        HAL_PWREx_ConfigVddIORange(PWR_VDDIO2, PWR_VDDIO_RANGE_1V8);

        /* XSPI1 peripheral clock */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_XSPI1;
        PeriphClkInitStruct.Xspi1ClockSelection = RCC_XSPI1CLKSOURCE_HCLK;

        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        /* XSPI1 clock */
        __HAL_RCC_XSPI1_CLK_ENABLE();

        /* XSPI1 reset */
        __HAL_RCC_XSPI1_FORCE_RESET();
        __HAL_RCC_XSPI1_RELEASE_RESET();

        /* XSPIM clock + reset */
        __HAL_RCC_XSPIM_CLK_ENABLE();

        __HAL_RCC_XSPIM_FORCE_RESET();
        __HAL_RCC_XSPIM_RELEASE_RESET();

        /* GPIO clocks */
        __HAL_RCC_GPIOO_CLK_ENABLE();
        __HAL_RCC_GPIOP_CLK_ENABLE();

        /*
         * GPIOO
         *
         * PO0 -> XSPIM_P1_NCS1
         * PO2 -> XSPIM_P1_DQS0
         * PO3 -> XSPIM_P1_DQS1
         * PO4 -> XSPIM_P1_CLK
         */
        GPIO_InitStruct.Pin =
                GPIO_PIN_0 |
                GPIO_PIN_2 |
                GPIO_PIN_3 |
                GPIO_PIN_4;

        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P1;

        HAL_GPIO_Init(GPIOO, &GPIO_InitStruct);

        /*
         * GPIOP
         *
         * PP0..PP15 -> XSPIM_P1_IO0..IO15
         */
        GPIO_InitStruct.Pin =
                GPIO_PIN_0  |
                GPIO_PIN_1  |
                GPIO_PIN_2  |
                GPIO_PIN_3  |
                GPIO_PIN_4  |
                GPIO_PIN_5  |
                GPIO_PIN_6  |
                GPIO_PIN_7  |
                GPIO_PIN_8  |
                GPIO_PIN_9  |
                GPIO_PIN_10 |
                GPIO_PIN_11 |
                GPIO_PIN_12 |
                GPIO_PIN_13 |
                GPIO_PIN_14 |
                GPIO_PIN_15;

        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P1;

        HAL_GPIO_Init(GPIOP, &GPIO_InitStruct);
    }

    /* ============================================================
     * XSPI2 - MX66UW1G45G NOR
     * ============================================================ */
    else if (hxspi->Instance == XSPI2)
    {
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_XSPI2;
        PeriphClkInitStruct.Xspi2ClockSelection = RCC_XSPI2CLKSOURCE_IC3;
        PeriphClkInitStruct.ICSelection[RCC_IC3].ClockSelection =
                RCC_ICCLKSOURCE_PLL1;
        PeriphClkInitStruct.ICSelection[RCC_IC3].ClockDivider = 24;

        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        /* Peripheral clocks */
        __HAL_RCC_XSPIM_CLK_ENABLE();
        __HAL_RCC_XSPI2_CLK_ENABLE();

        /* GPIO clock */
        __HAL_RCC_GPION_CLK_ENABLE();

        /*
         * XSPI2 GPIO Configuration
         *
         * PN4  -> XSPIM_P2_IO2
         * PN6  -> XSPIM_P2_CLK
         * PN8  -> XSPIM_P2_IO4
         * PN0  -> XSPIM_P2_DQS0
         * PN3  -> XSPIM_P2_IO1
         * PN5  -> XSPIM_P2_IO3
         * PN1  -> XSPIM_P2_NCS1
         * PN9  -> XSPIM_P2_IO5
         * PN2  -> XSPIM_P2_IO0
         * PN10 -> XSPIM_P2_IO6
         * PN11 -> XSPIM_P2_IO7
         */
        GPIO_InitStruct.Pin =
                OCTOSPI_IO2_Pin |
                OCTOSPI_CLK_Pin |
                OCTOSPI_IO4_Pin |
                OCTOSPI_DQS_Pin |
                OCTOSPI_IO1_Pin |
                OCTOSPI_IO3_Pin |
                OCTOSPI_NCS_Pin |
                OCTOSPI_IO5_Pin |
                OCTOSPI_IO0_Pin |
                OCTOSPI_IO6_Pin |
                OCTOSPI_IO7_Pin;

        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P2;

        HAL_GPIO_Init(GPION, &GPIO_InitStruct);
    }
}


void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef *hxspi)
{
    /* ============================================================
     * XSPI1 - APS256XX PSRAM
     * ============================================================ */
    if (hxspi->Instance == XSPI1)
    {
        /* Control pins */
        HAL_GPIO_DeInit(
                GPIOO,
                GPIO_PIN_0 |
                GPIO_PIN_2 |
                GPIO_PIN_3 |
                GPIO_PIN_4);

        /* IO0..IO15 */
        HAL_GPIO_DeInit(
                GPIOP,
                GPIO_PIN_0  |
                GPIO_PIN_1  |
                GPIO_PIN_2  |
                GPIO_PIN_3  |
                GPIO_PIN_4  |
                GPIO_PIN_5  |
                GPIO_PIN_6  |
                GPIO_PIN_7  |
                GPIO_PIN_8  |
                GPIO_PIN_9  |
                GPIO_PIN_10 |
                GPIO_PIN_11 |
                GPIO_PIN_12 |
                GPIO_PIN_13 |
                GPIO_PIN_14 |
                GPIO_PIN_15);

        __HAL_RCC_XSPI1_FORCE_RESET();
        __HAL_RCC_XSPI1_RELEASE_RESET();

        __HAL_RCC_XSPI1_CLK_DISABLE();
    }

    /* ============================================================
     * XSPI2 - MX66UW1G45G NOR
     * ============================================================ */
    else if (hxspi->Instance == XSPI2)
    {
        __HAL_RCC_XSPI2_CLK_DISABLE();

        HAL_GPIO_DeInit(
                GPION,
                OCTOSPI_IO2_Pin |
                OCTOSPI_CLK_Pin |
                OCTOSPI_IO4_Pin |
                OCTOSPI_DQS_Pin |
                OCTOSPI_IO1_Pin |
                OCTOSPI_IO3_Pin |
                OCTOSPI_NCS_Pin |
                OCTOSPI_IO5_Pin |
                OCTOSPI_IO0_Pin |
                OCTOSPI_IO6_Pin |
                OCTOSPI_IO7_Pin);
    }
}

//void HAL_XSPI_MspInit(XSPI_HandleTypeDef* hxspi)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
//  if(hxspi->Instance==XSPI2)
//  {
//
//  /** Initializes the peripherals clock
//  */
//    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_XSPI2;
//    PeriphClkInitStruct.Xspi2ClockSelection = RCC_XSPI2CLKSOURCE_IC3;
//    PeriphClkInitStruct.ICSelection[RCC_IC3].ClockSelection = RCC_ICCLKSOURCE_PLL1;
//    PeriphClkInitStruct.ICSelection[RCC_IC3].ClockDivider = 24;
//    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//    {
//      Error_Handler();
//    }
//
//    /* Peripheral clock enable */
//    __HAL_RCC_XSPIM_CLK_ENABLE();
//    __HAL_RCC_XSPI2_CLK_ENABLE();
//
//    __HAL_RCC_GPION_CLK_ENABLE();
//    /**XSPI2 GPIO Configuration
//    PN4     ------> XSPIM_P2_IO2
//    PN6     ------> XSPIM_P2_CLK
//    PN8     ------> XSPIM_P2_IO4
//    PN0     ------> XSPIM_P2_DQS0
//    PN3     ------> XSPIM_P2_IO1
//    PN5     ------> XSPIM_P2_IO3
//    PN1     ------> XSPIM_P2_NCS1
//    PN9     ------> XSPIM_P2_IO5
//    PN2     ------> XSPIM_P2_IO0
//    PN10     ------> XSPIM_P2_IO6
//    PN11     ------> XSPIM_P2_IO7
//    */
//    GPIO_InitStruct.Pin = OCTOSPI_IO2_Pin|OCTOSPI_CLK_Pin|OCTOSPI_IO4_Pin|OCTOSPI_DQS_Pin
//                          |OCTOSPI_IO1_Pin|OCTOSPI_IO3_Pin|OCTOSPI_NCS_Pin|OCTOSPI_IO5_Pin
//                          |OCTOSPI_IO0_Pin|OCTOSPI_IO6_Pin|OCTOSPI_IO7_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P2;
//    HAL_GPIO_Init(GPION, &GPIO_InitStruct);
//
//  }
//
//}
//
//void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef* hxspi)
//{
//  if(hxspi->Instance==XSPI2)
//  {
//
//    /* Peripheral clock disable */
//    __HAL_RCC_XSPIM_CLK_DISABLE();
//    __HAL_RCC_XSPI2_CLK_DISABLE();
//
//    /**XSPI2 GPIO Configuration
//    PN4     ------> XSPIM_P2_IO2
//    PN6     ------> XSPIM_P2_CLK
//    PN8     ------> XSPIM_P2_IO4
//    PN0     ------> XSPIM_P2_DQS0
//    PN3     ------> XSPIM_P2_IO1
//    PN5     ------> XSPIM_P2_IO3
//    PN1     ------> XSPIM_P2_NCS1
//    PN9     ------> XSPIM_P2_IO5
//    PN2     ------> XSPIM_P2_IO0
//    PN10     ------> XSPIM_P2_IO6
//    PN11     ------> XSPIM_P2_IO7
//    */
//    HAL_GPIO_DeInit(GPION, OCTOSPI_IO2_Pin|OCTOSPI_CLK_Pin|OCTOSPI_IO4_Pin|OCTOSPI_DQS_Pin
//                          |OCTOSPI_IO1_Pin|OCTOSPI_IO3_Pin|OCTOSPI_NCS_Pin|OCTOSPI_IO5_Pin
//                          |OCTOSPI_IO0_Pin|OCTOSPI_IO6_Pin|OCTOSPI_IO7_Pin);
//
//  }
//
//}
