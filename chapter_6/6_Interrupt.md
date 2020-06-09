### 6. Interrupt

- 인터럽트는 임베디드 시스템을 표함한 모든 컴퓨팅 시스템의 꽃이다.
- 컴퓨팅 시스템은 외부와의 상호작용으로 인터럽트를 이용한다.
- 인터럽트는 처리하기 위해 인터럽트 컨트롤러를 사용하는 법을 알고, 인터럽트 컨트롤러를 초기화하고 사용하는 코드를 작성해야 한다.
- 익셉션 핸들러에서 적절한 인터럽트 핸들러를 호출하면 인터럽트 처리가 완료된다.
- 우선 main 함수의 맨 끝에 무한루프를 삽입해 OS를 멈춰놓고, 인터럽트를 테스트해 보자.
##
### 6.1 인터럽트 컨트롤러
- RealViewPB에는 Generic Interrupt Contoller라는 이름의 인터럽트 컨트롤러 하드웨어가 달려있다.
- 먼저 GIC의 레지스터 구조체를 만든다.
-  hal/rvpb/Interrupt.h 파일에 작성한다.
- 
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTc1MjM5NjQ4NywtMTc0Mjg2NDE0LDE1OT
I5NzE4NzMsMTI2NzIxMzc3N119
-->