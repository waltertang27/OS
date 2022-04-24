#ifndef SCHEDULING_H
#define SCHEDULING_H
#include "types.h"

#define FREQ_SET        0x36
#define CHAN_0          0x40
#define MC_REG          0x43
#define PIT_FREQ        11932

#define PIT_IRQ_NUM     0
#define SHIFT           8
#define LOW_MASK        0xFF
#define UP_MASK         0xFF00


extern void pit_init(void);
extern void pit_handler(void);
extern void cont_switch(int prev_id, int term_id);
extern void init_terminals();
extern void next_process();

#endif
