#include <stdint.h>

#define CFG_MAX_ENT_CNT 16
#define CFG_ENT_SIZE	16

struct fifo_Entity
{
	uint8_t dummy[CFG_ENT_SIZE];

};

struct fifo
{
	struct fifo_Entity buff [CFG_MAX_ENT_CNT];
	int idxReader;
	int idxWritter;
};

struct fifo ls_Fifo = { {0}, 0, 0 };



int fifo_push(struct fifo *f, struct fifo_Entity entity){
	int next = (f->idxWritter + 1)% CFG_MAX_ENT_CNT;

	if (next == f->idxReader)
	return 1;

	f->buff[f->idxWritter] = entity;
	f->idxWritter = next;
	return 0;
}



int fifo_pop(struct fifo *f, struct fifo_Entity *out){
	if(f->idxReader == f->idxWritter)
	return 1;
	
	*out = f->buff[f->idxReader];
	f->idxReader = (f->idxReader + 1) % CFG_MAX_ENT_CNT;
	return 0;
}



