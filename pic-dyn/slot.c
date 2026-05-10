#include <stdint.h>
#include "api.h"

//Global
uint32_t ticks;

//linker script symbols
extern uint8_t __got;
extern uint8_t __got_end;
extern uint8_t __rel;
extern uint8_t __erel;
extern uint8_t __dynsym;
extern uint8_t __dynstr
;
//extern global
extern uint32_t toggles;

void blink_init(){
} 

void blink_tick(){
    if(!(ticks++ & 511)){
        led_toggle();
        toggles++;
    }
}

slot_header_t __attribute__((section(".header"))) blink_slot = { 
    .api_ver = UINT32_MAX,
    .init_func = blink_init,
    .tick_func = blink_tick,
    .got_offset = &__got,
    .got_end_offset = &__got_end,
    .rel_offset = &__rel,
    .rel_end_offset = &__erel,
    .dynsym_offset = &__dynsym,
    .dynstr_offset = &__dynstr,
};


