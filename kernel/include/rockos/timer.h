#include <stdint.h>

#define SUBTICKS_PER_TICK 100

#define PIT_0 0x40
#define PIT_1 0x41
#define PIT_2 0x42
#define PIT_CONTROL 0x43

#define PIT_MASK 0xFF
#define PIT_SCALE 1193180
#define PIT_SET 0x36

void timer_phase(uint32_t hz);
void timer_handler(void* frame);
uint64_t get_ticks();