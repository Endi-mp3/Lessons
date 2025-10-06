#include <stdio.h>
#include <string.h>
#include "private.h"

int total = 0;
int passed = 0;
int failed = 0;

void run_test(uint8_t en, uint8_t mode, uint8_t loc, uint8_t col, uint8_t baud)
{
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
        printf("✅ [PASS]\n\n");
    } else {
        failed++;
        fprintf(stderr, "❌ [FAIL] CTRL: expected 0x%02X, got 0x%02X | BAUD: expected 0x%02X, got 0x%02X\n\n",
                UART_CTRL_REF, UART_CTRL, UART_BAUD_REF, UART_BAUD);
    }
}

void test_set_registers()
{
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
}

static int compare_arrays(const uint8_t *a, const uint8_t *b, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            printf("Mismatch at index %zu: got %02X, expected %02X\n", i, a[i], b[i]);
            return 0;
        }
    }
    return 1;
}


void test_serialize()
{
    uint8_t buffer[64];
    uint8_t expected[64];
    size_t len;
    int passed = 0;
    int total = 0;

    // ---- Test StructA ----
    {
        total++;
        StructA sa = {0x11, 0x2233, 0x44556677};
        len = stud_serialize(buffer, &sa, STRUCT_A);
        uint8_t expA[] = {0x11, 0x33, 0x22, 0x77, 0x66, 0x55, 0x44};
        if (len == sizeof(expA) && compare_arrays(buffer, expA, sizeof(expA))) {
            passed++;
        } else {
            printf("StructA failed (len=%zu, expected=%zu)\n", len, sizeof(expA));
        }
    }

    // ---- Test StructB ----
    {
        total++;
        StructB sb = {0x11223344, 0x55};
        len = stud_serialize(buffer, &sb, STRUCT_B);
        uint8_t expB[] = {0x44, 0x33, 0x22, 0x11, 0x55};
        if (len == sizeof(expB) && compare_arrays(buffer, expB, sizeof(expB))) {
            passed++;
        } else {
            printf("StructB failed (len=%zu, expected=%zu)\n", len, sizeof(expB));
        }
    }

    // ---- Test StructC ----
    {
        total++;
        StructC sc = {0x1122334455667788ULL, 0x99AA};
        len = stud_serialize(buffer, &sc, STRUCT_C);
        uint8_t expC[] = {0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0xAA,0x99};
        if (len == sizeof(expC) && compare_arrays(buffer, expC, sizeof(expC))) {
            passed++;
        } else {
            printf("StructC failed (len=%zu, expected=%zu)\n", len, sizeof(expC));
        }
    }

    // ---- Test StructD ----
    {
        total++;
        StructD sd = {0x11, 0x22334455, 0x6677};
        len = stud_serialize(buffer, &sd, STRUCT_D);
        uint8_t expD[] = {0x11,0x55,0x44,0x33,0x22,0x77,0x66};
        if (len == sizeof(expD) && compare_arrays(buffer, expD, sizeof(expD))) {
            passed++;
        } else {
            printf("StructD failed (len=%zu, expected=%zu)\n", len, sizeof(expD));
        }
    }

    // ---- Test StructE ----
    {
        total++;
        StructE se = {"Hello", 0x42};
        len = stud_serialize(buffer, &se, STRUCT_E);
        uint8_t expE[] = {'H','e','l','l','o',0x00,0x42};
        if (len == sizeof(expE) && compare_arrays(buffer, expE, sizeof(expE))) {
            passed++;
        } else {
            printf("StructE failed (len=%zu, expected=%zu)\n", len, sizeof(expE));
        }
    }

    // ---- Test StructF ----
    {
        total++;
        StructF sf = {1.0f, 2.0};
        len = stud_serialize(buffer, &sf, STRUCT_F);
        size_t expected_len = sizeof(float) + sizeof(double);
        memcpy(expected, &sf.f, sizeof(float));
        memcpy(expected+sizeof(float), &sf.d, sizeof(double));
        if (len == expected_len && compare_arrays(buffer, expected, expected_len)) {
            passed++;
        } else {
            printf("StructF failed (len=%zu, expected=%zu)\n", len, expected_len);
        }
    }

    // ---- Summary ----
    printf("\nTest summary: %d/%d tests passed.\n", passed, total);
    if (passed == total) {
        printf("✅ All tests passed successfully!\n");
    } else {
        printf("❌ Some tests failed. Check details above.\n");
    }
}

static void print_break(int w)
{
	printf("\n");
	for(int i = 0; i < w; i++)
	{
		printf("-");
	}
	printf("\n");
}

int main(void)
{
	test_set_registers();
	print_break(80);
	test_serialize();
}

