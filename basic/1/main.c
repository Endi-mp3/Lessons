#include <stdint.h>
#include <stdio.h>



int Bit_Status (uint64_t reg, uint8_t BitNum){


if (BitNum < 64)
return ( (reg >> BitNum) & 1);

    
}

Bit0;
Bit1;
Bit2;
Bit3;
Bit4;
Bit5;
Bit6;
Bit7;

int main(void) 
{
 Bit0 = Bit_Status (01001001, 0);
 Bit1 = Bit_Status (01001111, 1);
 Bit2 = Bit_Status (01101001, 2);
 Bit3 = Bit_Status (01001011, 3);
 Bit4 = Bit_Status (01000101, 4);
 Bit5 = Bit_Status (01111001, 5);
 Bit6 = Bit_Status (01011001, 6);
 Bit7 = Bit_Status (01111101, 7);

 printf ("Bit0 = %d\n", Bit0);
 printf ("Bit1 = %d\n", Bit1);
 printf ("Bit2 = %d\n", Bit2);
 printf ("Bit3 = %d\n", Bit3);
 printf ("Bit4 = %d\n", Bit4);
 printf ("Bit5 = %d\n", Bit5);
 printf ("Bit6 = %d\n", Bit6);
 printf ("Bit7 = %d\n", Bit7);
}

