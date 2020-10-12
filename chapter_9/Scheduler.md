## 9.1 스케줄러
- 먼저 간단한 스케줄러를 만들어 보자.
- 스케줄러는 현재 실행하는 태스크 다음에 실행할 테스크를 선택하는 방법을 결정한다.
- 여기서는 라운드 로빈 방식을 택해 스케줄러를 작성하도록 한다.
- kernel/task.c 에 파일ㅇ르 수정해 라운드 로빈을 작성하도록 한다.

~~~C
static KernelTcb_t* Scheduler_round_robin_algorithm(void)
{
    sCurrent_tcb_index++;
    sCurrent_tcb_index %=sAllocated_tcb_index;

    return &sTask_list[sCurrent_tcb_index];
}
~~~
- sCurrent_tcb_index 라는 변수를 만들어 현재 실행중인 태스크의 블록 인덱스를 저장한다.
- 나머지 연산을 이용해 최대 인덱스를 넘지 않도록 한다.
- 이것이 라운드 로빈 스케줄러의 전부이다.

## 9.2 우선순위 스케줄러
- 
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTI0MDM3MjE4NCwxMTg5MTAwNzE5LDM5MD
Y4NDEzN119
-->