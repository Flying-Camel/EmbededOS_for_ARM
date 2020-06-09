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
- GIC에 관한 내용은 Spec의 4.11.2장에 나와있다.
- 앞부분만 조금 살펴 보자면 아래의 이미지와 같다.
- ![GCC 설치확인 이미지](./img/GIC.png)
-  GIC는 크게 두 그룹으로 구분된다. 
- 하나는 CPU Interface registers이고, 다른 하나는 distributor registers다.
- 실제로 GIC는 작성한 레지스터보다 훨씬 더 많은 레지스터를 가지고 있지만, 대부분 사용하지 않는 것이기 때문에 적당한 추상과 과정을 거쳤다.
- 이제 hal/rvpb/Regs.c 파일을 수정해 실제 인스턴스를 선언한다.
~~~C

#include "stdint.h"
#include "Uart.h"
#include "Interrupt.h"

volatile PL011_t*            Uart        = (PL011_t*)UART_BASE_ADDRESS0;
volatile GicCput_t*          GicCpu      = (GicCput_t*)GIC_CPU_BASE;
volatile DistributorCtrl_t*  GicDist_t   = (GicDist_t*)GIC_DIST_BASE;
~~~
- 이제 공용 API를 제작할 차례이다.
- UART와 같이, 제조사와 상관 없이 동작하도록 API를 만들어 주자.
- hal/HalInterrupt.h 파일에 작성한다.
- 초기화, 활성화, 비활성화, 핸들러 등록 등의 API를 작성한다.
~~~C
#ifndef HAL_HALINTERRUPT_H_
#define HAL_INTERRUPT_H_

#define INTERRUPT_HANDLER_NUM 255

typedef void (*InterHdlr_fptr)(void);

void Hal_interupt_init(void);
void Hal_interupt_enable(uint32_t interrupt_num);
void Hal_interupt_disable(uint32_t interrupt_num);
void Hal_interupt_register_handler(InterHdlr_fptr handler, uint32_t interrupt_num);
void Hal_interupt_run_handler(void);

#endif
~~~
- 활성화, 비활성화 함수는 인자로 인터럽트의 번호를 할당 받는다.
- 앞서 제작한 Uart는 44번이다. 따라서 Hal_inte

<!--stackedit_data:
eyJoaXN0b3J5IjpbMTUwMjQ1MDAyLDIwODU3MzcwOTMsMTc5Mz
c5NjUxOSwxMjY4NDEwNjU4LDE3NTIzOTY0ODcsLTE3NDI4NjQx
NCwxNTkyOTcxODczLDEyNjcyMTM3NzddfQ==
-->