## 11.1 이벤트 플래그
- 이벤트는 개발자가 정한 어떤 값으로 전달된다.
- 여기서는 비트맵으로 처리하도록 한다.
- 비트맵으로 만들면 각각의 이벤트를 명확하게 구분할 수 있고, 이벤트를 구분하는 코드를 간단하게 구현할 수 있기 때문이다.
- 각각의 이벤트 값ㅇ르 겹치지 않는 비트 위치에 할당한다.
- 특정 비트 위치에 독립된 이벤트를 할당해서 이벤트가 있다 없다를 표시하는 방식이다.
- kernel/event.h 와 event.c 파일 두개를 만들도록 한다.
~~~C
#ifndef KERNEL_EVENT_H_
#define KERNEL_EVENT_H_

typedef enum KernelEventFlag_t
{
    KernelEventFlag_UartIn      = 0x00000001,
    KernelEventFlag_CmdIn       = 0x00000002,
    KernelEventFlag_CmdOut      = 0x00000004,
    KernelEventFlag_Reserved03  = 0x00000008,
    KernelEventFlag_Reserved04  = 0x00000010,
    KernelEventFlag_Reserved05  = 0x00000020,
    KernelEventFlag_Reserved06  = 0x00000040,
    KernelEventFlag_Reserved07  = 0x00000080,
    KernelEventFlag_Reserved08  = 0x00000100,
    KernelEventFlag_Reserved09  = 0x00000200,
    KernelEventFlag_Reserved10  = 0x00000400,
    KernelEventFlag_Reserved11  = 0x00000800,
    KernelEventFlag_Reserved12  = 0x00001000,
    KernelEventFlag_Reserved13  = 0x00002000,
    KernelEventFlag_Reserved14  = 0x00004000,
    KernelEventFlag_Reserved15  = 0x00008000,
    KernelEventFlag_Reserved16  = 0x00010000,
    KernelEventFlag_Reserved17  = 0x00020000,
    KernelEventFlag_Reserved18  = 0x00040000,
    KernelEventFlag_Reserved19  = 0x00080000,
    KernelEventFlag_Reserved20  = 0x00100000,
    KernelEventFlag_Reserved21  = 0x00200000,
    KernelEventFlag_Reserved22  = 0x00400000,
    KernelEventFlag_Reserved23  = 0x00800000,
    KernelEventFlag_Reserved24  = 0x01000000,
    KernelEventFlag_Reserved25  = 0x02000000,
    KernelEventFlag_Reserved26  = 0x04000000,
    KernelEventFlag_Reserved27  = 0x08000000,
    KernelEventFlag_Reserved28  = 0x10000000,
    KernelEventFlag_Reserved29  = 0x20000000,
    KernelEventFlag_Reserved30  = 0x40000000,
    KernelEventFlag_Reserved31  = 0x80000000,

    KernelEventFlag_Empty       = 0x00000000,
} KernelEventFlag_t;

void Kernel_event_flag_init(void);
void Kernel_event_flag_set(KernelEventFlag_t event);
void Kernel_event_flag_clear(KernelEventFlag_t event);
bool Kernel_event_flag_check(KernelEventFlag_t event);

#endif /* KERNEL_EVENT_H_ */
~~~
- 위와 같이 이벤트 플래드를 선언해준다.
- 아직 이벤트를 추가하지는 않고, 6번째줄에 UartIn만 선언해 놓았다.
- 32비트 변수로는 32개의 이벤트를 표현할 수 있다.
- 이어서 event.c 파일을 구현하도록 한다.
~~~C
#include "stdint.h"
#include "stdbool.h"

#include "stdio.h"
#include "event.h"

static uint32_t sEventFlag;

void Kernel_event_flag_init(void)
{
    sEventFlag=0;
}

void Kernel_event_flag_set(KernelEventFlag_t event)
{
    sEventFlag |= (uint32_t)event;
}

void Kernel_event_flga_clear(KernelEventFlag_t event)
{
    sEventFlag &= ~((uint32_t)event);
}

bool Kernel_event_flag_check(KernelEventFlag_t event)
{
    if (sEventFlag & (uint32_t)event)
    {
        Kernel_event_flag_clear(event);
        return true;
    }
    return false;
}
~~~
- `sEventFlag`는 이벤트 플래그를 32개 기록하면서, 태스크에 전달하는 역할을 하는 커널 자료 구조이다.
- `Kernel_event_flag_init()`는 `sEventFlag`를 0으로 초기화하는 역할을 한다.
- `Kernel_event_flag_set()` 과 `Kernel_event_flag_clear()`는 비트를 0이나 1로 바꾸는 역할을 한다. 어떤 값을 바꿀지는 파라미터로 전달한다.
- `Kernel_event_flag_check()`는 파라미터로 전달된 특정 이벤트가 `sEventFlag`에 1로 세팅되어 있는지 확인하는 함수이다.
- 커널 API를 통해서 처리하도록 하기 위해 추가로 함수를 작성해 준다.
~~~C
void Kernel_send_events(uint32_t event_list)
{
    for(uint32_t i=0; i<32 ; i++)
    {
        if((event_list >> i) &1)
        {
            KernelEventFlag_t sending_event = KernelEventFlag_Empty;
            sending_event = (KernelEventFlag_t)SET_BIT(sending_event, i);
            Kernel_event_flag_set(sending_event);
        }
    }
}

KernelEventFlag_t Kernel_wait_events(uint32_t waiting_list)
{
    for(uint32_t i=0; i<32; i++)
    {
        if ((waiting_list >>i) & 1)
        {
            KernelEventFlag_t waiting_event = KernelEventFlag_Empty;
            waiting_event = (KernelEventFlag_t)SET_BIT(waiting_event, i);

            if(Kernel_event_flag_check(waiting_event))
            {
                return waiting_event;
            }
        }
    }

    return KernelEventFlag_Empty;
}
~~~

-`Kernel_send_events()`는 이벤트를 전달하는 함수이다.
- `Kernel_wait_events()`는 이벤트를 기다리는 함수이다.
- 주의할 점은 event.h에서 선언한 kernelEventFlag_t가 아니라 그냥 uint32_t이라는 점이다.
- 한번에 이벤트를 여러개를 보내고 받기 위해 구현되어 있다. 예시는 아래와 같다.
- `Kernel_send_events(event1|event2|event3|event4)`

## 인터럽트와 이벤트
- 이벤트는 인터럽트와 엮어서 사용하는것이 일반적이다.
- 하지만 여기서 QEMU는 에뮬레이터이기 때문에 사용할 수 있는 인터럽트가 한정적이다. 
- 따라서 UART 입력이 발생하면 그대로 UART로 전송하는 작업을 할 것이다.
- 이 기능은 태스크의 이벤트 핸들러로 옮겨서 진행하도록 한다.
- Uart.c를 아래와 같이 수정해준다.
~~~C
static void interrupt_handler(){
    uint8_t ch = Hal_uart_get_char();
    Hal_uart_put_char(ch);

    // Chapter 11에서 추가함.
    Kernel_send_events(KernelEventFlag_UartIn);
}
~~~
- 이렇게 한줄 추가해 줌으로써 인터럽트와 이벤트의 연결이 끝났다. 
- 이제 태스크에서 이벤트를 받아서 처리하는 코드를 넣어보고 실험해 보도록 하자.
- Main.c를 수정한 코드를 삽입해 주자.
~~~C
static void Kernel_init(void)
{
    uint32_t taskId;

    Kernel_task_init();
    Kernel_event_flag_init();
    // 중략

}

...


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
            debug_printf("\nCmdOut Event by Task0\n");sssssssss
            break;
        }
        Kernel_yield();
    }
}
~~~
- 크게 두부분을 수정해 준다.
- 첫번째는 `Kernel_init()`에 이벤트 플래그의 초기화 함수를 호출하는 부분을 추가하는 것이다.
- 두번째는 `User_task0()` 함수에 이벤트 처리 함수를 추가하는 것이다.
- while문 안에 있던 스택 주소 출력 코드를 밖으로 빼내고, 이벤트를 기다리는 `KernelEventFlag_UartIn` 플래그를 사용해 이벤트를 기다린다.
- 이 API는 기다리는 이벤트 중 하나가 도착하면 이벤트 값 자체를 리턴한다.
- 위의 코드는 11.3 부분까지 모두 작성된 코드이다.

## 11.3 사용자 정의 이벤트
- 이벤트는 언제나 필요하다고 생각되면 사용하지 않는 이벤트 플래그 하나에 이름을 등록해 태스크에서 태스크로 이벤트를 보낼 수 있다.
- 위에서는 UART 입력에 `KernelEventFlag_UartIn` 이벤트를 연결했다.
- 그리고 Task0에서 이 이벤트를 받았다는것을 알려주는 출력을 하여 이벤트를 처리했다.
- 이제 사용하지 않는 이벤트 플러그 하나인 `Reserved01`을 `CmdIn`으로 변경한다.
- 커맨들가 입력되는것을 가져오는 이벤트이다.
- 아래는 수정된 `boot/Main.c`이다.

~~~C
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
        }
        Kernel_yield();
    }
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
~~~

- `Kernel_send_events()` 커널 API는 Task0의 이벤트 처리 루틴 코드 안에서 호출된다.
- 즉 UART 인터럽트 핸들러에서 `KernelEventFlag_UartIn` 이벤트를 보내고 스케줄러에 의해 Task0이 실행되면 `KernelEventFlag_UartIn` 이벤트를 확인하고 받아온다.
- 그러면 Task0코드에 있는 `case` 부분이 실행이 된다.
- 이 구간에서 다시 `Kernel_send_events()` API 코드가 호출이 되어 `KernelEventFlag_CmdIn` 이벤트를 보낸다. 그리고 `Kernel_yield()`를 호출해서 스케줄링을 한다.
- 그 이후 스케줄링은 Task1이 받게되고, `KerneleventFlag_CmdIn`을 기다리고 있다. 
- 이후 코드에서 이벤트를 처리한다.
- 위의 코드가 정상적으로 작동하게 되면 키보드를 이용해 입력을 주었을 시 `Event handled by Task0`이라는 문구가 출력이 되고 이벤트를 받은 Task1에서 `Event handled by Task1`이라는 문구가 출력이 된다.
- 이렇게 사용자 정의 이벤트가 완료가 되었다.

## 11.4 여러 이벤트 플래그를 동시에 보내고 처리하기.
- 이벤트 플래그를 설계할 때 비트로 처리하는 이유는 여러개의 이벤트를 동시에 처리하기 위해서이다.
- 하나의 이벤트를 더 추가하도록 한다. (위의 코드에는 이미 적용되어 있음)
- Reserved02 이었던 영역에 추가해 CmdOut이라는 이름을 붙인다.
- 그리고 이벤트 핸들러를 수정해 주도록 한다.

~~~C

<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE2MjYwMTQ1NjAsMTYyMDIzNzA2NSwxOT
AyMDM3ODMsODY0OTcwMDg1LC0xODMzMzAxNDgwLC0xNTc5ODgx
Nzk4LC00NzQ5NDY0ODgsMTE2ODg4MzQ5MiwtNTAwMzIxNzgyLC
03NTA0NTQyNjQsLTExODA3NTE2MTEsLTc3MzgzNzE5Niw1OTM3
NDI0MTJdfQ==
-->