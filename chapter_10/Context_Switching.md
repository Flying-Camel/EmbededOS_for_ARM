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

~~~
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTMwOTkzMTk3MywxNjEwMzc4MjkwXX0=
-->