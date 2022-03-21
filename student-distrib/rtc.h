
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"

#define RTC_PORT_1 0x70
#define RTC_PORT_2 0x71
#define RTC_REG_B 0x8B 
#define RTC_REG_A 0x8A
#define RTC_REG_C 0x8C
#define SIXTH_BIT_MASK 0x40
#define TOP_FOUR_BITMASK 0xF0 
#define LOWER_FOUR_BITMASK 0xF
#define RTC_IRQ_NUM 8

extern void rtc_init(void);
extern void rtc_handler(void);
int testing_RTC ; 