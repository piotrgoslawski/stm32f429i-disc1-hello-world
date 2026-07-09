#include "main.h"

void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}

/* SPI5: SCK=PF7(AF5), MOSI=PF9(AF5), CS=PC2(out), DC=PD13(out) */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance != SPI5)
        return;

    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_SPI5_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* SPI5 SCK=PF7, MOSI=PF9 */
    gpio.Pin       = GPIO_PIN_7 | GPIO_PIN_9;
    gpio.Mode      = GPIO_MODE_AF_PP;
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GPIOF, &gpio);

    /* CS=PC2, DC=PD13 as push-pull outputs */
    gpio.Pin   = GPIO_PIN_2;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &gpio);

    gpio.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOD, &gpio);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance != SPI5)
        return;

    __HAL_RCC_SPI5_FORCE_RESET();
    __HAL_RCC_SPI5_RELEASE_RESET();

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_7 | GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_13);
}
