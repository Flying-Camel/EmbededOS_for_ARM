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
- 
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTExODA3NTE2MTEsLTc3MzgzNzE5Niw1OT
M3NDI0MTJdfQ==
-->