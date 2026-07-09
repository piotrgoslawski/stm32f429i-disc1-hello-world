#include "main.h"
#include "stm32f429i_discovery.h"

SPI_HandleTypeDef hspi5;

static void SystemClock_Config(void);
static void SPI5_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    SPI5_Init();
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);

    ILI9341_Init(&hspi5);
    ILI9341_FillScreen(ILI9341_BLUE);

    /* Draw "Hello World!" centered on the 320x240 display */
    const char *msg = "HELLO WORLD!";
    /* 12 chars * 8px/char = 96px wide, centered at x=(320-96)/2=112 */
    ILI9341_DrawString(112, 116, msg, ILI9341_WHITE, ILI9341_BLUE);

    uint32_t led3_last = 0, led4_last = 0;
    while (1) {
        uint32_t now = HAL_GetTick();
        if (now - led3_last >= 125) {
            BSP_LED_Toggle(LED3);
            led3_last = now;
        }
        if (now - led4_last >= 250) {
            BSP_LED_Toggle(LED4);
            led4_last = now;
        }
    }
}


/* 168 MHz using 8 MHz HSE: PLL_M=8, PLL_N=336, PLL_P=2, PLL_Q=7 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    osc.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState            = RCC_HSE_ON;
    osc.PLL.PLLState        = RCC_PLL_ON;
    osc.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLM            = 8;
    osc.PLL.PLLN            = 336;
    osc.PLL.PLLP            = RCC_PLLP_DIV2;
    osc.PLL.PLLQ            = 7;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK)
        Error_Handler();

    clk.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
                       | RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV4;  /* 42 MHz */
    clk.APB2CLKDivider = RCC_HCLK_DIV2;  /* 84 MHz */
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_5) != HAL_OK)
        Error_Handler();
}

/* SPI5 master, 8-bit, CPOL=0, CPHA=0, software NSS, 84/8 = ~10.5 MHz */
static void SPI5_Init(void)
{
    hspi5.Instance               = SPI5;
    hspi5.Init.Mode              = SPI_MODE_MASTER;
    hspi5.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi5.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi5.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi5.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi5.Init.NSS               = SPI_NSS_SOFT;
    hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi5.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi5.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi5.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi5.Init.CRCPolynomial     = 10;
    if (HAL_SPI_Init(&hspi5) != HAL_OK)
        Error_Handler();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}
