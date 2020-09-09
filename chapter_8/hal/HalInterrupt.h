#ifndef HAL_HALINTERRUPT_H_
#define HAL_INTERRUPT_H_

#define INTERRUPT_HANDLER_NUM 255

typedef void (*InterHdlr_fptr)(void);

void Hal_interupt_init(void);
void Hal_interupt_enable(uint32_t interrupt_num);
void Hal_interupt_disable(uint32_t interrupt_num);
void Hal_interupt_register_handler(InterHdlr_fptr handler, uint32_t interrupt_num);
void Hal_interupt_run_handler(void);

#endif