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
- 
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE1NDg3MDM2ODIsMTcxMDcxMTQ0OCwxMT
I4MzY2ODg5LDE2MTAzNzgyOTBdfQ==
-->