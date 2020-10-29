#include "stdint.h"
#include "HalUart.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "HalInterrupt.h"
#include "task.h"
#include "Kernel.h"

#include "HalTimer.h"

static void Hw_init(void);
static void Printf_test(void);
static void Timer_test(void);
static void Kernel_init(void);

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

    taskId = Kernel_task_create(User_task0);
    if(NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task0 Creation Fail\n");
    }

    taskId = Kernel_task_create(User_task1);
    if(NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task1 Creation Fail\n");
    }

    taskId = Kernel_task_create(User_task2);
    if(NOT_ENOUGH_TASK_NUM == taskId)
    {
        putstr("Task2 Creation Fail\n");
    }

    Kernel_start();

}

void User_task0(void)
{
    uint32_t local = 0;
    while(true)
    {
        debug_printf("User Task #0, SP=0x%x\n",&local);
        Kernel_yield();
    }
}

void User_task1(void)
{
    uint32_t local = 0;
    while(true)
    {
        debug_printf("User Task #1, SP=0x%x\n",&local);
        Kernel_yield();
    }
}


void User_task2(void)
{
    uint32_t local = 0;
    while(true)
    {
        debug_printf("User Task #2, SP=0x%x\n",&local);
        Kernel_yield();
    }
}
