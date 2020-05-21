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

~~~C
#ifndef HAL_RVPB_UART_H_
#define HAL_RVPB_UART_H_

typedef union UARTDR_t
{
    uint32_t all;
    struct {
        uint32_t DATA:8;    // 7:0
        uint32_t FE:1;      // 8
        uint32_t PE:1;      // 9
        uint32_t BE:1;      // 10
        uint32_t OE:1;      // 11
        uint32_t reserved:20;
    } bits;
} UARTDR_t;
...
typedef union UARTCR_t
{
    uint32_t all;
    struct {
        uint32_t UARTEN:1;      // 0
        uint32_t SIREN:1;       // 1
        uint32_t SIRLP:1;       // 2
        uint32_t Reserved1:4;   // 6:3
        uint32_t LBE:1;         // 7
        uint32_t TXE:1;         // 8
        uint32_t RXE:1;         // 9
        uint32_t DTR:1;         // 10
        uint32_t RTS:1;         // 11
        uint32_t Out1:1;        // 12
        uint32_t Out2:1;        // 13
        uint32_t RTSEn:1;       // 14
        uint32_t CTSEn:1;       // 15
        uint32_t reserved2:16;
    } bits;
} UARTCR_t;
~~~
- 코드를 보면 공용체를 사용해 구현했다.
- 매크로를 이용한 것보다 어려워 보이지만 기본적인 개념은 동일하다.
- 공용체는 선언된 자료형 중 가장 큰 자료형의 크기를 갖고, 공용체 멤버끼리 주소를 공유한다.
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE3NzYwNTM1NSwtMTY0OTc5MTY3Ml19
-->