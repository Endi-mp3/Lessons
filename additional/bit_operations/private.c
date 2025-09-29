#include "private.h"
#include <stdio.h>

// UART registers
uint8_t UART_CTRL = 0;
uint8_t UART_BAUD = 0;
uint8_t UART_STATUS = 0;

// Reference values for comparison
uint8_t UART_CTRL_REF = 0;
uint8_t UART_BAUD_REF = 0;

void uart_configure(uint8_t enable, uint8_t mode, uint8_t locale, uint8_t color, uint8_t baud) {
    UART_CTRL = 0;
    UART_CTRL |= (enable & 0x01);         // bit 0
    UART_CTRL |= (mode & 0x03) << 1;      // bits 1-2
    UART_CTRL |= (locale & 0x01) << 3;    // bit 3
    UART_CTRL |= (color & 0x03) << 4;     // bits 4-5

    UART_BAUD = 0;
    UART_BAUD |= (baud & 0x07);           // bits 0-2
}

int uart_compare_registers(void) {
    return (UART_CTRL == UART_CTRL_REF) && (UART_BAUD == UART_BAUD_REF);
}

void uart_reset_registers(void) {
    UART_CTRL = 0;
    UART_BAUD = 0;
    UART_STATUS = 0;
}

