## 컨텍스트 스위칭
- 동작하는 프로그램을 바꾼다는 의미이다.
- 나빌로스의 컨텍스트 스위칭은 아래의 순서대로 진행된다.
1. 현재 동작하고 있는 태스크의 컨텍스트를 현재 스택에 저장한다.
2. 다음에 동작할 태스크 컨트롤 블록을 스케줄러에서 받는다.
3. 2에서 받은 태스크 컨트롤 블록에서 스택 포인터를 읽는다.
4. 3에서 읽은 태스크의 스택에서 컨텍스트를 읽어서 ARM코어에 복구한다.
5. 다음에 동작할 태스크의 직전 프로그램 실행 위치로 이동한다. 그러면 이제 현재 동작하고 있는 태스크가 된다.
- 아래는 위의 절차를 코드로 옮긴 것이다.
- kernel/task.c 파일에 수정해 주도록 한다.

~~~C
static KernelTcb_t* Scheduler_round_robin_algorithm(void)
{
    sCurrent_tcb_index++;
    sCurrent_tcb_index %=sAllocated_tcb_index;

    return &sTask_list[sCurrent_tcb_index];
}

void Kernel_task_scheduler(void)
{
    sCurrent_tcb = &sTask_list[sCurrent_tcb_index];
    sNext_tcb = Scheduler_round_robin_algorithm();

    Kernel_task_context_switching();
}
~~~

- `sCurrent_tcb`는 현재 동작중인 태스크 컨트롤 블록의 포인터이다.
- `sNext_tcb`는 라운드 로빈 알고리즘이 선택한 다음에 동작할 태스크 컨트롤 블록의 포인터이다.
- 아래는 컨텍스트 스위칭 함수의 코드이다.
~~~C
__attribute__ ((naked)) void Kernel_task_context_switching(void)
{
    __asm__ ("B Save_context");
    __asm__ ("B Restore_context");
}
~~~

- attribute를 `naked`로 설정하면 컴파일러가 함수를 컴파일 할 때 자동으로 만드는 스택백업, 복구, 리턴 관련 어셈블리를 생성하지 않고 내부에 코딩한대로 그대로 사용한다.
- 나빌로스는 컨텍스트를 스택에 백업하고 스택에서 복구할 것이므로, 스택을 그대로 유지한다.
- `B` instruction을 사용한 이유는 LR를 변경하지 않기 위함이다.
- 따라서 위의 코드는 스택을 쌓지도 않고, LR를 설정해 리턴 주소도 넣지 않는다.
- 위의 컨택스트 스위칭은 엄밀히 따지면 쓰레드간의 전환에 가깝다. 
- 하지만 의미적으로 둘은 차이가 거의 없다.
- 
## 10.1 컨텍스트 백업하기
- 컨텍스트는 현재 동작 중인 태스크의 스택에 직접 백업한다.
- 아래는 컨택스트를 백업하는 코드이다.
~~~C

static __attribute__((naked)) void Save_context(void)
{
    __asm__ ("PUSH {lr}");
    __asm__ ("PUSH {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,}");
    __asm__ ("MRS r0, cpsr");
    __asm__ ("PUSH {r0}");

    __asm__ ("LDR r0, =sCurrent_tcb");
    __asm__ ("LDR r0, [r0]");
    __asm__ ("STMIA r0!, {sp}");
}
~~~

- `PUSH lr` 을 통해 `KernelTaskContext_t`의 pc에 저장한다.
	- 다시 스케줄링되어 복귀할 때 사용된다
- 다음줄은 범용 레지스터를 모두 스택에 푸쉬한다.
- 이어서 `CPSR`을 `spsr`에 저장한다.
- 프로그램 상태 레지스터는 직접 메모리에 저장할 수 없으므로 R0를 사용한다.
- 마지막 부분을 C언어로 구현하면 다음과 같다.
~~~C
sCurrent_tcb->sp = SP Reg 값
or
(uint32_t)(*sCurrent_tdcb) = SP Reg 값.
~~~
## 10.2 컨텍스트 복구하기
- 복구하는 작업은 백업하는 작업의 역순.
- 아래의 함수대로 구현한다.

~~~C
static __attribute__ ((naked)) void Restore_context(void)
{
    __asm__ ("LDR r0, =sNext_tcb");
    __asm__ ("LDR r0, [r0]");
    __asm__ ("LDMIA r0!, {sp}");

    __asm__ ("POP {r0}");
    __asm__ ("MSR cspr, r0");
    __asm__ ("POP {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}");
    __asm__ ("POP {pc}");
}
~~~

- 첫번째로 해줘야 할 작업은 `sNext_tcb` 에서 스택 포인터 값을 읽어오는 작업이다. 
- 그 다음으로 태스크 컨트롤 블록의 sp 멤버 변수의 값을 읽어서 ARM 코어의 SP에 값을 쓰는 작업이다.
- 스택에 저장되어 있는 cpsr의 값을 읽어와 ARM 코어의 cpsr에 값을 적는다.
- 이어서 범용 레지스터를 복구한다.
- 그 후 PC를 복구하면서 태스크로 점프한다.
- 그러면 태스크에서 이어서 작업을 실행하게 된다.

## 10.3 yield 만들기
- 위와 같은 스위칭을 하기 위한 스케줄러가 있어야 한다.
- 여기서는 라운드 로빈이 아닌 비선점형 스케줄링을 사용한다.
- 티스크가 커널에 스케줄링을 요청하는 동작은 태스크가 CPU 자원을 다음 태스크에 양보한다는 의미를 갖는다.
- 이런 동작하는 함수를 yield() 라고 하고 작성해 보도록 하겠다.
- kernel/Kernel.c 와 kernel/Kernel.h 에 파일을 만들고 작성해 보도록 하겠다.
- 아래는 Kernel.h이다.
~~~C
#ifdef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include "task.h"

void Kernel_yield(void);

#endif
~~~
- 아래는 Kernel.c 이다.
~~~C
#include "stdint.h"
#include "stdbool.h"

#include "Kerenl.h"

void Kernel_yield(void)
{
    Kernel_task_scheduler();
}
~~~

- 위의 간단한 코드를 이요해 Kernel_yield() 함수를 호출해 스케줄러를 호출하면 다음에 동작할 태스크를 선정한다.
- 위의 코드를 타게 되면 다음에 동작할 태스크를 선정해서 
<!--stackedit_data:
eyJoaXN0b3J5IjpbMjA2ODExMDk0Miw0NzU5OTI3NTEsNjk0Nj
I1NDc4LDY3NDQ3NTAxOSwxMDgzNTgxNzA5LC0xMDgyNDkwNjkw
LC0xNDgwNzMzMTM3LDE3MTA3MTE0NDgsMTEyODM2Njg4OSwxNj
EwMzc4MjkwXX0=
-->