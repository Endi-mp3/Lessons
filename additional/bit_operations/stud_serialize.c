#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "private.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
		    {
				StructC* s = (StructC*)structure;
				out[i++] = s->id & 0xFF;
				out[i++] = (s->id >> 8) & 0xFF;
				out[i++] = (s->id >> 16) & 0xFF;
				out[i++] = (s->id >> 24) & 0xFF;
				out[i++] = (s->id >> 32) & 0xFF;
				out[i++] = (s->id >> 40) & 0xFF;
				out[i++] = (s->id >> 48) & 0xFF;
				out[i++] = (s->id >> 56) & 0xFF;
				out[i++] = s->code & 0xFF;
				out[i++] = (s->code >> 8) & 0xFF;
		    }
			break;
		case STRUCT_D:
		{
		StructD* s = (StructD*)structure;
				out[i++] = s->a;
				out[i++] = s->b & 0xFF;
				out[i++] = (s->b >> 8) & 0xFF;
				out[i++] = (s->b >> 16) & 0xFF;
				out[i++] = (s->b >> 24) & 0xFF;
				out[i++] = s->c & 0xFF;
				out[i++] = s->c >> 8;
		}		
			break;
		case STRUCT_E:
		{
			StructE* s = (StructE*)structure;
			for (int j= 0; j < 6; j++) 
			{
			out[i++] = s->name[j];
			}

			out[i++] = s->age;

#
		}
			break;
		case STRUCT_F:
		{
			StructF* s = (StructF*)structure;
		
			

		}
			break;
	}

    return i; // return number of bytes written. Resulting length of out array.
}


