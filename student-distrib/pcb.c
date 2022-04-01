#include "pcb.h"

pcb_t * get_pcb(int32_t id){
	uint32_t addr = EIGHTMB - EIGHTKB * (id + 1);
	return (pcb_t * )addr;
}