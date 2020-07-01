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
- `timerxload`는 카운터의 목표 값을 지정하는 레지스터이다.
- `timerxvalue`는 감소하는 레지스터 이다. 따라서 타이머가 켜지면 `timerxvalue`에 `timerxload`의 값을 복사하고 `timerxvalue`가 감소한다.
- 감소하다가 0이되면 인터럽트가 발생한다.
- `timerxcontrol`은 타이머 하드웨어의 속성을 설정하는 레지스터이다.
- `timerxintclr`은 인터럽트 처리가 완료 되었음을 타이머 핟웨어에 알려주는 레지스터이다.
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTQzMTQ2NjUzNCwtMTUyNTkyODM2MiwtMT
E3OTgyMDk4MF19
-->