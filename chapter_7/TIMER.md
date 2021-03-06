##
###  7. 타이머
- RealViewPB는 SP804라는 타이머를 사용한다.
- 이 타이머는 측정 카운터가 감소하는 형식이다.
- 이번 장에는 delay()를 구현해 보도록 하자.
##
### 7.1 타이머 하드웨어 초기화
- 하드웨어 레지스터를 구조체로 추상화하여 hal에 추가한다.
- SP805의 스펙은 아래의 링크에 나와있다.
	- [https://developer.arm.com/documentation/ddi0271/d/programmer-s-model/summary-of-registers?lang=en](https://developer.arm.com/documentation/ddi0271/d/programmer-s-model/summary-of-registers?lang=en)
- hal/rvpb/Timer.h에 레지스터 코드를 작성하고 나면 총 7개의 레지스터를 정의하는 것을 알 수 있다.
- `timerxload`는 카운터의 목표 값을 지정하는 레지스터이다.
- `timerxvalue`는 감소하는 레지스터 이다. 따라서 타이머가 켜지면 `timerxvalue`에 `timerxload`의 값을 복사하고 `timerxvalue`가 감소한다.
- 감소하다가 0이되면 인터럽트가 발생한다.
- `timerxcontrol`은 타이머 하드웨어의 속성을 설정하는 레지스터이다.
- `timerxintclr`은 인터럽트 처리가 완료 되었음을 타이머 핟웨어에 알려주는 레지스터이다.
- `TimerxControl_t`는 하드웨어의 속성을 설정한다.
	- `OneShot`이 1이면 타이머 인터럽트가 한 번 발생하고 타이머가 바로 꺼진다.
	- 다시 켜려면 수동으로 레지스터를 설정해야 한다.
- `TimerSize` 는 `timerxload`와 `timerxvalue`의 크기를 설정한다. 0이면 16비트만 사용하고 1이면 32비트를 사용한다.
- `TimerPre`는 클럭마다 카운터를줄일지, 16번마다 줄일지, 256번마다 줄일지를 설정한다.
- `IntEnable`은 타이머 하드웨어의 인터럽트를 켠다.
- `TimerMode`는 `timerxload`를 사용할지 않을지를 결정한다. 0이면 사용하지 않는다.
- `timerxvalue`는 최대값 (0xFFFF or 0xFFFFFFFF) 부터 0까지 카운트가 내려가야 인터럽트가 발생한다.
	- 하드웨어 제조사는 이를 프리러닝 모드라고 부른다.
	- 1일때는 `timerxload`를 사용한다. 해당 값부터 0까지 카운트가 내려가면 인터럽트가 발생한다.
	- 이 모드를 피리오딕(periodic)모드라고 부른다.
- `TIMER_CPU_BASE`는 타이머 하드웨어 레지스터가 할당되어 있는 메모리 주소이다.
	-  RealViewPB에 보면 0x10011000에 할당되어 있는 것을 알 수 있다.
		- 0이면 하나의 타이머만 사용하면 1이면 멀티 타이머를 사용한다.
- `TIMER_INTERRUPT`는 타이머 하드웨어가 인터럽트를 발생시킬 때 GIC에 전달하는 인터럽트 번호이다.
- 여기까지 설명이고 이제 코드를 작성해 보자.
- hal/HalTimer.h 파일을 만들어 공용 인터페이스를 작성한다.
~~~C
#ifndef HAL_HALUART_H_
#define HAL_HALUART_H_

void    Hal_uart_init(void);
void    Hal_uart_put_char(uint8_t ch);

#endif 
~~~
- 이어서 공용 API를 구현하는 하드웨어 의존적인 코드를 hal/rvpb/Timer.c에 작성한다.
~~~C
#include "stdint.h"
#include "Timer.h"
#include "HalTimer.h"
#include "HalInterrupt.h"

extern volatile Timer_t* Timer;

static void interrupt_handler(void);

static uint32_t internal_1ms_counter;

void Hal_timer_init(void){

    Timer->timerxcontrol.bits.TimerEn = 0;
    Timer->timerxcontrol.bits.TimerMode = 0;
    Timer->timerxcontrol.bits.OneShot = 0;
    Timer->timerxcontrol.bits.TimerSize = 0;
    Timer->timerxcontrol.bits.TimerPre = 0;
    Timer->timerxcontrol.bits.IntEnable = 1;
    Timer->timerxload = 0;
    Timer->timerxvalue = 0xFFFFFFFF;


    Timer->timerxcontrol.bits.TimerMode = TIMER_PERIOIC;
    Timer->timerxcontrol.bits.TimerSize = TIMER_32BIT_COUNTER;
    Timer->timerxcontrol.bits.OneShot = 0;
    Timer->timerxcontrol.bits.TimerPre = 0;
    Timer->timerxcontrol.bits.IntEnable = 1;

    uint32_t interval = TIMER_10HZ_INTERVAL / 100;

    Timer->timerxload = interval;
    Timer->timerxcontrol.bits.TimerEn = 1;

    internal_1ms_counter = 0;

    Hal_interrupt_enable(TIMER_INTERRUPT);
    Hal_interrupt_register_handler(interrupt_handler, TIMER_INTERRUPT);
}

uint32_t Hal_timer_get_1ms_counter(void){
    return internal_1ms_counter;
}

static void interrupt_handler(void){
    internal_1ms_counter++;

    Timer->timerxintclr = 1;
}

~~~
- `Hal_timer_init()`은 초기화 하는 함수이다.
	- 초기화 시퀸스는 SP804의 스펙에 나와있다.
- 초기화는 다음과 같은 순서에 의해 진행된다.
1. 타이머를 끈다. `TimerEn=0`
2. 프리-러닝 모드로 설정해 놓는다 `TimerMode=0, OneShot=0`
3. 16비트 카운터 모드로 설정한다 `TimerSize=0`
4. 프리스케일러 분주는 1로 설정한다 `timerPre=0`
5. 인터럽트를 켠다 `IntEnable=1`
6. 로드 레지스터를 켠다.
7. 카운터 레지스터는 0xFFFFFFFF로 설정한다.

- QEMU에서는 1~7의 과정을 거치지 않아도 타이머가 잘 동작한다.
- 하지만 실제 하드웨어를 사용할 때는 앞의 과정이 필요하다.
- 코드에 있는 `interver` 변수가 로드 레지스터로 들어가게 되는데, 해당 값을 이용해 인터럽트의 발생 간격을 지정한다.
- QEMU에서는 이부분을 단순하게 구현해도 동작한다.
- RealViewPB는 타이머 클럭으로 1MHz클럭을 받거나 32.768 오실레이터를 클럭으로 쓸 수 있다.
- QEMU에서는 어떤 값을 사용하는지 알아보기 위해 `SYSCTRL0`의 값을 봐야 한다.
- 앞서 구현한 UART로 확인해보면 모든 값이 0임을 알 수 있다. 따라서 RealViewlPB에서는 1MHz를 사용한다.
- 타이머 로드 레지스터에 값을 설정하는 방법은 스펙에 나와있고, 아래와 같다.
- ![IMG](./img/TimerXLoad.jpg)

## 7.2 타이머 카운터 오버플로
- 카운터 변수의 크기는 32비트이다. 따라서 오버플로 를 염두해 두어야 한다.
- 32비트를 나타내는 최대값은 0xFFFFFFFF 이고, 이를 10진수로 나타내면 4294967295이다.
- 1미리마다 1씩 증가하므로 약 4294967초가 타이머가 나타낼 수 있는 최대수치이다.
- 따라서 약 50일정도 사용 가능한데, `delay()` 타이머를 사용하므로 오버플로에 주의해서 사용해야 한다.

## 7.3 delay() 함수
- 타이머 카운터를 만들어 시간을 측정하는 주된 이유는 다양한 형태의 시간 지연 함수를 만들어 사용하기 위함이다.
- delay()함수를 만들어 보도록 하자.
- stdlib.h  파일을 lib 디렉토리 밑에 만든다.
	- delay() 같은 유틸리티 함수를 앞으로 정의 하도록 한다.

~~~C
#ifdef HAL_HALTIMER_H_
#define HAL_HALTIMER_H_

  

void  Hal_timer_init(void);
uint32_t  Hal_timer_get_1ms_counter(void);

#endif /*HAL_HALTIMER_H_
~~~

- 이제 본체를 `hal/rvpb/Timer.c` 에 작성한다.

~~~C
uint32_t  Hal_timer_get_1ms_counter(){
return  internal_1ms_counter;
}
~~~

- 해당 함수는 카운터 변수인 internal_1ms_counter 를 리턴하고 종료된다.
- 이제 본체인 delay()를 만들어 보도록 하자.
- 함수는 lib/stdlib.c 에 작성 하도록 하자.

~~~ C
#include  "stdint.h"
#include  "stdbool.h"
#include  "HalTimer.h"

void  delay(uint32_t  ms){
uint32_t  goal = Hal_timer_get_1ms_counter() + ms;
while(goal != Hal_timer_get_1ms_counter());
}

~~~

- 카운터가 목표한 값까지 도달하길 기다리는 간단한 코드이다.
- 목표한 값에 도달하는 지 확인하는 방법은 아래의 두가지가 있다.
	- 목표한 값보다 큰 동안 대기
	- 목표한 값과 같은지 비교
- 일반적으론 첫번째 방법이 더 안전하다.
- 하지만 타이머가 오버플로우가 발생할 경우를 고려해 아래의 방법을 사용한다.
- 이제 boot/Main.c 파일에 delay()를 테스트 하는 코드를 삽입해 보겠다.

~~~C
#include "stdint.h"
#include "HalUart.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "HalInterrupt.h"

#include "HalTimer.h"

static void Hw_init(void);
static void Printf_test(void);
static void Timer_test(void);

int main(void){

    Hw_init();

    uint32_t i = 100;

    while(i--){
        Hal_uart_put_char('N');
    }

    Hal_uart_put_char('\n');

    putstr("Hello World!!\n");

    Printf_test();

    Timer_test();

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
~~~

- 이대로 하고 실행하면 오류가 나는데, hal/rvpb/Regs.c에도 등록해 주어야 한다.
~~~C
 
#include  "stdint.h"
#include  "Uart.h"
#include  "Interrupt.h"
#include  "Timer.h" 

volatile  PL011_t* Uart = (PL011_t*)UART_BASE_ADDRESS0;
volatile  GicCput_t* GicCpu = (GicCput_t*)GIC_CPU_BASE;
volatile  GicDist_t* GicDist = (GicDist_t*)GIC_DIST_BASE;
volatile  Timer_t* Timer = (Timer_t*)TIMER_CPU_BASE;
~~~

- 이후 make를 실행하고 qemu를 실행하게 되면, 3초마다 한번씩 찍히는 것을 볼 수 있다.
## 7.4 요약
 - 이번 장에서는 타이머 하드웨어를 제어하는 방법을 공부했다.
 - 이제 다음 장 부터 RTOS 다운 기능을 구현하게 된다.
 - 바로 태스크 구현이다.
 
<!--stackedit_data:
eyJoaXN0b3J5IjpbMjA2NTQ3MTE4NSwxNjU2ODc4NzU4LDIwMj
kyMjUyMTksLTc1NjEyNTM5NCwzNjI3MTYzMTYsLTE0OTUwODI2
NTMsLTk4OTc2MTQzLC00MzE0NjY1MzQsLTE1MjU5MjgzNjIsLT
ExNzk4MjA5ODBdfQ==
-->