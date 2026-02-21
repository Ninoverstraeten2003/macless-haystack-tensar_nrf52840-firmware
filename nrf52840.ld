SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
    /* S140 v6.1.1 application starts at 0x26000 */
    FLASH_VECTORS (rx) : ORIGIN = 0x26000, LENGTH = 0x100

    /* Gap 0x26100–0x2ACA8: reserved to avoid bootloader region overlap.
     * Adjust ORIGIN if your bootloader layout differs.                 */
    FLASH_TEXT    (rx) : ORIGIN = 0x2ACA8, LENGTH = 0xD9358

    /* RAM: 64KB reserved for SoftDevice (conservative).
     * Actual SD RAM usage is reported by nrf_sdh_ble_enable().
     * You can reclaim unused RAM by lowering this reservation.        */
    RAM (rwx)         : ORIGIN = 0x20010000, LENGTH = 0x30000
}

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH_VECTORS

    .text :
    {
        *(.text*)
        *(.rodata*)

        KEEP(*(.init))
        KEEP(*(.fini))

        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        *(.ARM.extab*)

        /* SDH Observers */
        . = ALIGN(4);
        __start_sdh_ble_observers = .;
        KEEP(*(SORT(.sdh_ble_observers*)))
        __stop_sdh_ble_observers = .;

        . = ALIGN(4);
        __start_sdh_state_observers = .;
        KEEP(*(SORT(.sdh_state_observers*)))
        __stop_sdh_state_observers = .;

        . = ALIGN(4);
        __start_sdh_stack_observers = .;
        KEEP(*(SORT(.sdh_stack_observers*)))
        __stop_sdh_stack_observers = .;

        . = ALIGN(4);
        __start_sdh_req_observers = .;
        KEEP(*(SORT(.sdh_req_observers*)))
        __stop_sdh_req_observers = .;

        . = ALIGN(4);
        __start_log_const_data = .;
        KEEP(*(SORT(.log_const_data*)))
        __stop_log_const_data = .;

        . = ALIGN(4);
        PROVIDE(__etext = .);
    } > FLASH_TEXT

    .data :
    {
        . = ALIGN(4);
        __data_start__ = .;
        *(.data*)

        . = ALIGN(4);
        __start_log_dynamic_data = .;
        KEEP(*(SORT(.log_dynamic_data*)))
        __stop_log_dynamic_data = .;

        . = ALIGN(4);
    } > RAM AT > FLASH_TEXT
    __data_load__ = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    .heap (COPY):
    {
        __HeapBase = .;
        KEEP(*(.heap*))
        __HeapLimit = .;
    } > RAM

    .stack (COPY):
    {
        . = ALIGN(8);
        __StackLimit = .;
        KEEP(*(.stack*))
        . = ALIGN(8);
        __StackTop = .;
    } > RAM
}