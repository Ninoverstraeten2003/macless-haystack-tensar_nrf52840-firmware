#ifndef OPENHAYSTACK_H
#define OPENHAYSTACK_H

#include <stdint.h>

/**
 * @brief Derive BLE address + advertisement payload from a 28-byte public key.
 * @param key      28-byte OpenHaystack public key.
 * @param bleAddr  [out] Pointer to 6-byte BLE static random address.
 * @param data     [out] Pointer to 31-byte advertisement payload.
 * @return Payload length (always 31).
 */
uint8_t setAdvertisementKey(const uint8_t *key, uint8_t **bleAddr, uint8_t **data);

#endif /* OPENHAYSTACK_H */