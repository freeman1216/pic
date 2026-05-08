#include <stdint.h>
#include "api.h"

typedef struct{
    uint32_t ticks;
}blink_context_t;

void blick_init(api_t *api,void *context){
    (void)api;
    blink_context_t *module_context = context;
    *module_context = (blink_context_t){0};
} 

void blink_tick(api_t *api, void *context){
    blink_context_t *module_context = context;
    if(!(module_context->ticks++ & 511)){
        api->toggle_led_func();
    }
}

slot_header_t __attribute__((section(".header"))) blink_slot = {.context_size = sizeof(blink_context_t) , .init_func = blick_init,.tick_func = blink_tick};


