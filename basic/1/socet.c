#include <stdint.h>


struct hader
{
	uint8_t cmd;
};

struct packet
{
	struct hader header;
	uint8_t id;
	uint8_t length;
	uint16_t comand[];
};



