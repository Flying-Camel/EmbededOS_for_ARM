### 5. Uart
- 이제 UART 코딩을 시작해 보자.
- RealView Board에 있는 UART는 PL011을 사용한다.
- 그럼 먼저 PL011 DataSheet에 있는 Uart Register를 코드로 옮겨보자.
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
- 위와 같은 레지스터값이 쓰인 공용체를 하나의 구조체로 모아 다음과 같이 하나의 구조체로 코딩할 수 있다.
~~~C
typedef struct PL011_t
{
    UARTDR_t    uartdr;         //0x000
    UARTRSR_t   uartrsr;        //0x004
    uint32_t    reserved0[4];   //0x008-0x014
    UARTFR_t    uartfr;         //0x018
    uint32_t    reserved1;      //0x01C
    UARTILPR_t  uartilpr;       //0x020
    UARTIBRD_t  uartibrd;       //0x024
    UARTFBRD_t  uartfbrd;       //0x028
    UARTLCR_H_t uartlcr_h;      //0x02C
    UARTCR_t    uartcr;         //0x030
    UARTIFLS_t  uartifls;       //0x034
    UARTIMSC_t  uartimsc;       //0x038
    UARTRIS_t   uartris;        //0x03C
    UARTMIS_t   uartmis;        //0x040
    UARTICR_t   uarticr;        //0x044
    UARTDMACR_t uartdmacr;      //0x048
} PL011_t;
~~~
- 다음은 위의 구조체를 이용해 에러 처리를 하는 부분이다.

~~~C
PL011_t* Uart = (PL011_t*)UART_BASE_ADDR;
Uart->uartdr.DATA = data & 0xFF;
if(Uart->uartdr.FE || Uart->uartdr.PE ||
  Uart->uartdt.BE || Uart->uartdr.OE) {
  에러처리 ...
  }
~~~
- 위의 두 가지 방식 중 더 편한 방식대로 구현하면 된다.
- 본 Repo에서는 두번째 방식으로 코딩을 진행한다.
- 이제 구조체 추상화가 끝났으니, UART 하드웨어 베이스 값을 할당해 주면 나머지 레지스터는 Offset에 맞춰 접근이 가능하게 된다.
- UART 하드웨어를 제어할 수 있는 변수를 선언해 Reg.s에 적는다. Reg.c 에는 RealViewPB의 레지스터들을 선언될 페이지이다.
- UART 부분을 코드로 작성하면 아래와 같다.
~~~C
#include "stdint.h"
#include "Uart.h"

volatile PL011_t*   Uart    = (PL011_t*)UART_BASE_ADDRESS0;
~~~
##
### 5.1.1 UART 공용 인터페이스
- PL011은 앞서 말했다시피 RealViewPB의 UART이다.
- 공용 인터페이스를 제작하면, Board가 어떤 하드웨어를 사용하던지, OS에서 공용 인터페이스를 이용해 제어가 가능하다.
- 따라서 아주 간단한 디바이스 드라이버라고 할 수 있다.
- 윈도우나 리눅스에서 사용되는 디바이스 드라이버는, 많은 하드웨어를 만족시켜야 하기 때문에 굉
- 그래서 이제 공용 인터페이스 API를 정의해 보도록 하자.
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTI3NDEzMTY3MiwxNTgxMTg0MzQ0LC0xNj
Q5NzkxNjcyXX0=
-->