#include "l3gd20.h"
#include "log.h"

/* SPI5 GPIO: shared bus with the LCD (SCK=PF7, MISO=PF8, MOSI=PF9), own CS=PC1 */
#define GYRO_CS_PIN   GPIO_PIN_1
#define GYRO_CS_PORT  GPIOC

#define GYRO_CS_LOW()  HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_RESET)
#define GYRO_CS_HIGH() HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET)

/* L3GD20 register map (subset actually used) */
#define L3GD20_WHO_AM_I_ADDR   0x0F
#define L3GD20_CTRL_REG1_ADDR  0x20
#define L3GD20_CTRL_REG4_ADDR  0x23
#define L3GD20_OUT_X_L_ADDR    0x28

#define L3GD20_I_AM_L3GD20     0xD4
#define L3GD20_I_AM_L3GD20_TR  0xD5

/* Address-byte control bits (SPI read/write protocol, see datasheet) */
#define L3GD20_READ_CMD   0x80
#define L3GD20_MULTI_CMD  0x40

/* CTRL_REG1: normal/active mode, ODR1, bandwidth 4, X/Y/Z enabled */
#define L3GD20_CTRL_REG1_VALUE  0x3F
/* CTRL_REG4: continuous update, LSB-first, +-500 dps full scale */
#define L3GD20_CTRL_REG4_VALUE  0x10

/* Sensitivity for the +-500 dps full-scale setting above, in mdps/LSB */
#define L3GD20_SENSITIVITY_500DPS  17.50f

static SPI_HandleTypeDef *_hspi;

static void gyro_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t tx[2] = { reg, value };

    GYRO_CS_LOW();
    HAL_SPI_Transmit(_hspi, tx, sizeof(tx), HAL_MAX_DELAY);
    GYRO_CS_HIGH();
}

static uint8_t gyro_read_reg(uint8_t reg)
{
    uint8_t tx = reg | L3GD20_READ_CMD;
    uint8_t rx = 0;

    GYRO_CS_LOW();
    HAL_SPI_Transmit(_hspi, &tx, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(_hspi, &rx, 1, HAL_MAX_DELAY);
    GYRO_CS_HIGH();

    return rx;
}

static void gyro_read_burst(uint8_t start_reg, uint8_t *buf, uint16_t len)
{
    uint8_t tx = start_reg | L3GD20_READ_CMD | L3GD20_MULTI_CMD;

    GYRO_CS_LOW();
    HAL_SPI_Transmit(_hspi, &tx, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(_hspi, buf, len, HAL_MAX_DELAY);
    GYRO_CS_HIGH();
}

void L3GD20_Init(SPI_HandleTypeDef *hspi)
{
    LOG_DEBUG("L3GD20: init start");

    _hspi = hspi;
    GYRO_CS_HIGH();

    uint8_t id = gyro_read_reg(L3GD20_WHO_AM_I_ADDR);
    if (id != L3GD20_I_AM_L3GD20 && id != L3GD20_I_AM_L3GD20_TR) {
        LOG_ERROR("L3GD20: unexpected WHO_AM_I=0x%02X (gyroscope not detected)", id);
    } else {
        LOG_DEBUG("L3GD20: WHO_AM_I=0x%02X", id);
    }

    gyro_write_reg(L3GD20_CTRL_REG1_ADDR, L3GD20_CTRL_REG1_VALUE);
    gyro_write_reg(L3GD20_CTRL_REG4_ADDR, L3GD20_CTRL_REG4_VALUE);

    /* Datasheet: allow ~100 ms turn-on time after leaving power-down before
       trusting the output. */
    HAL_Delay(100);

    LOG_DEBUG("L3GD20: init done");
}

void L3GD20_ReadDPS(float *x, float *y, float *z)
{
    uint8_t raw[6];
    gyro_read_burst(L3GD20_OUT_X_L_ADDR, raw, sizeof(raw));

    int16_t rx = (int16_t)((raw[1] << 8) | raw[0]);
    int16_t ry = (int16_t)((raw[3] << 8) | raw[2]);
    int16_t rz = (int16_t)((raw[5] << 8) | raw[4]);

    *x = (float)rx * L3GD20_SENSITIVITY_500DPS / 1000.0f;
    *y = (float)ry * L3GD20_SENSITIVITY_500DPS / 1000.0f;
    *z = (float)rz * L3GD20_SENSITIVITY_500DPS / 1000.0f;
}
