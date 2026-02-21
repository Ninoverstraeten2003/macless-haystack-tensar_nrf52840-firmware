/*
 * Custom Minimal Startup for nRF52840
 * - Initializes .data and .bss BEFORE calling main()
 * - Fault handler enters low-power sleep (not LED strobe)
 */
    .syntax unified
    .arch armv7e-m

/* ─── Vector Table ──────────────────────────────────────── */
    .section .isr_vector, "a", %progbits
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   0x20040000            /* Initial SP (top of 256KB RAM) */
    .long   Reset_Handler
    .long   Default_Handler       /* NMI */
    .long   Default_Handler       /* HardFault */
    .long   Default_Handler       /* MemManage */
    .long   Default_Handler       /* BusFault */
    .long   Default_Handler       /* UsageFault */
    .long   0, 0, 0, 0           /* Reserved */
    .long   Default_Handler       /* SVCall */
    .long   Default_Handler       /* DebugMon */
    .long   0                     /* Reserved */
    .long   Default_Handler       /* PendSV */
    .long   Default_Handler       /* SysTick */
    .rept   48                    /* External IRQs */
    .long   Default_Handler
    .endr

/* ─── Reset Handler ─────────────────────────────────────── */
    .text
    .thumb
    .thumb_func
    .align 2
    .globl Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:

    /* A. Disable Watchdog (write unlock keys) */
    movw r0, #0x0600
    movt r0, #0x4001        /* NRF_WDT base = 0x40010000, CRV offset = 0x600-ish */
    movw r1, #0x4635
    movt r1, #0x6E52        /* WDT unlock key = 0x6E524635 */
    str  r1, [r0, #0]
    str  r1, [r0, #4]
    str  r1, [r0, #8]
    str  r1, [r0, #12]

    /* B. Set Stack Pointer */
    ldr  r0, =0x20040000
    mov  sp, r0

    /* C. Initialize .data section (copy LMA → VMA) */
    ldr  r0, =__data_load__     /* Source: Flash (LMA) */
    ldr  r1, =__data_start__    /* Destination: RAM (VMA) */
    ldr  r2, =__bss_start__     /* End of .data = start of .bss */
.L_copy_data:
    cmp  r1, r2
    bge  .L_zero_bss
    ldr  r3, [r0], #4
    str  r3, [r1], #4
    b    .L_copy_data

    /* D. Zero .bss section */
.L_zero_bss:
    ldr  r1, =__bss_start__
    ldr  r2, =__bss_end__
    movs r3, #0
.L_zero_loop:
    cmp  r1, r2
    bge  .L_init_done
    str  r3, [r1], #4
    b    .L_zero_loop

.L_init_done:
    /* E. Jump to main */
    bl   main

    /* If main returns, enter low-power sleep */
    b    Default_Handler

/* ─── Default / Fault Handler ───────────────────────────── */
    .globl Default_Handler
    .type Default_Handler, %function
Default_Handler:
    /* Enter ultra-low-power infinite sleep instead of burning
     * current on LED strobing. If you need fault debugging,
     * attach a debugger and check the stacked PC/LR.
     *
     * In DEBUG builds, you can replace this with an LED strobe. */
    cpsid i                      /* Disable interrupts */
.L_sleep_forever:
    wfi                          /* Wait for Interrupt (sleep) */
    b    .L_sleep_forever        /* Loop in case of spurious wake */

    .end