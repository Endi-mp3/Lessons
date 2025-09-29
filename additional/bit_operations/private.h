#ifndef PRIVATE_H
#define PRIVATE_H

#include <stdint.h>

// UART registers
extern uint8_t UART_CTRL;
extern uint8_t UART_BAUD;

extern uint8_t UART_CTRL_REF;
extern uint8_t UART_BAUD_REF;
extern uint8_t UART_STATUS;

// Reference implementation
void uart_configure(uint8_t enable, uint8_t mode, uint8_t locale, uint8_t color, uint8_t baud);

// Student implementation
void stud_uart_configure(uint8_t enable, uint8_t mode, uint8_t locale, uint8_t color, uint8_t baud);

// Test helpers
int uart_compare_registers(void);
void uart_reset_registers(void);

#endif

