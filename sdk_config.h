#ifndef SDK_CONFIG_H
#define SDK_CONFIG_H

/*──────────────────────────────────────────────────────────────
 * MaclessHaystack Beacon — Minimal sdk_config.h
 * SDK 15.3.0 / S140 v6.1.1 / nRF52840
 *──────────────────────────────────────────────────────────────*/

// ============================================================
// SoftDevice Handler
// ============================================================
#define NRF_SDH_ENABLED                      1
#define NRF_SDH_DISPATCH_MODEL               0  // Interrupt context

// --- LF Clock (CHOOSE ONE) ---
// OPTION A: 32.768 kHz Crystal Present
#define NRF_SDH_CLOCK_LF_SRC                 1
#define NRF_SDH_CLOCK_LF_RC_CTIV             0
#define NRF_SDH_CLOCK_LF_RC_TEMP_CTIV        0
#define NRF_SDH_CLOCK_LF_ACCURACY            7  // 20 ppm

// OPTION B: No Crystal (RC) — uncomment these, comment Option A
// #define NRF_SDH_CLOCK_LF_SRC                 0
// #define NRF_SDH_CLOCK_LF_RC_CTIV             16
// #define NRF_SDH_CLOCK_LF_RC_TEMP_CTIV        2
// #define NRF_SDH_CLOCK_LF_ACCURACY            1  // 500 ppm  

#define NRF_SDH_SOC_ENABLED                  1
#define NRF_SDH_SOC_OBSERVER_PRIO_LEVELS     2

// ============================================================
// BLE Stack Configuration
// ============================================================
#define NRF_SDH_BLE_ENABLED                  1
#define NRF_SDH_BLE_GAP_DATA_LENGTH          27
#define NRF_SDH_BLE_GAP_EVENT_LENGTH         6
#define NRF_SDH_BLE_SERVICE_CHANGED 0
#define NRF_SDH_BLE_GATT_MAX_MTU_SIZE 23  /* Default ATT MTU, minimum */

#define NRF_SDH_BLE_PERIPHERAL_LINK_COUNT    0  // NO CONNECTIONS
#define NRF_SDH_BLE_CENTRAL_LINK_COUNT       0
#define NRF_SDH_BLE_TOTAL_LINK_COUNT         0
#define NRF_SDH_BLE_VS_UUID_COUNT            0
#define NRF_SDH_BLE_GATTS_ATTR_TAB_SIZE      248  // Minimal GATT table

#define NRF_SDH_BLE_OBSERVER_PRIO_LEVELS     2
#define NRF_SDH_BLE_STACK_OBSERVER_PRIO      0
#define NRF_SDH_STATE_OBSERVER_PRIO_LEVELS   2
#define NRF_SDH_STACK_OBSERVER_PRIO_LEVELS   2
#define NRF_SDH_REQ_OBSERVER_PRIO_LEVELS     2

// ============================================================
// Error Handling
// ============================================================
#define APP_ERROR_ENABLED                    1

// ============================================================
// Board / BSP
// ============================================================
#define BSP_DEFINES_ONLY                     1

// ============================================================
// Logging — ALL DISABLED
// ============================================================
#define NRF_LOG_ENABLED                      0
#define NRF_SDH_BLE_LOG_ENABLED 0
#define NRF_SDH_LOG_ENABLED 0
#define NRF_LOG_BACKEND_RTT_ENABLED          0
#define NRF_LOG_BACKEND_UART_ENABLED         0
#define NRF_LOG_BACKEND_SERIAL_USES_UART     0
#define NRF_LOG_DEFERRED                     0
#define NRF_LOG_STR_PUSH_BUFFER_SIZE         16
#define NRF_FPRINTF_ENABLED                  0

// ============================================================
// Peripherals — ALL DISABLED
// ============================================================
#define NRFX_UARTE_ENABLED                  0
#define NRFX_UART_ENABLED                   0
#define NRFX_TWIM_ENABLED                   0
#define NRFX_TWI_ENABLED                    0
#define NRFX_SPIM_ENABLED                   0
#define NRFX_SPI_ENABLED                    0
#define NRFX_SAADC_ENABLED                  0
#define NRFX_RTC_ENABLED                    0
#define NRFX_TIMER_ENABLED                  0
#define NRFX_PWM_ENABLED                    0
#define NRFX_WDT_ENABLED                    0
#define NRFX_GPIOTE_ENABLED                 0
#define NRFX_PPI_ENABLED                    0
#define NRFX_NFCT_ENABLED                   0
#define NRFX_COMP_ENABLED                   0
#define NRFX_LPCOMP_ENABLED                 0
#define NRFX_QDEC_ENABLED                   0
#define NRFX_PDM_ENABLED                    0
#define NRFX_I2S_ENABLED                    0
#define NRFX_USBD_ENABLED                   0

// Legacy wrappers
#define UART_ENABLED                         0
#define TWI_ENABLED                          0
#define SPI_ENABLED                          0
#define SAADC_ENABLED                        0
#define TIMER_ENABLED                        0
#define RTC_ENABLED                          0
#define WDT_ENABLED                          0

// ============================================================
// Higher-Level Modules — ALL DISABLED
// ============================================================
#define APP_UART_ENABLED                     0
#define APP_FIFO_ENABLED                     0
#define APP_USBD_ENABLED                     0
#define CRC16_ENABLED                        0
#define FDS_ENABLED                          0
#define NRF_FSTORAGE_ENABLED                 0
#define NRF_CRYPTO_ENABLED                   0
#define NRF_QUEUE_ENABLED                    0
#define NRF_PWR_MGMT_ENABLED                 0
#define NRF_BALLOC_ENABLED                   0
#define NRF_MEMOBJ_ENABLED                   0
#define NRF_STRERROR_ENABLED                 0
#define NRF_SECTION_ITER_ENABLED             1  // Required by SDH observers

// ============================================================
// Pin Configuration
// ============================================================
#define CONFIG_NFCT_PINS_AS_GPIOS            1  // Free P0.09/P0.10
#define CONFIG_GPIO_AS_PINRESET              0  // Don't reserve reset pin

// ============================================================
// Power (handled via sd_power_dcdc_mode_set() in code)
// ============================================================
#define NRFX_POWER_ENABLED                   0  // We use SD API directly

//==========================================================
// <e> APP_TIMER_ENABLED - app_timer - Application timer functionality
//==========================================================
#define APP_TIMER_ENABLED 1
// <o> APP_TIMER_CONFIG_RTC_FREQUENCY  - Configure RTC prescaler.
// <0=> 32768 Hz
// <1=> 16384 Hz
// <3=> 8192 Hz
// <7=> 4096 Hz
// <15=> 2048 Hz
// <31=> 1024 Hz
#define APP_TIMER_CONFIG_RTC_FREQUENCY 0

// <o> APP_TIMER_CONFIG_IRQ_PRIORITY  - Interrupt priority
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 
#define APP_TIMER_CONFIG_IRQ_PRIORITY 6

// <o> APP_TIMER_CONFIG_OP_QUEUE_SIZE - Capacity of timer requests queue. 
// <i> Size of the queue depends on how many timers are used and how often timers are started, stopped, or deleted.
#define APP_TIMER_CONFIG_OP_QUEUE_SIZE 10

// <o> APP_TIMER_CONFIG_USE_SCHEDULER - Enable scheduling app_timer events to app_scheduler 
#define APP_TIMER_CONFIG_USE_SCHEDULER 0

// <q> APP_TIMER_KEEPS_RTC_ACTIVE  - Enable RTC always on
// <i> If option is enabled RTC is kept running even if there is no active timers.
#define APP_TIMER_KEEPS_RTC_ACTIVE 0

// <o> APP_TIMER_SAFE_WINDOW_MS - Maximum possible latency (in milliseconds) of the application interrupt handler. 
// <i> This configuration parameter ensures that the timer scheduling mechanism will work correctly even if the application interrupt handler execution is delayed.
#define APP_TIMER_SAFE_WINDOW_MS 300000

// <o> APP_TIMER_WITH_PROFILER - Enable app_timer profiling 
#define APP_TIMER_WITH_PROFILER 0

// <o> APP_TIMER_CONFIG_SWI_NUMBER - Configure SWI instance used. 
// <0=> 0 
// <1=> 1 
#define APP_TIMER_CONFIG_SWI_NUMBER 0

//==========================================================

#endif // SDK_CONFIG_H
