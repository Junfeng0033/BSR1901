;/*****************************************************************************
; * @file:    startup_gecko.s
; * @purpose: CMSIS Cortex-M0 Core Device Startup File 
; *           for the TWS Charger Device  
; * @version: V1.0
; * @date:    25 July 2020
;	Author:	  Junfeng
; * Copyright (C) 2020 ARM Limited. All rights reserved.
; * ARM Limited (ARM) is supplying this software for use with Cortex-M0 
; * processor based microcontrollers.  This file can be freely distributed 
; * within development tools that are supporting such ARM based processors. 
; 
; *****************************************************************************/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

;Stack_Size      EQU     0x00000100				 ;define the size as 256 bytes

;                AREA    STACK, NOINIT, READWRITE, ALIGN=3 
;Stack_Mem       SPACE   Stack_Size
__initial_sp	EQU		0x20000ffc


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

; Heap_Size       EQU     0x00000100				;heap size 0
; if not use standard library "malloc" function, it can be set to zero
Heap_Size       EQU     0x00000000				;heap size 0

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     CMP1_IRQn           	  ; IRQ 0
				DCD     CMP2_IRQn           	  ; IRQ 1	
                DCD     UART0_IRQHandler          ; IRQ 2
                DCD     UART1_IRQHandler          ; IRQ 3
                DCD     DMA_IRQHandler            ; IRQ 4	
					
                DCD     TIMER0_IRQHandler         ; IRQ 5
                DCD     TIMER1_IRQHandler         ; IRQ 6
                DCD     GPADC_IRQHandler          ; IRQ 7
					
                DCD     SYS_GPIO_0_IRQHandler     ; IRQ 8;	Group0//GPIO[7:0] combination interrupt
                DCD     SYS_GPIO_1_IRQHandler     ; IRQ 9;	Group1//GPIO[7:0] combination interrupt
				DCD     WDT_IRQHandler            ; IRQ 10
;               DCD     Lightning_IRQHandler   	  ; IRQ 11	Lightning Transfer Interrupt
                DCD     GPIO_A2_IRQHandler   	  ; IRQ 11	changed it to GPIO_A_2(PAD20)
					
                DCD     GPIO_A0_IRQHandler        ; IRQ 12; 
                DCD     APB_SPI0_IRQnHandler      ; IRQ 13; 
                DCD     APB_SPI1_IRQnHandler      ; IRQ 14;
;               DCD     QI_IRQnHandler            ; IRQ 15;
                DCD     GPIO_A3_IRQHandler        ; IRQ 15; changed it to GPIO_A_3(PAD21)
                DCD     QSPI_CTRL_IRQHandler      ; IRQ 16; QSPI controller interrupt 					
                DCD     RTC_IRQHandler            ; IRQ 17; 
                DCD     GPIO_A4_IRQHandler        ; IRQ 18; 
                DCD     GPIO_A5_IRQHandler        ; IRQ 19; 
                DCD     GPIO_A6_IRQHandler        ; IRQ 20; Reserved for GPIO[6] MISC
                DCD     GPIO_A7_IRQHandler        ; IRQ 21; Reserved for GPIO[7] MISC
                DCD     GPIO_B0_IRQHandler        ; IRQ 22; Reserved for GPIO[8] MISC
                DCD     GPIO_B1_IRQHandler        ; IRQ 23; Reserved for GPIO[9] MISC
                DCD     GPIO_B2_IRQHandler        ; IRQ 24; Reserved for GPIO[10] MISC
                DCD     GPIO_B3_IRQHandler        ; IRQ 25; Reserved for GPIO[11] MISC
                DCD     GPIO_B4_IRQHandler        ; IRQ 26; Reserved for GPIO[12] MISC
                DCD     GPIO_B5_IRQHandler        ; IRQ 27; Reserved for GPIO[13] MISC
                DCD     GPIO_B6_IRQHandler        ; IRQ 28; Reserved for GPIO[14] MISC
                DCD     GPIO_B7_IRQHandler        ; IRQ 29; Reserved for GPIO[15] MISC
                DCD     AON_WKUP_IRQHandler       ; IRQ 30; 
                DCD     CMU2AHB_IRQHandler        ; IRQ 31; 


                AREA    |.text|, CODE, READONLY
; Reset Handler
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)                

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  CMP1_IRQn           	  [WEAK]
                EXPORT  CMP2_IRQn                 [WEAK]					
                EXPORT  UART0_IRQHandler          [WEAK]
                EXPORT  UART1_IRQHandler          [WEAK]
                EXPORT  UART2_IRQHandler          [WEAK]					
                EXPORT  TIMER0_IRQHandler         [WEAK]
                EXPORT  TIMER1_IRQHandler         [WEAK]
                EXPORT  GPADC_IRQHandler          [WEAK]
                EXPORT  SYS_GPIO_0_IRQHandler     [WEAK]
                EXPORT  SYS_GPIO_1_IRQHandler     [WEAK]
                EXPORT  WDT_IRQHandler            [WEAK]

                EXPORT  DMA_IRQHandler            [WEAK]
                EXPORT  APB_SPI0_IRQnHandler      [WEAK]	
                EXPORT  APB_SPI1_IRQnHandler      [WEAK]
;               EXPORT  QI_IRQnHandler      	  [WEAK]
                EXPORT  GPIO_A3_IRQHandler        [WEAK]					
                EXPORT  RTC_IRQHandler      	  [WEAK]

;               EXPORT  Lightning_IRQHandler      [WEAK]
                EXPORT  GPIO_A2_IRQHandler        [WEAK]

                EXPORT  QSPI_CTRL_IRQHandler      [WEAK]
                EXPORT  AON_WKUP_IRQHandler       [WEAK]			
                EXPORT  CMU2AHB_IRQHandler        [WEAK]
					
                EXPORT  GPIO_A4_IRQHandler        [WEAK]
                EXPORT  GPIO_A5_IRQHandler        [WEAK]					
                EXPORT  GPIO_A6_IRQHandler        [WEAK]
                EXPORT  GPIO_A7_IRQHandler        [WEAK]
                EXPORT  GPIO_B0_IRQHandler        [WEAK]
                EXPORT  GPIO_B1_IRQHandler        [WEAK]
					
                EXPORT  GPIO_B0_IRQHandler        [WEAK]
                EXPORT  GPIO_B1_IRQHandler        [WEAK]
                EXPORT  GPIO_B2_IRQHandler        [WEAK]
                EXPORT  GPIO_B3_IRQHandler        [WEAK]
                EXPORT  GPIO_B4_IRQHandler        [WEAK]
                EXPORT  GPIO_B5_IRQHandler        [WEAK]
                EXPORT  GPIO_B6_IRQHandler        [WEAK]
                EXPORT  GPIO_B7_IRQHandler        [WEAK]					
CMP1_IRQn
CMP2_IRQn
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
TIMER0_IRQHandler
TIMER1_IRQHandler
GPADC_IRQHandler
SYS_GPIO_0_IRQHandler
SYS_GPIO_1_IRQHandler
WDT_IRQHandler
;Lightning_IRQHandler
GPIO_A2_IRQHandler
DMA_IRQHandler
APB_SPI0_IRQnHandler
APB_SPI1_IRQnHandler
;QI_IRQnHandler
GPIO_A3_IRQHandler
RTC_IRQHandler
QSPI_CTRL_IRQHandler
AON_WKUP_IRQHandler
APB_SPI_IRQnHandler
CMU2AHB_IRQHandler
GPIO_A0_IRQHandler
GPIO_A4_IRQHandler
GPIO_A5_IRQHandler
GPIO_A6_IRQHandler
GPIO_A7_IRQHandler
GPIO_B0_IRQHandler
GPIO_B1_IRQHandler
GPIO_B2_IRQHandler
GPIO_B3_IRQHandler
GPIO_B4_IRQHandler
GPIO_B5_IRQHandler
GPIO_B6_IRQHandler
GPIO_B7_IRQHandler


                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB
                
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit
                
                ELSE							   
                
                IMPORT  __use_two_region_memory	   
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, = Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
