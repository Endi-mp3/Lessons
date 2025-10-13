#include "private.h"

void stud_uart_configure(uint8_t enable, uint8_t mode, uint8_t locale, uint8_t color, uint8_t baud)
{
    UART_CTRL |= (enable & 1) << 0;
    UART_CTRL |= (mode & 3) << 1;
    UART_CTRL |= (locale & 1) << 3;
    UART_CTRL |= (color & 3) << 4;
    UART_CTRL &= ~(3 << 6);

    UART_BAUD |= (baud & 7) << 0;
    UART_BAUD &= ~(0b11111 << 3);
}
