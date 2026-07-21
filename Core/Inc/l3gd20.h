#ifndef L3GD20_H
#define L3GD20_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes the L3GD20 MEMS gyroscope over the given SPI bus (SPI5, shared
   with the LCD) and configures it for continuous 500 dps measurement. */
void L3GD20_Init(SPI_HandleTypeDef *hspi);

/* Reads the current angular rate on all three axes, in degrees/second. */
void L3GD20_ReadDPS(float *x, float *y, float *z);

#ifdef __cplusplus
}
#endif

#endif /* L3GD20_H */
