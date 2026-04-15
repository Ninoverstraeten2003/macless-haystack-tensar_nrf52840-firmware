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
 * Tuned for CR2477 direct to VDD (no LDO).
 *
 * CR2477 discharge curve at low current:
 *   3.0V plateau (years) → 2.9V knee → 2.7V decline → 2.0V dead
 *
 * Earlier thresholds give more warning time before the steep drop. */
#define BATTERY_THRESHOLD_FULL_MV       2900
#define BATTERY_THRESHOLD_MEDIUM_MV     2700
#define BATTERY_THRESHOLD_LOW_MV        2500

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