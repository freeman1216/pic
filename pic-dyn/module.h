/* date = May 10th 2026 6:17 am */

#ifndef MODULE_H
#define MODULE_H

#include "api.h"

typedef struct{
    slot_header_t *header;
}module_host_context_t;

typedef enum{
    MODULE_FUNC = 22,
    MODULE_GLOBAL = 21,
}module_symbol_kind_t;

typedef struct{
    char *name;
    void *addr;
    module_symbol_kind_t kind;
}module_symbol_t;

typedef enum{
    MODULE_OK,
    MODULE_SYMBOL_RESOLUTION_ERROR,
} module_status_t;

module_status_t module_init(
                            module_host_context_t *context, 
                            uint8_t *module_mem, 
                            module_symbol_t *symbols,
                            uint32_t symbols_size
                            );
void module_call(uint8_t *got_addr, void *call);

#endif //MODULE_H
