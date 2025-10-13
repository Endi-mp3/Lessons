#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "private.h"

size_t stud_serialize(uint8_t *out, void *structure, int type)
{
    // TODO: implement serialization logic
    // Convert fields of the structure into bytes in little-endian order
	switch(type)
	{
		case STRUCT_A:
			break;
		case STRUCT_B:
			break;
		case STRUCT_C:
			break;
		case STRUCT_D:
			break;
		case STRUCT_E:
			break;
		case STRUCT_F:
			break;
	}

    return 0; // return number of bytes written. Resulting length of out array.
}


