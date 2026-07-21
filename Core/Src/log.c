#include "log.h"
#include <stdarg.h>
#include <stdio.h>

static UART_HandleTypeDef *s_huart;
static LogLevel s_min_level = LOG_LEVEL_TRACE;

static const char *const LEVEL_TAG[] = {
    "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"
};

void Log_Init(UART_HandleTypeDef *huart)
{
    s_huart = huart;
}

void Log_SetLevel(LogLevel level)
{
    s_min_level = level;
}

void Log_Write(LogLevel level, const char *fmt, ...)
{
    if (s_huart == NULL || level < s_min_level)
        return;

    char buf[160];
    int prefix_len = snprintf(buf, sizeof(buf), "[%10lu][%s] ",
                               (unsigned long)HAL_GetTick(), LEVEL_TAG[level]);
    if (prefix_len < 0 || (size_t)prefix_len >= sizeof(buf))
        return;

    va_list args;
    va_start(args, fmt);
    int msg_len = vsnprintf(buf + prefix_len, sizeof(buf) - (size_t)prefix_len, fmt, args);
    va_end(args);
    if (msg_len < 0)
        return;

    size_t len = (size_t)prefix_len + (size_t)msg_len;
    if (len > sizeof(buf) - 2)
        len = sizeof(buf) - 2;

    buf[len++] = '\r';
    buf[len++] = '\n';

    HAL_UART_Transmit(s_huart, (uint8_t *)buf, (uint16_t)len, HAL_MAX_DELAY);
}
