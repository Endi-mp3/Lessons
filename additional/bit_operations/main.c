#include <stdio.h>
#include "private.h"

int total = 0;
int passed = 0;
int failed = 0;

void run_test(uint8_t en, uint8_t mode, uint8_t loc, uint8_t col, uint8_t baud) {
    total++;
    printf("Test #%03d → EN=%u MODE=%u LOC=%u COL=%u BAUD=%u\n",
           total, en, mode, loc, col, baud);

    // Step 1: run reference
    uart_configure(en, mode, loc, col, baud);
    UART_CTRL_REF = UART_CTRL;
    UART_BAUD_REF = UART_BAUD;

    // Step 2: reset and run student version
    uart_reset_registers();
    stud_uart_configure(en, mode, loc, col, baud);

    // Step 3: compare
    if (uart_compare_registers()) {
        passed++;
        printf("[PASS]\n\n");
    } else {
        failed++;
        fprintf(stderr, "[FAIL] CTRL: expected 0x%02X, got 0x%02X | BAUD: expected 0x%02X, got 0x%02X\n\n",
                UART_CTRL_REF, UART_CTRL, UART_BAUD_REF, UART_BAUD);
    }
}

int main(void) {
    for (uint8_t en = 0; en <= 1; en++) {
        for (uint8_t mode = 0; mode <= 3; mode++) {
            for (uint8_t loc = 0; loc <= 1; loc++) {
                for (uint8_t col = 0; col <= 3; col++) {
                    for (uint8_t baud = 0; baud <= 7; baud++) {
                        run_test(en, mode, loc, col, baud);
                    }
                }
            }
        }
    }

    printf("\n=== UART CONFIG SUMMARY ===\n");
    printf("Total tests:   %d\n", total);
    printf("Passed tests:  %d\n", passed);
    printf("Failed tests:  %d\n", failed);
    printf("===========================\n\n");

    return 0;
}

