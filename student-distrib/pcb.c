#include "pcb.h"

uint32_t cur_id = 0;


// gets address to pcb corresponding to the id
pcb_t * get_pcb(int32_t id){
	uint32_t addr = EIGHTMB - EIGHTKB * (id + 1);
	return (pcb_t * )addr;
}

// gets address to current pcb
pcb_t * get_cur_pcb() {
	uint32_t addr = EIGHTMB - EIGHTKB * (cur_id + 1);
	return (pcb_t *) addr;
}