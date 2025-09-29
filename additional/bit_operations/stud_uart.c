#include "private.h"

void stud_uart_configure(uint8_t enable, uint8_t mode, uint8_t locale, uint8_t color, uint8_t baud)
{
    UART_CTRL = 0;
    UART_BAUD = 0;
}

