#include <rockos/timer.h>
#include <rockos/hal.h>
#include <rockos/pic.h>
#include <stdio.h>

uint64_t ticks = 0;
uint8_t subticks = 0;

__attribute__((interrupt)) void timer_handler(void* frame) {
    if(++subticks == SUBTICKS_PER_TICK) {
        ++ticks;
        subticks = 0;
    }
    pic_eoi(0);
}

void timer_phase(uint32_t hz) {
    uint32_t divisor = PIT_SCALE / hz;
    outb(PIT_CONTROL, PIT_SET);
    outb(PIT_0, divisor & PIT_MASK);
    outb(PIT_0, (divisor >> 8) & PIT_MASK);
}

uint64_t get_ticks() {
    return ticks*100 + subticks;
}