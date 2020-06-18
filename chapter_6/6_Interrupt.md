### 6. Interrupt

- 인터럽트는 임베디드 시스템을 표함한 모든 컴퓨팅 시스템의 꽃이다.
- 컴퓨팅 시스템은 외부와의 상호작용으로 인터럽트를 이용한다.
- 인터럽트는 처리하기 위해 인터럽트 컨트롤러를 사용하는 법을 알고, 인터럽트 컨트롤러를 초기화하고 사용하는 코드를 작성해야 한다.
- 익셉션 핸들러에서 적절한 인터럽트 핸들러를 호출하면 인터럽트 처리가 완료된다.
- 우선 main 함수의 맨 끝에 무한루프를 삽입해 OS를 멈춰놓고, 인터럽트를 테스트해 보자.
##
### 6.1 인터럽트 컨트롤러
- RealViewPB에는 Generic Interrupt Contoller라는 이름의 인터럽트 컨트롤러 하드웨어가 달려있다.
- 먼저 GIC의 레지스터 구조체를 만든다.
-  hal/rvpb/Interrupt.h 파일에 작성한다.
- GIC에 관한 내용은 Spec의 4.11.2장에 나와있다.
- 앞부분만 조금 살펴 보자면 아래의 이미지와 같다.
- ![GIC](./img/GIC.png)
-  GIC는 크게 두 그룹으로 구분된다. 
- 하나는 CPU Interface registers이고, 다른 하나는 distributor registers다.
- 실제로 GIC는 작성한 레지스터보다 훨씬 더 많은 레지스터를 가지고 있지만, 대부분 사용하지 않는 것이기 때문에 적당한 추상과 과정을 거쳤다.
- 이제 hal/rvpb/Regs.c 파일을 수정해 실제 인스턴스를 선언한다.
~~~C

#include "stdint.h"
#include "Uart.h"
#include "Interrupt.h"

volatile PL011_t*            Uart        = (PL011_t*)UART_BASE_ADDRESS0;
volatile GicCput_t*          GicCpu      = (GicCput_t*)GIC_CPU_BASE;
volatile DistributorCtrl_t*  GicDist_t   = (GicDist_t*)GIC_DIST_BASE;
~~~
- 이제 공용 API를 제작할 차례이다.
- UART와 같이, 제조사와 상관 없이 동작하도록 API를 만들어 주자.
- hal/HalInterrupt.h 파일에 작성한다.
- 초기화, 활성화, 비활성화, 핸들러 등록 등의 API를 작성한다.
~~~C
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
~~~
- 활성화, 비활성화 함수는 인자로 인터럽트의 번호를 할당 받는다.
- 앞서 제작한 Uart는 44번이다. 따라서 Hal_interrupt_enable()에 44를 전달하면 UART 인터럽트를 키고, disable 함수에 전달하면 끄게 된다.
- ARM은 모든 인터럽를 IRQ와 FIQ 핸들러로 처리하기 때문에, 개별 이터럽트의 핸들러를 구분해야 한다.
- 그럼 이제 위에 선언한 함수를 구현해 보자.
- 구현 위치는 hal/rvpb/Interrupt.c 이다.

~~~C
#include "stdint.h"
#include "memio.h"
#include "Interrpt.h"
#include "HalInterrupt.h"
#include "armcpu.h"

extern volatile GicCput_t* GicCpu;
extern volatile GicDist_t* GicDist;

static InterHdlr_fptr sHandlers[INTERRUPT_HANDLER_NUM];

void Hal_interrupt_init(void){
    GicCpu->cpucontrol.bits.Enable = 1;
    GicCpu->prioritymask.bits.Prioritymask = GIC_PRIORITY_MASK_NONE;
    GicDist->distributorctrl.bits.Enable = 1;

    for(uint32_t i = 0; i < INTERRUPT_HANDLER_NUM ; i++){
        sHandlers[i] = NULL;
    }

    enable_irq();
}

void Hal_interrupt_enable(uint32_t interrupt_num){
    if ((interrupt_num < GIC_IRQ_START) || (GIC_IRQ_END < interrupt_num)){
        return;
    }

    uint32_t bit_num = interrupt_num - GIC_IRQ_START;

    if (bit_num < GIC_IRQ_START){
        SET_BIT(GicDist->setenable1, bit_num);
    }
    else{
        bit_num -= GIC_IRQ_START;
        SET_BIT(GicDist->setenable2, bit_num);
    }
}

void Hal_interrupt_disable(uint32_t interrupt_num){

    if ((interrupt_num < GIC_IRQ_START) || (GIC_IRQ_END < interrupt_num)){
        return;
    }

    uint32_t bit_num = interrupt_num - GIC_IRQ_START;

    if( bit_num < GIC_IRQ_START){
        CLR_BIT(GicDist->setenable1, bit_num);
    }
    else{
        bit_num -= GIC_IRQ_START;
        CLR_BIT(GicDist->setenable2, bit_num);
    }

}

void Hal_interrupt_register_handler(InterHdlr_fptr handler, uint32_t interrupt_num){
    sHandlers[interrupt_num] = handler;
}

void Hal_interrupt_run_handler(void){
    uint32_t interrupt_num = GicCpu->interruptack.bits.InterruptID;

    if(sHandlers[interrupt_num] != NULL){
        sHandlers[interrupt_num]();
    }
    GicCpu->endofinterrupt.bits.InterruptID = interrupt_num;

    
}
~~~
- `static InterHdlr_fptr sHandlers[INTERRUPT_HANDLER_NUM]` 
	- 인터럽트 핸들러 이고, 255로 선언 되었다. 함수 포인터이다.
- `Hal_interrupt_init()`
	- 이 함수는 스위치를 겨는 동작을 한다.
	- Priority mask 레지스터를 이용해 키고 끈다.
	![Priority mask 레지스터](./img/Priority_mask.PNG)
	- 위의 설명에 나와있듯이, 4번부터 7번비트까지가 유효한 비트이다. 
	- 0x0 으로 설정하게 되면 모든 비트를 마스크 한다.
	- 0xF로 설정하게 되면 인터럽트의 우선순위가 0x0~0xE인 인터럽트를 허용한다.
	- 위의 코드에서는 모든 인터럽트를 허용한다.
	![인터럽트 허용](./img/GIC_INTERRUPT.PNG)
- GIC는 64개의 인터럽트를 관리할 수 있다.
	- `setenable1` 과 `setenable2`로 관리한다.
	- IRQ의 시작 번호는 32이다.
	- GIC는 0번부터 15번까지를 Software Interrupt를 위해 사용하고, 16~31를 GIC software interrupt를 위해 reserve해 놓았다.
	- 나머지가 IRQ Interrupt이다. 각 레지스터의 개별 비트를 IRQ ID32 부터 IRQ ID 95까지 연결했다.
- 시작번호가 32번부터 이기 때문에 오프셋을 구하기 위해선, ID에서 32를 빼고, 그래도 32보다 크다면 다시 32를 빼면 된다.
~~~C
    uint32_t bit_num = interrupt_num - GIC_IRQ_START;

    if (bit_num < GIC_IRQ_START){
        SET_BIT(GicDist->setenable1, bit_num);
    }
    else{
        bit_num -= GIC_IRQ_START;
        SET_BIT(GicDist->setenable2, bit_num);
    }
   ~~~
   - 나머지는 레지스터를 등록하고, 실행하는 코드이다.
##
### 6.2 UART 입력과 인터럽트 연결
 - 작성한 인터럽트와 하드웨어를 연결해보자.
 - UART에 연결해 보도록 하자.
 - 아래와 같이 Uart.c 파일을 수정하고 추가해 준다
 ~~~C
 #include "stdint.h"
#include "Uart.h"
#include "HalUart.h"
#include "HalInterrupt.h"

extern volatile PL011_t* Uart;

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

static void interrupt_handler(){
    uint8_t ch = Hal_uart_get_char();
    Hal_uart_put_char();
}
~~~
- UART 입력이 발생하게 되면, `interrupt_handler()` 함수를 코어가 자동으로 실행한다.
- 위와 같이 코딩해 놓았기 때문에, 현재는 그저 입력받은 함수를 다시 출력하는 역할을 수행한다.
- 이제 Main.c 파일에서 하드웨어 초기화 코드를 수정한다.
~~~C
static void Hw_init(void){
    Hal_interrupt_init();
    Hal_uart_init();
}
~~~
- 보는 바와 같이 interrupt를 먼저 초기화 해 주어야 한다.
- 인터럽트가 먼저 초기화가 되어야 uart에서 사용할 수 있기 때문이다.
##
### 6.3 IRQ 인셉션 벡터 연결.
- 6장에서 다룬 내용은 다음과 같다.
	- main()함수를 무한루프를 주어 종료하지 않도록 설정.
	- 인터럽트 컨트롤러 초기화
	- cspr의 IRQ 마스크를 해제
	- uart 인터럽트 핸들러를 인터럽트 컨트롤러에 등록
	- 인터럽트 컨트롤러와 uart 하드웨어 초기화 순서 조정.
- 이제 마지막으로 IRQ 익셉션 벡터와 인터럽트 컨트롤러의 인터럽트 핸들러를 연결하는 작업이다.
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE5NDA0MzUwODIsMjAwNzY4ODQ2Myw3Nj
c3NTE4NjMsODkwNTAxNTk2LC05OTAxNjcwMjAsLTE3NTU4Mzg4
NjcsMTI0OTY0MTg0OSwyMTE2NzkyNjIxLDIwODcyNjgxODAsLT
U2NTEzNzUzOSw3NDExNTc1MzksMTEzMjEwMzM4OSw0MjM4NTc4
NjUsLTIxNDEwNTY3MzEsMjA4NTczNzA5MywxNzkzNzk2NTE5LD
EyNjg0MTA2NTgsMTc1MjM5NjQ4NywtMTc0Mjg2NDE0LDE1OTI5
NzE4NzNdfQ==
-->