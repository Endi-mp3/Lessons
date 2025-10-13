#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "private.h"

size_t stud_serialize(uint8_t *out, void *structure, int type)
{
    // TODO: implement serialization logic
    // Convert fields of the structure into bytes in little-endian order
	int i = 0;
	switch(type)
	{
		case STRUCT_A:
			{
				StructA* s = (StructA*)structure;
				out[i++] = s->a;
				out[i++] = s->b & 0xFF;
				out[i++] = s->b >> 8;
				out[i++] = s->c & 0xFF;
				out[i++] = (s->c >> 8) & 0xFF;
				out[i++] = (s->c >> 16) & 0xFF;
				out[i++] = (s->c >> 24) & 0xFF;
			}
			break;
		case STRUCT_B:
			{
				StructB* s = (StructB*)structure;
				out[i++] = s->x & 0xFF;
				out[i++] = (s->x >> 8) & 0xFF;
				out[i++] = (s->x >> 16) & 0xFF;
				out[i++] = (s->x >> 24) & 0xFF;
				out[i++] = s->y;
			}
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

    return i; // return number of bytes written. Resulting length of out array.
}


