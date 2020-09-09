## 8.1 태스크 컨트롤 블록
- 태스크 컨트롤 블록이란 개별 태스크 자체를 추상화하는 자료구조를 말한다.
- 태스크는 운영체재에서 동작하는 프로그램 그 자체이다.
- 프로그램은 현재 상태를 기록하고 그 기록에 따라 태스크간 전환이 이루어 지는데 그 상태를 기록한 것을 **컨택스트** 라고 한다.
- 태스크를 관리하는 데 필요한 정보들을 컨트롤 블록에 넣어서 관리한다.
- 이제 이 컨트롤 블록을 구현해 RTOS 커널을 만드는 첫번째 작업에 들어가 보자.
- kernel 이라는 디렉토리를 만들고, task.c 와 task.h 파일을 만든다.
- kernel/task.h 에 태스크 컨트롤 블록을 정의한다.
~~~C
#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_

#include "MemoryMap.h"

#define NOT_ENOUGH_TASK_NUM 0xFFFFFFFF

#define USR_TASK_STACK_SIZE 0x100000
#define MAX_TASK_NUM        (TASK_STACK_SIZE / USR_TASK_STACK_SIZE)

typedef struct KernelTaskContext_t
{
    uint32_t spsr;
    uint32_t r0_r12[13];
    uint32_t pc;
} KernelTaskContext_t

typedef struct KernelTcb_t
{
    uint32_t sp;
    uint8_t* stack_base;
} KernelTcb_t

typedef void (*KernelTaskContext_t)(void);

void Kernel_task_init(void);
uint32_t Kernel_task_create(KernelTaskFunc_t startFunc);

#endif /*KERNEL_TASK_H_*/
~~~

- `MemoryMap.h`를 포함시킨 이유는 밑에서 `TASK_STACK_SIZE` 변수를 사용하기 때문이다.
	- 이 값은 4.3.1에서 모드별 스택을 리셋 핸들러에서 설정할 때 만든 값이다.
-  `USR_TASK_STACK_SIZE`는 개별 태스크의 스택 사이즈이다.
- 0x100000 는 1MB이다. 즉 각 태스크별로 1MB의 스택을 할당한다.
	- 실제 프로젝트에서 사용하기엔 너무 큰 값이다.
	- 실무 프로젝트에선 몇 KB정도의 값이 사용된다.
- 태스크 스택용으로 64MB를 할당해 놓았고, 태스크마다 1MB씩 할당해 놓았으므로, 태스크를 최대 64개까지 사용할 수 있다.
- `Kernel_task_init()` 과 `Kernel_task_create()` 함수는 각각 태스크 관련 기능을 초기화 하고, 등록하는 함수이다.
- `KernelTaskContext_t` 와 `KernelTcb_t` 구조체가 태스크 컨트롤러 블록이다.
- 프로그램 상태 레지스터와 범용 레지스터를 백업할 수 있는 영역을 구조체로 확보해 놓은 것이다.
- 태스크 컨택스트는 결국 레지스터와 스택 포인터의 값이다.
- 즉, 컨택스트를 스위칭 한다는 것은 레지스터 값을 다른 태스크의 것으로 바꾼다는 말과 동일하다.

## 8.2 태스크 컨트롤 블록 초기화

- 이제 실제 메모리에 태스크 컨트롤 블록 인스턴스를 만들고 기본값을 할당하는 코드를 작성해 보자.
- kernel/task.c에 작성한다.
~~~C
#include "stdint.h"
#include "stdbool.h"

#include "ARMv7AR.h"
#include "task.h"

static KernelTcb_t sTask_list(MAX_TASK_NUM);
static uint32_t sAllocated_tcb_index;

void kernel_task_init(void)
{
    sAllocated_tcb_index = 0;

    for(uint32_t i = 0 ; i < MAX_TASK_NUM ; i++)
    {
        sTask_list[i].stack_base=(uint8_t*)(TASK_STACK_START + (i*USR_TASK_STACK_SIZE));
        sTask_list[i].sp = (uint32_t)sTask_list[i].stack_base + USR_TASK_STACK_SIZE -4;

        sTask_list[i].sp -= sizeof(KernelTaskContext_t);
        KernelTaskContext_t* ctx = (KernelTaskContext_t*)sTask_list[i].sp;
        ctx->pc=0;
        ctx->spsr=ARM_MODE_BIT_SYS;
    }
}

uint32_t Kernel_task_create(KernelTaskFunc_t startFunc)
{
    return NOT_ENOUGH_TASK_NUM;
}
~~~
- 7번째 줄에서 태스크 컨트롤 블록을 64개 배열로 선언한다.
- 
<!--stackedit_data:
eyJoaXN0b3J5IjpbMjA2OTMyNzU1MiwtNTcxODA0MjUyLC0xOT
YyMjYyNzc5LDE3ODczMDI1MjVdfQ==
-->