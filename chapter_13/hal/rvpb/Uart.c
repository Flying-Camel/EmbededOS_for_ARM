
#include "stdint.h"
#include "stdbool.h"
#include "Uart.h"
#include "HalUart.h"
#include "HalInterrupt.h"
#include "Kernel.h"

extern volatile PL011_t* Uart;

static void interrupt_handler();

void Hal_uart_init(void){
    // Enable Uart

    Uart->uartcr.bits.UARTEN=0;
    Uart->uartcr.bits.TXE=1;
    Uart->uartcr.bits.RXE=1;
    Uart->uartcr.bits.UARTEN=1;

    // Enable input interrupt
    Uart->uartimsc.bits.RXIM = 1;

    // Register Uart interrupt handler.
    Hal_interrupt_enable(UART_INTERRUPT0);
    Hal_interrupt_register_handler(interrupt_handler, UART_INTERRUPT0);
}

void Hal_uart_put_char(uint8_t ch){
    while(Uart->uartfr.bits.TXFF);
    Uart->uartdr.all = (ch & 0xFF);
}

uint8_t Hal_uart_get_char(void){

    uint32_t uartdr;

    while(Uart->uartfr.bits.RXFE);

    uartdr = Uart->uartdr.all;

    // check Error
    if(uartdr & 0xffffff00){
        Uart->uartrsr.all = 0xff;
        return 0;
    }

    return (uint8_t *)(uartdr & 0xff);
    
}

static void interrupt_handler(){
    uint8_t ch = Hal_uart_get_char();
    Hal_uart_put_char(ch);

    Kernel_send_msg(KernelMsgQ_Task0, &ch,1);
    Kernel_send_events(KernelEventFlag_UartIn);

    // Chapter 11에서 추가함.
    //Kernel_send_events(KernelEventFlag_UartIn | KernelEventFlag_CmdIn);
    //if(ch == 'X')
    //{
    //    Kernel_send_events(KernelEventFlag_CmdOut);
    //}
}
