#include <stdint.h>
#include "api.h"

uint32_t ticks;

extern uint32_t __got;
extern uint32_t __got_end;

void blick_init(api_t *api){
    (void)api;
} 

void blink_tick(api_t *api){
    if(!(ticks++ & 511)){
        api->toggle_led_func();
    }
}

slot_header_t __attribute__((section(".header"))) blink_slot = {  
    .init_func = blick_init,
    .tick_func = blink_tick,
    .got_offset = &__got,
    .got_end_offset = &__got_end
};


