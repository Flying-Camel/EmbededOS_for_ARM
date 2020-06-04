#include "stdint.h"
#include "HalUart.h"
#include "stdio.h"

static void Hw_init(void);
static void Printf_test(void);

int main(void){

    Hw_init();

    uint32_t i = 100;

    while(i--){
        Hal_uart_put_char('N');
    }

    Hal_uart_put_char('\n');

    putstr("Hello World!!\n");

    Printf_test();

    i=100;

    while(i--){        
        uint8_t ch = Hal_uart_get_char();
        Hal_uart_put_char(ch);
    }



    /*
    uint32_t* dummyAddr = (uint32_t*)(1024*1024*100);
    *dummyAddr = sizeof(long);
    reurn 0;
    */
}

static void Hw_init(void){
    Hal_uart_init();
}

static void Printf_test(){

    char* str = "This is a Test!!";
    uint32_t dec = 1234;
    uint32_t hex = 133;

    debug_printf("%s\n",str);
    debug_printf("This is a Decimal : %u\n",dec);
    debug_printf("This is a Hex : %x",hex);
 
}