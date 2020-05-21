### 5.2 Uart
- 이제 uart 코딩을 시작해 보자.
- 먼저 PL011 DataSheet에 있는 Uart Register를 코드로 옮겨보자.
- 레지스터를 코드로 구현하는 부분은 다음과 같이 나타낼 수 있다.
~~~C
#define UART_BASE_ADDR 0x10009000

#define UARTDR_OFFSET (0X00)
#define UARTDR_OFFSET (0)
.
...
~~~
- 위의 경우로 코딩된 부분을 에러 비트 검사를 ㅎ
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTg1MTMwOTY5MiwtMTY0OTc5MTY3Ml19
-->