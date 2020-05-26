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
- 윈도우나 리눅스에서 사용되는 디바이스 드라이버는, 많은 하드웨어를 만족시켜야 하기 때문에 굉장히 복잡;하지만, 보통 펌웨어에서는 적당한 수준의 범용성만 만족시키면 된다.
- 이러한 공용 인터페이스를 HAL (Hardware Abstraction Layer) 라고 한다.
- 다른 하드웨어를 추상화 계층이 중계해주는 모양새이다.
- 그럼 이제 공용 HAL API를 만들자.
HalUart.h
~~~C
#ifndef HAL_HALUART_H_
#define HAL_HALUART_H_

void    Hal_uart_init(void);
void    Hal_uart_put_char(uint8_t ch);

#endif /* HAL_HALUART_H_ */
~~~
- HalUart.h은 hal 디렉토리 바로 아래에 위치 시키도록 하자.
- 그래야 만약 보드가 RealViewPB 보드가 아닌 다른 보드의 다른 하드웨어를 제어 하더라도, HalUart.h API를 이용해 제어할 수 있기 때문이다.
- 위의 코드는 Uart를 출력하는 부분이다.
- `Hal_uart_init()` 은 Uart를 초기화 하는 부분이다.
- `Hal_uart_put_char()`는  알파벳 한 글자를 출력하는 함수이다.
- 이렇게 만들어 놓으면, 나중에 컴파일 할 때, 보드의 환경에 따라 지정된 타깃의 H/W에 맞는 코드를 찾아 컴파일 한다.
- 예를들어 만약 라즈베리 파이 보드를 사용한다면, hal/rvpb/Uart.c 대신 hal/rasppi/Uart.c 를 사용하게 되는 것이다.
##
### 5.1.2 Uart 공용 인터페이스 구현
- 헤더 파일을 만들었으니 이제 구현부를 만들어 보자
~~~C
#include "stdint.h"
#include "Uart.h"
#include "HalUart.h"

extern volatile PL011_t* Uart;

void Hal_uart_init(void){
    // Enable Uart

    Uart->uartcr.bits.UARTEN=0;
    Uart->uartcr.bits.TXE=1;
    Uart->uartcr.bits.RXE=1;
    Uart->uartcr.bits.UARTEN=1;

}

void Hal_uart_put_char(uint8_t ch){
    while(Uart->uartfr.bits.TXFF);
    Uart->uartdr.all = (ch & 0xFF);
}
~~~
- UART 초기화를 위해서, 위의 코드보다 더 복잡한 코드가 사용되어야 한다.
- 하지만 QEMU에서 어느 정도 시뮬레이션 되기 때문에 위의 코드만으로도 동작이 가능하다.
- `Uart->uartcr.bits.UARTEN=0;`
	- 하드웨어 제어 전에 미리 꺼놓는다.
- TXE는 출력 RXE는 입력을 의미한다.
- `while(Uart->uartfr.bits.TXFF);
    Uart->uartdr.all = (ch & 0xFF);`
	- uartfr.bits가 0이면, buffer가 비었음을 의미한다.
	- buffer가 0이되면, 알파벳 하나를 출력한다.
- 이제 main에 uart 코드를 작성 해보자.
~~~C
#include "stdint.h"
#include "HalUart.h"

static void Hw_init(void);

int main(void){

    Hw_innit();

    uint32_t i = 100;

    while(i--){
        Hal_Uart_put_char("N");
    }

    /*
    uint32_t* dummyAddr = (uint32_t*)(1024*1024*100);
    *dummyAddr = sizeof(long);
    reurn 0;
    */
}

static void Hw_init(void){
    Hal_uart_init();
}
~~~
- 방금 만든 `Hal_uart_init()`을 통해 Uart를 Enable 시키고, N을 적는 코드이다.
- 위의 코드를 컴파일 하기 위해 Makefile도 수정해 준다.
~~~Makefile
ARCH = armv7-a
MCPU = cortex-a8

TARGET = rvpb

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OC = arm-none-eabi-objcopy

LINKER_SCRIPT = ./navilos.ld
MAP_FILE = build/navilos.map

ASM_SRCS = $(wildcard boot/*.S)
ASM_OBJS = $(patsubst boot/%.S, build/%.os, $(ASM_SRCS))

VPATH = boot \
        hal/$(TARGET)

C_SRCS  = $(notdir $(wildcard boot/*.c))
C_SRCS += $(notdir $(wildcard hal/$(TARGET)/*.c))
C_OBJS = $(patsubst %.c, build/%.o, $(C_SRCS))

INC_DIRS  = -I include 			\
            -I hal	   			\
            -I hal/$(TARGET)
            
CFLAGS = -c -g -std=c11

navilos = build/navilos.axf
navilos_bin = build/navilos.bin

.PHONY: all clean run debug gdb

all: $(navilos)

clean:
	@rm -fr build
	
run: $(navilos)
	qemu-system-arm -M realview-pb-a8 -kernel $(navilos) -nographic
	
debug: $(navilos)
	qemu-system-arm -M realview-pb-a8 -kernel $(navilos) -S -gdb tcp::1234,ipv4
	
gdb:
	arm-none-eabi-gdb
	
$(navilos): $(ASM_OBJS) $(C_OBJS) $(LINKER_SCRIPT)
	$(LD) -n -T $(LINKER_SCRIPT) -o $(navilos) $(ASM_OBJS) $(C_OBJS) -Map=$(MAP_FILE)
	$(OC) -O binary $(navilos) $(navilos_bin)
	
build/%.os: %.S
	mkdir -p $(shell dirname $@)
	$(CC) -march=$(ARCH) -mcpu=$(MCPU) $(INC_DIRS) $(CFLAGS) -o $@ $<
	
build/%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) -march=$(ARCH) -mcpu=$(MCPU) $(INC_DIRS) $(CFLAGS) -o $@ $<
	
~~~

- 위의 내용으로 변경 후 `make run` 명령을 실행하게 되면, "N"이 100개 출력 되는 것을 알 수 있다.
- QUEMU를 UART로 입출력이 되도록 설정 했기 때문에 프로그램을 종료할 수 없다.
- 따라서 kill 명령으로 process를 죽여줘야 한다.

##
### 5.2 안녕 세상!
- 이제 첫 프로그래밍 시 작성하는 Hello World! 를 출력해 보자.
- 현재 printf() 함수가 존재하지 않기 때문에 직접 만들어줘야 한다.
- lib 폴더를 만들고 stdio.c와 stdio.h 파일을 만들어 주자.
- 다음은 stdio.h의 내용이다.
~~~C
#ifndef LIB_STDIO_H_
#define LIB_STDIO_H_

uint32_t putstr(const char* s);

#endif /* LIB_STDIO_H_ */
~~~
- 스트링을 출력하는 putstr() 함수를 만들어 라이브러리에 등록 했다.
- 리턴은 전체 문자의 개수이다.
- 파라미터는 `const char*` 로 설정 했는데, 이것은 읽기 전용으로만 파라미터를 사용할 때 유용하다.
- const로 선언된 파라미터는 실수로 포인터를 변경하는 것을 줄여준다.
- 이제 stdio.c를 만들어 보자.
~~~C

~~~
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTEzMjMzMzU2NTgsLTE3ODg1NDgwNjYsNT
I4ODI3NDgwLC01MjY0Njk0OSwxNzg4MTk5NTg5LDE1ODExODQz
NDQsLTE2NDk3OTE2NzJdfQ==
-->