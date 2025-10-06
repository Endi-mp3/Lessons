#ifndef PRIVATE_H
#define PRIVATE_H

#include <string.h>
#include <stdint.h>
// Enumeration for structure types
typedef enum
{
    STRUCT_A = 0,
    STRUCT_B,
    STRUCT_C,
    STRUCT_D,
    STRUCT_E,
    STRUCT_F
} StructType;

// Example structures with different sizes and alignments
typedef struct
{
    uint8_t a;
    uint16_t b;
    uint32_t c;
} StructA;

typedef struct
{
    uint32_t x;
    uint8_t y;
} StructB;

typedef struct
{
    uint64_t id;
    uint16_t code;
} StructC;

#pragma pack(push, 1) // packed structure (no alignment)
typedef struct
{
    uint8_t a;
    uint32_t b;
    uint16_t c;
} StructD;
#pragma pack(pop)

typedef struct
{
    char name[6];
    uint8_t age;
} StructE;

typedef struct
{
    float f;
    double d;
} StructF;


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

size_t serialize(uint8_t *out, void *structure, int type);
size_t stud_serialize(uint8_t *out, void *structure, int type);
#endif

