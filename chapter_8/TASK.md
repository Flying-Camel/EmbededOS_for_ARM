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
	- 실무 프로젝
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTEyMDE3MzMyNjgsMTc4NzMwMjUyNV19
-->