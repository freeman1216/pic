#define BAD_USART_IMPLEMENTATION
#define BAD_RCC_IMPLEMENTATION
#define BAD_FLASH_IMPLEMENTATION
#define BAD_GPIO_IMPLEMENTATION
#define BAD_PWR_IMPLEMENTATION
#define BAD_BTIMER_IMPLEMENTATION

#define BAD_HARDFAULT_ISR_IMPLEMENTATION
#define BAD_HARDFAULT_USE_UART

#include "badhal_h562.h"
#include "slot.h"
#include "api.h"

#define UART_GPIO_PORT          (GPIOA)
#define UART1_TX_PIN            (9)
#define UART1_RX_PIN            (10)
#define UART1_TX_AF             (7)
#define UART1_RX_AF             (7)

#define BAD_PIC_FLASH_LATENCY    (FLASH_LATENCY_5ws)

#define BAD_PIC_AHB2_PERIPEHRALS    (RCC_AHB2_GPIOA|RCC_AHB2_GPIOC|RCC_AHB2_SRAM3|RCC_AHB2_SRAM2)
#define BAD_PIC_APB2_PERIPHERALS    (RCC_APB2_USART1)

#define BAD_PIC_SETTINGS (USART_FEATURE_RECIEVE_EN|USART_FEATURE_TRANSMIT_EN)

#define LED_GPIO_PORT (GPIOC)
#define LED_PIN  (13)   

static inline void __main_clock_setup(){
    flash_acceleration_setup(FLASH_REGS,BAD_PIC_FLASH_LATENCY, FLASH_PROGRAMMING_DELAY_2);
    pwr_setup_vos(PWR,PWR_VOS0);
    rcc_default_sysclock_setup(RCC);
}

static inline void __periph_setup(){
    rcc_set_ahb2_clocking(RCC,BAD_PIC_AHB2_PERIPEHRALS);
    io_setup_pin(UART_GPIO_PORT, UART1_TX_PIN, MODER_af, UART1_TX_AF, OSPEEDR_high_speed, PUPDR_no_pull, OTYPR_push_pull);
    io_setup_pin(UART_GPIO_PORT, UART1_RX_PIN, MODER_af, UART1_RX_AF, OSPEEDR_high_speed, PUPDR_no_pull, OTYPR_push_pull);
    io_setup_pin(LED_GPIO_PORT, LED_PIN, MODER_output, 0, OSPEEDR_high_speed, PUPDR_no_pull, OTYPR_push_pull);
    //Enable UART clocking
    rcc_set_apb2_clocking(RCC,BAD_PIC_APB2_PERIPHERALS);
}

static inline void __tick_setup(){
    scb_set_core_interrupt_priority(SCB_SYSTICK_INTR,SCB_PRIO15);
    systick_setup(CLOCK_SPEED/1000, SYSTICK_FEATURE_CLOCK_SOURCE|SYSTICK_FEATURE_TICK_INTERRUPT);
    systick_enable();
}

void led_toggle(){
    io_pin_toggle(LED_GPIO_PORT,LED_PIN);
}


__attribute__((naked))
void  module_call(api_t *api,uint32_t got_addr,void *call){
    (void) api;
    (void) got_addr;
    (void) call;
    __asm__ volatile (
                      "push {r9,lr}    \n"
                      "mov r9,r1       \n"
                      "blx r2          \n"
                      "pop {r9,pc}     \n"
                      );
}

api_t api = {.toggle_led_func = led_toggle};

typedef struct{
    slot_header_t *header;
    slot_init_t callable_init;
    slot_tick_t callable_tick;
    uint32_t got_addr;
}slot_host_context_t;

slot_host_context_t blinky_context;

__attribute__((aligned(4))) uint8_t module[8192];

void systick_isr(){
    module_call(&api,blinky_context.got_addr,blinky_context.callable_tick);
}


int __attribute__((noinline,noreturn)) main(){
    __main_clock_setup();
    __periph_setup();
    
    for(uint32_t i = 0; i < slot_len; i++){
        module[i] = slot[i];
    }
    
    blinky_context.header= (slot_header_t *)module; 
    // patch got
    uint32_t offset = (uint32_t)module;
    blinky_context.got_addr = (uint32_t)blinky_context.header->got_offset + offset;
    
    uint32_t got_word_offset = (uint32_t)blinky_context.header->got_offset / sizeof(uint32_t);
    uint32_t got_word_end_offset = (uint32_t)blinky_context.header->got_end_offset / sizeof(uint32_t);
    
    uint32_t got_absolute = ((uint32_t)blinky_context.header->got_offset + (uint32_t)module);
    
    uint32_t *module_word_ptr = (uint32_t *)module;
    
    for(uint32_t i = 0; i < got_word_end_offset - got_word_offset; i++ ){
        module_word_ptr[i + got_word_offset] = module_word_ptr[i + got_word_offset] + got_absolute;
    }
    blinky_context.callable_init =(slot_init_t) (offset + (uint32_t)blinky_context.header->init_func);
    
    module_call(&api,blinky_context.got_addr,blinky_context.callable_init);
    
    blinky_context.callable_tick =(slot_tick_t) (offset + (uint32_t)blinky_context.header->tick_func); 
    
    
    __tick_setup();
    while(1){
        
    }
}
