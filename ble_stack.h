#ifndef BLE_STACK_H
#define BLE_STACK_H

#include <stdint.h>

/**
 * @brief Initialize BLE SoftDevice, configure RAM, enable DC/DC converters.
 */
void init_ble(void);

/**
 * @brief Set the BLE MAC address (static random).
 * @param addr  6-byte address in Nordic little-endian order.
 */
void setMacAddress(uint8_t *addr);

/**
 * @brief Copy raw advertising payload into the internal buffer.
 * @param data  Pointer to advertisement bytes.
 * @param dlen  Length (max 31).
 */
void setAdvertisementData(uint8_t *data, uint8_t dlen);

/**
 * @brief Configure and start non-connectable BLE advertising.
 * @param interval_ms  Advertising interval in milliseconds.
 * @param tx_power     TX power in dBm (e.g. -4, 0, +4).
 */
void startAdvertisement(int interval_ms, int tx_power);

/**
 * @brief Stop advertising (if running).
 */
void stopAdvertisement(void);

#endif /* BLE_STACK_H */