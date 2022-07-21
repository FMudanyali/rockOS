#include <rockos/keyboard.h>
#include <rockos/pic.h>
#include <rockos/timer.h>
#include <rockos/hal.h>
#include <string.h>	

#define ESC_P 0x01
#define BACKSPACE_P 0x0F
#define ENTER_P 0x1C
#define LEFT_C_P 0x1D
#define LEFT_SHIFT_P 0x2B
#define RIGHT_SHIFT_P 0x36
#define LEFT_ALT_P 0x38
#define CAPS_LCK_P 0x3A

static unsigned char interrupt_key;
static uint8_t keyset[89];
static uint64_t keytimes[89];

static unsigned char keymap[89] = {
    0, ESC_P, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE_P,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', ENTER_P, LEFT_C_P,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', LEFT_SHIFT_P, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', RIGHT_SHIFT_P, '*', LEFT_ALT_P, ' ', CAPS_LCK_P
};

void initialize_keyset() {
    memset(keyset, 0, 89);
    memset(keytimes, 0, 89);
}

__attribute__((interrupt)) void keyboard_handler(void* frame) {
    interrupt_key = inb(0x60);
    pic_eoi(1);
    if((interrupt_key & 128) == 128){
        keyset[interrupt_key - 0x80] = 0;
        keytimes[interrupt_key - 0x80] = 0;
    } else {
        if(keyset[interrupt_key] == 2)
            return;
        keyset[interrupt_key] = 1;
    }
}

unsigned char readkey() {
    uint8_t shift_pressed = 0;
    if (keyset[LEFT_SHIFT_P] || keyset[RIGHT_SHIFT_P])
        shift_pressed = 1;

    for(uint8_t i = 0; i < 89; ++i) {
        switch(i) {
            case ESC_P:
            case LEFT_C_P:
            case LEFT_SHIFT_P:
            case RIGHT_SHIFT_P:
            case LEFT_ALT_P:
            case CAPS_LCK_P:
                continue;
        }
        if (keyset[i]){
            uint64_t temptime = get_ticks();
            if(keytimes[i]) {
                if(keyset[i] == 1) {
                    if(temptime - keytimes[i] < 20) {
                        return 0;
                    } else {
                        keyset[i] = 2;
                    }
                } else {
                    if(temptime - keytimes[i] < 5) {
                        return 0;
                    }
                }
            }
            keytimes[i] = temptime;
            if (shift_pressed && (keymap[i] > 0x60 && keymap[i] < 0x7A)) {
                return keymap[i] - 0x20;
            } else {
                return keymap[i];
            }
        }
    }
    return 0;
}