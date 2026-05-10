#include "module.h"
#include "api.h"

typedef struct{
    uint32_t r_offset;  // Where to patch (Address of the GOT entry)
    uint32_t r_info;    // Symbol index and relocation type
}rel_entry_t;

typedef enum{
    REL_ARM_ABS32= 2,
    REL_ARM_GLOB_DATA= 21,
    REL_ARM_JUMP_SLOT = 22,
    REL_ARM_RELATIVE = 23,
}rel_type_t;

typedef struct{
    uint32_t name_idx;
    uint32_t value;
    uint32_t size;
    uint8_t info;
    uint8_t other;
    uint16_t section_idx;
}dynsym_entry_t;

#define ELF32_R_SYM(i)  ((i) >> 8)    // The index in the Symbol Table
#define ELF32_R_TYPE(i) ((uint8_t)(i)) 

__attribute__((naked))
void module_call(uint8_t *got_addr,void *call){
    (void) got_addr;
    (void) call;
    __asm__ volatile (
                      "push {r9,lr}    \n"
                      "mov r9,r0       \n"
                      "blx r1          \n"
                      "pop {r9,pc}     \n"
                      );
}

static uint32_t __strcmp(char *in0,char *in1){
    while(*in0 || *in1 ){
        if(*in0++ != *in1++){
            return 1;
        }
    }
    return 0;
}

static uint32_t module_symbol_resolve(
                                      char *input_symbol,
                                      module_symbol_kind_t kind,
                                      module_symbol_t *symbols, 
                                      uint32_t symbols_len
                                      )
{
    for(uint32_t i = 0; i < symbols_len; i++){
        if(symbols[i].kind != kind){
            continue;
        }
        
        if(!__strcmp(symbols[i].name, input_symbol)){
            return (uint32_t)symbols[i].addr;
        }
        
    }
    
    return 0;
}

module_status_t module_init(
                            module_host_context_t *context,
                            uint8_t *module_mem,
                            module_symbol_t *symbols,
                            uint32_t symbols_len
                            )
{
    slot_header_t *header =(slot_header_t *)module_mem; 
    context->header = header;
    // figure out addresses
    uint32_t offset = (uint32_t)module_mem;
    
    rel_entry_t *begin = (rel_entry_t *)(header->rel_offset + offset );
    rel_entry_t *end = (rel_entry_t *)(header->rel_end_offset + offset );
    dynsym_entry_t *table = (dynsym_entry_t *)(header->dynsym_offset + offset);
    char *dynstr = header->dynstr_offset + offset;
    // handle reloc
    for(rel_entry_t *it = begin; it < end; it++){
        rel_type_t type = ELF32_R_TYPE(it->r_info);
        switch(type){
            case REL_ARM_GLOB_DATA:
            case REL_ARM_JUMP_SLOT:{
                uint32_t dynsym_idx =  ELF32_R_SYM(it->r_info);
                dynsym_entry_t *assosiated_entry = table + dynsym_idx;
                char *sym_name = dynstr + assosiated_entry->name_idx;
                uint32_t sym_addr = module_symbol_resolve(sym_name,(module_symbol_kind_t)type,symbols,symbols_len);
                
                if(sym_addr){
                    *(uint32_t *)(module_mem + it->r_offset) = sym_addr;
                }else{
                    return MODULE_SYMBOL_RESOLUTION_ERROR;
                }
                
                break;
            }
            case REL_ARM_RELATIVE:{
                *(uint32_t *)(module_mem + it->r_offset) = (uint32_t)module_mem + *(uint32_t *)(module_mem + it->r_offset);
                break;
            }
            default:{
                __builtin_trap();
            }
        }
    }
    return MODULE_OK;
}

