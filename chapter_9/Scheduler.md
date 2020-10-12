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
- QEMU의 제약으로 스케줄러를 직접 구현하는 데에는 한계가 있다. 따라서 어떻게 스케줄러를 만드는지 개념만 알고 넘어간다.
- 우선순위 스케줄러란 태스크 우선순위가 있어서 스케줄러가 낮은 우선순위 태스크를 높은 우선순위 태스크가 동작하는 동안 다음에 동작할 태스크로 선택하지 않는 것을 말한다.
- 이를 구현하기 위해선 태스크 컨트롤 블록에 우선순위를 부여해야 한다.
- 우선순위는 태스크를 생성할 때 부여하도록 한다.
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTUzNTU5NjM4MCwxMTg5MTAwNzE5LDM5MD
Y4NDEzN119
-->