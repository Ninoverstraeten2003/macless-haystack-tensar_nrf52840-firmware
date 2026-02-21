#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

/* Apple FindMy status byte battery bits (bits 7:6) */
#define STATUS_FLAG_BATTERY_MASK            0xC0   /* 0b11000000 */
#define STATUS_FLAG_MEDIUM_BATTERY          0x40   /* 0b01000000 - 01 */
#define STATUS_FLAG_LOW_BATTERY             0x80   /* 0b10000000 - 10 */
#define STATUS_FLAG_CRITICALLY_LOW_BATTERY  0xC0   /* 0b11000000 - 11 */
#define STATUS_FLAG_FULL_BATTERY            0x00   /* 0b00000000 - 00 */

/* Battery voltage thresholds (millivolts).
 * Tune for your power path:
 *   CR2477 direct:      3000mV fresh → 2000mV dead
 *   LiPo + LDO (3.3V): VDD = 3300mV until LiPo ≈ 3.5V, then drops */
#define BATTERY_THRESHOLD_FULL_MV       2800
#define BATTERY_THRESHOLD_MEDIUM_MV     2500
#define BATTERY_THRESHOLD_LOW_MV        2200

/**
 * @brief Measure VDD via SAADC and encode battery level
 *        into the OpenHaystack status byte (data[6]).
 *
 * Must be called from main-loop (thread) context, not from an ISR.
 *
 * @param data  Pointer to the 31-byte advertisement payload.
 */
void updateBatteryLevel(uint8_t *data);

#endif /* BATTERY_H */