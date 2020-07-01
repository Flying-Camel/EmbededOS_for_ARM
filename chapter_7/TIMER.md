##
###  7. 타이머
- RealViewPB는 SP804라는 타이머를 사용한다.
- 이 타이머는 측정 카운터가 감소하는 형식이다.
- 이번 장에는 delay()를 구현해 보도록 하자.
##
### 7.1 타이머 하드웨어 초기화
- 하드웨어 레지스터를 구조체로 추상화하여 hal에 추가한다.
- SP805의 스펙은 아래의 링크에 나와있다.
	- 
<!--stackedit_data:
eyJoaXN0b3J5IjpbNDcyNTQ2ODM2LC0xMTc5ODIwOTgwXX0=
-->