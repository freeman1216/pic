#ifndef API_H
#define API_H
#include "stdint.h"
/* host api*/
void led_toggle();
/*slot api*/

typedef void (*slot_tick_t)(); /*pass the context */
typedef void (*slot_init_t)(); /*pass the context*/

typedef struct{
    uint32_t api_ver;
    uint8_t *got_offset;
    uint8_t *got_end_offset;
    uint8_t *rel_offset;
    uint8_t *rel_end_offset;
    uint8_t *dynsym_offset;
    uint8_t *dynstr_offset;
    slot_init_t init_func;
    slot_tick_t tick_func;
}slot_header_t;

#endif //API_H
