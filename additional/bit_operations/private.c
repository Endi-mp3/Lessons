#include "private.h"
#include <stdio.h>

// UART registers
uint8_t UART_CTRL = 0;
uint8_t UART_BAUD = 0;
uint8_t UART_STATUS = 0;

// Reference values for comparison
uint8_t UART_CTRL_REF = 0;
uint8_t UART_BAUD_REF = 0;

#define UART_EN_OFFSET		(0x0u)
#define UART_EN_MASK		(0x1u)
#define UART_MODE_OFFSET	(0x1u)
#define UART_LOCALE_OFFSET	(0x3u)
#define UART_COLOR_OFFSET	(0x4u)

#define UART_BAUD_MASK		(0x7)


#define SET_VALUE_WITH_OFFSET(v, mask, off) ( ((v) & (mask)) << (off) )


void uart_configure(uint8_t enable, uint8_t mode, uint8_t locale, uint8_t color, uint8_t baud)
{
    UART_CTRL = 0;
	UART_CTRL |= SET_VALUE_WITH_OFFSET(enable, UART_EN_MASK, UART_EN_OFFSET);
    UART_CTRL |= (enable & 0x01) << UART_EN_OFFSET;         // bit 0
    UART_CTRL |= (mode & 0x03) << UART_MODE_OFFSET;      // bits 1-2
    UART_CTRL |= (locale & 0x01) << UART_LOCALE_OFFSET;    // bit 3
    UART_CTRL |= (color & 0x03) << UART_COLOR_OFFSET;     // bits 4-5

    UART_BAUD = 0;
    UART_BAUD |= (baud & UART_BAUD_MASK);           // bits 0-2
}

int uart_compare_registers(void)
{
    return (UART_CTRL == UART_CTRL_REF) && (UART_BAUD == UART_BAUD_REF);
}

void uart_reset_registers(void)
{
    UART_CTRL = 0;
    UART_BAUD = 0;
    UART_STATUS = 0;
}

size_t serialize(uint8_t *out, void *structure, int type)
{
	return 0;
}

