#ifndef API_H
#define API_H
/* host api*/
typedef void (* toggle_led_ptr_t) (void);
typedef void (* log_print_ptr_t) (char *);

typedef struct{
    toggle_led_ptr_t toggle_led_func;
    log_print_ptr_t log_print_func;
}api_t;

/*slot api*/

typedef void (*slot_tick_t)(api_t *); /*pass the context */
typedef void (*slot_init_t)(api_t *); /*pass the context*/

typedef struct{
    uint32_t api_ver;
    uint32_t *got_offset;
    uint32_t *got_end_offset;
    slot_init_t init_func;
    slot_tick_t tick_func;
}slot_header_t;

#endif //API_H
