#include "openhaystack.h"
#include <string.h>

#define OF_PAYLOAD_LEN      31
#define OF_KEY_OFFSET        7   /* Byte offset where 22 key bytes start */
#define OF_KEY_LEN          22
#define OF_HINT_OFFSET      29
#define OF_ADDR_LEN          6

/* Static buffers */
static uint8_t generated_addr[OF_ADDR_LEN];
static uint8_t generated_payload[OF_PAYLOAD_LEN];

/*  MaclessHaystack packet template (31 bytes):
 *  [0]     Outer Length  = 0x1E (30 bytes follow)
 *  [1]     AD Type       = 0xFF (Manufacturer Specific)
 *  [2-3]   Company ID    = 0x004C (Apple)
 *  [4]     OF Type       = 0x12 (Offline Finding)
 *  [5]     OF Length      = 0x19 (25: state + 22 key + hint + pad)
 *  [6]     State/Status  = 0x00
 *  [7-28]  Public key bytes 6..27 (22 bytes)
 *  [29]    Hint          = key[0] >> 6
 *  [30]    Padding       = 0x00
 */
static const uint8_t offline_finding_adv_template[OF_PAYLOAD_LEN] = {
    0x1e, 0xff,             /* Outer Len, AD Type */
    0x4c, 0x00,             /* Apple Company ID */
    0x12, 0x19,             /* OF Type, OF Len */
    0x00,                   /* State byte */
    0,0,0,0,0,0,0,0,0,0,0, /* 22-byte key placeholder */
    0,0,0,0,0,0,0,0,0,0,0,
    0x00,                   /* Hint */
    0x00                    /* Padding */
};

static void set_addr_from_key(const uint8_t *key)
{
    /* Nordic BLE address is little-endian: addr[0]=LSB, addr[5]=MSB.
     * Static random address requires two MSBs of MSB byte = 11. */
    generated_addr[5] = key[0] | 0xC0;
    generated_addr[4] = key[1];
    generated_addr[3] = key[2];
    generated_addr[2] = key[3];
    generated_addr[1] = key[4];
    generated_addr[0] = key[5];
}

static void fill_adv_template_from_key(const uint8_t *key)
{
    memcpy(generated_payload, offline_finding_adv_template, OF_PAYLOAD_LEN);
    memcpy(&generated_payload[OF_KEY_OFFSET], &key[6], OF_KEY_LEN);
    generated_payload[OF_HINT_OFFSET] = key[0] >> 6;
}

uint8_t setAdvertisementKey(const uint8_t *key, uint8_t **bleAddr, uint8_t **data)
{
    set_addr_from_key(key);
    fill_adv_template_from_key(key);

    *bleAddr = generated_addr;
    *data    = generated_payload;

    return OF_PAYLOAD_LEN;
}