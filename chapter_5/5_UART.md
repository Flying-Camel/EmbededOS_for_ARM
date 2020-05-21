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
- 위의 경우로 코딩된 부분을 에러 비트 검사를 한다고 할 경우 아래와 같이 나타낼 수 있다.
~~~C
uint32_r  *uartdr = (uint32_t*)(UART_BASE_ADDR + UARTDR_OFFSET);
*uartdr = (data) << UARTDR_DATA;
bool fe = (bool)((*uartdr >> UARTDR_FE) & 0x1);
bool pe = (bool)((*uartdr >> UARTDR_PE) & 0x1);
bool be = (bool)((*uartdr >> UARTDR_BE) & 0x1);
bool oe = (bool)((*uartdr >> UARTDR_OE) & 0x1);

if (fe || pe || be || oe){
 에러 처리 ...
 }
~~~

- 다음 방법은 구조체를 이용하는 방법이다.
<!--stackedit_data:
eyJoaXN0b3J5IjpbOTk0ODE5OTgzLC0xNjQ5NzkxNjcyXX0=
-->