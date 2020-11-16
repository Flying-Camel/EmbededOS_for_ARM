#include "stdint.h"
#include "stdbool.h"
#include "HalUart.h"
#include "HalInterrupt.h"
#include "HalTimer.h"
#include "stdio.h"
#include "stdlib.h"
#include "Kernel.h"

static void Hw_init(void);
static void Kernel_init(void);
static void Printf_test(void);
static void Timer_test(void);

void User_task0(void);
void User_task1(void);
void User_task2(void);

int main(void){

    Hw_init();

    uint32_t i = 100;

    while(i--){
        Hal_uart_put_char('N');
    }

    Hal_uart_put_char('\n');

    putstr("Hello World!!\n");

    Printf_test();

    //Timer_test();

    // Infinity Loop
    for(;;)

    /*
    i=100;
    while(i--){        
        uint8_t ch = Hal_uart_get_char();
        Hal_uart_put_char(ch);
    }
    */

    /*
    uint32_t* dummyAddr = (uint32_t*)(1024*1024*100);
    *dummyAddr = sizeof(long);
    */

   Kernel_init();

    return 0;
    
}

static void Hw_init(void){
    Hal_interrupt_init();
    Hal_uart_init();
    Hal_timer_init();
}

static void Printf_test(){

    char* str = "This is a Test!!";
    uint32_t dec = 1234;
    uint32_t hex = 133;

    debug_printf("%s\n",str);
    debug_printf("This is a Decimal : %u\n",dec);
    debug_printf("This is a Hex : %x",hex);
 
}

static void Timer_test(void){
    uint32_t i=5;
    while(i--){
        debug_printf("This is Test Satement for delay() ... current Counter is %u\n",Hal_timer_get_1ms_counter());
        delay(3000);
    }
}

static void Kernel_init(void)
{
    uint32_t taskId;

    Kernel_task_init();
    Kernel_event_flag_init();

    taskId = Kernel_task_create(User_task0);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task0 creation fail\n");
    }

    taskId = Kernel_task_create(User_task1);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task1 creation fail\n");
    }

    taskId = Kernel_task_create(User_task2);
    if (NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task2 creation fail\n");
    }

    Kernel_start();

}

void User_task0(void)
{
    uint32_t local = 0;

    debug_printf("User Task #0 SP=0x%x\n", &local);

    while(true)
    {
        KernelEventFlag_t handle_event = Kernel_wait_events(KernelEventFlag_UartIn|KernelEventFlag_CmdOut);
        switch(handle_event)
        {
        case KernelEventFlag_UartIn:
            debug_printf("\nEvent handled by Task0\n");
            break;
        case KernelEventFlag_CmdOut:
            debug_printf("\nCmdOut Event by Task0\n");
            break;
        }
        Kernel_yield();
    }

    /* 모든 이벤트를 처리한 다음 ret */

    /*
    while(true)
    {
        bool pendingEvent = true;
        while(pendingEvent)
        {
            KernelEventFlag_t handle_event = Kernel_wait_events(KernelEventFlag_UartIn|KernelEventFlag_CmdOut);
            switch(handle_event)
            {
            case KernelEventFlag_UartIn:
                debug_printf("\nEvent handled by Task0\n");
                break;
            case KernelEventFlag_CmdOut:
                debug_printf("\nCmdOut Event by Task0\n");
                break;
            default :
                pendingEvent = false;
                break;
            }            
        }
        Kernel_yield();
    }
    */
}

void User_task1(void)
{
    uint32_t local = 0;

    debug_printf("User Task #1 SP=0x%x\n", &local);

    while(true)
    {
        KernelEventFlag_t handle_event = Kernel_wait_events(KernelEventFlag_CmdIn);
        switch(handle_event)
        {
        case KernelEventFlag_CmdIn:
            debug_printf("\nEvent handled by Task1\n");
            break;
        }
        Kernel_yield();
    }
}

void User_task2(void)
{
    uint32_t local = 0;

    debug_printf("User Task #2 SP=0x%x\n", &local);

    while(true)
    {
        Kernel_yield();
    }
}
