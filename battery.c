#include "battery.h"
#include "nrf.h"
#include <stddef.h>

/**
 * @brief Measure VDD using the SAADC peripheral (blocking, single-shot).
 *
 * Uses internal 0.6 V reference with 1/6 gain → full-scale = 3.6 V.
 * SAADC is enabled only for the measurement, then disabled to save power.
 * Safe to call with SoftDevice enabled (SD does not restrict SAADC).
 *
 * IMPORTANT: Call from main-loop context only, NOT from an ISR.
 * The blocking spins (~70–100 µs total) are fine in thread mode
 * but would be inappropriate inside an interrupt handler.
 *
 * @return VDD in millivolts.
 */
static uint16_t measure_vdd_millivolts(void)
{
    volatile int16_t result = 0;

    /* Configure channel 0: VDD input, single-ended */
    NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_VDD;
    NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELN_PSELN_NC;
    NRF_SAADC->CH[0].CONFIG =
        (SAADC_CH_CONFIG_RESP_Bypass     << SAADC_CH_CONFIG_RESP_Pos)   |
        (SAADC_CH_CONFIG_RESN_Bypass     << SAADC_CH_CONFIG_RESN_Pos)   |
        (SAADC_CH_CONFIG_GAIN_Gain1_6    << SAADC_CH_CONFIG_GAIN_Pos)   |
        (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) |
        (SAADC_CH_CONFIG_TACQ_10us       << SAADC_CH_CONFIG_TACQ_Pos)   |
        (SAADC_CH_CONFIG_MODE_SE         << SAADC_CH_CONFIG_MODE_Pos)   |
        (SAADC_CH_CONFIG_BURST_Disabled  << SAADC_CH_CONFIG_BURST_Pos);

    NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_10bit;
    NRF_SAADC->RESULT.PTR    = (uint32_t)&result;
    NRF_SAADC->RESULT.MAXCNT = 1;

    /* Disable all SAADC interrupts (we're polling) */
    NRF_SAADC->INTENCLR = 0xFFFFFFFF;

    /* Enable SAADC */
    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled;

    /* Calibrate offset for better accuracy (~50 µs) */
    NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
    NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
    while (!NRF_SAADC->EVENTS_CALIBRATEDONE) { /* spin */ }
    NRF_SAADC->EVENTS_CALIBRATEDONE = 0;

    /* Start → Sample → Wait for END */
    NRF_SAADC->EVENTS_STARTED = 0;
    NRF_SAADC->TASKS_START = 1;
    while (!NRF_SAADC->EVENTS_STARTED) { /* spin */ }
    NRF_SAADC->EVENTS_STARTED = 0;

    NRF_SAADC->EVENTS_END = 0;
    NRF_SAADC->TASKS_SAMPLE = 1;
    while (!NRF_SAADC->EVENTS_END) { /* spin */ }
    NRF_SAADC->EVENTS_END = 0;

    /* Stop and disable to save power */
    NRF_SAADC->EVENTS_STOPPED = 0;
    NRF_SAADC->TASKS_STOP = 1;
    while (!NRF_SAADC->EVENTS_STOPPED) { /* spin */ }
    NRF_SAADC->EVENTS_STOPPED = 0;

    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Disabled;

    /* Single-ended measurements can return small negative values due to offset */
    if (result < 0) result = 0;

    /* Gain = 1/6, Reference = 0.6 V internal, Resolution = 10-bit (1024)
     * Full-scale input voltage = 0.6 V × 6 = 3.6 V
     * VDD (mV) = result × 3600 / 1024                                    */
    return (uint16_t)((uint32_t)result * 3600UL / 1024UL);
}

void updateBatteryLevel(uint8_t *data)
{
    if (data == NULL) return;

    uint16_t vdd_mv = measure_vdd_millivolts();

    /* Status byte is at offset 6 in the OF payload.
     * Bits 7:6 = battery level, other bits preserved. */
    uint8_t *status = &data[6];
    *status &= (uint8_t)(~STATUS_FLAG_BATTERY_MASK);

    if (vdd_mv > BATTERY_THRESHOLD_FULL_MV) {
        /* already 0x00 — no OR needed */
    } else if (vdd_mv > BATTERY_THRESHOLD_MEDIUM_MV) {
        *status |= STATUS_FLAG_MEDIUM_BATTERY;
    } else if (vdd_mv > BATTERY_THRESHOLD_LOW_MV) {
        *status |= STATUS_FLAG_LOW_BATTERY;
    } else {
        *status |= STATUS_FLAG_CRITICALLY_LOW_BATTERY;
    }
}