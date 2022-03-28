
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "types.h"

#define RTC_PORT_1 0x70
#define RTC_PORT_2 0x71
#define RTC_REG_B 0x8B 
#define RTC_REG_A 0x8A
#define RTC_REG_C 0x8C
#define SIXTH_BIT_MASK 0x40
#define TOP_FOUR_BITMASK 0xF0 
#define LOWER_FOUR_BITMASK 0xF
#define RTC_IRQ_NUM 8
#define MAX_FREQ 1024
#define MIN_FREQ 2

extern void rtc_init(void);
extern void rtc_handler(void);
int32_t open_rtc (const uint8_t* filename);
int32_t read_rtc (int32_t fd, void* buf, int32_t nbytes);
int32_t write_rtc (int32_t fd, const void* buf, int32_t nbytes);
int32_t close_rtc (int32_t fd);
uint32_t rtc_rate;
volatile uint32_t rtc_int;
extern void rtc_freq (int32_t freq);
// char rate_(uint32_t freq);

int testing_RTC; 

