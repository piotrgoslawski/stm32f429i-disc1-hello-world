#ifndef LOG_H
#define LOG_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
} LogLevel;

/* Attaches the UART used for log output; must stay initialized for the life of the program. */
void Log_Init(UART_HandleTypeDef *huart);

/* Messages below this level are dropped. Default is LOG_LEVEL_TRACE (everything printed). */
void Log_SetLevel(LogLevel level);

void Log_Write(LogLevel level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#define LOG_TRACE(...) Log_Write(LOG_LEVEL_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) Log_Write(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  Log_Write(LOG_LEVEL_INFO,  __VA_ARGS__)
#define LOG_WARN(...)  Log_Write(LOG_LEVEL_WARN,  __VA_ARGS__)
#define LOG_ERROR(...) Log_Write(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_FATAL(...) Log_Write(LOG_LEVEL_FATAL, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
