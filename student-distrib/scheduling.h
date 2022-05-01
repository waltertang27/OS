#ifndef SCHEDULING_H
#define SCHEDULING_H
#include "types.h"

#define FREQ_SET        0x36
#define CHAN_0          0x40  // PIT cmd port (channel 0 data port)
#define MC_REG          0x43  // Mode/Command register
#define PIT_FREQ        11932 // (1193180 + 50) / 100 

#define PIT_IRQ_NUM     0
#define SHIFT           8
#define PIT_MASK        0xFF
#define UP_MASK         0xFF00
#define MAX_TERM        3

extern int currScheduled; 
extern int nextScheduled; 
extern int currScheduledPID; 
extern int nextScheduledPID; 
extern int typingFlag; 
extern int sum; 
extern void pit_init(void);
extern void pit_handler(void);
extern void scheduler(int nextScheduled);
extern void cont_switch();



#endif


