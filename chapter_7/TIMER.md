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
- `timerxload`는 카운터의 목표 값을 지정하는 레
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE5MjcxMDc0ODYsLTE1MjU5MjgzNjIsLT
ExNzk4MjA5ODBdfQ==
-->