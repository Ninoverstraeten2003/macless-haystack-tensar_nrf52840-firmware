#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_stack.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "app_error.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"
#include "boards.h"

#define APP_BLE_CONN_CFG_TAG    1

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

/* Raw advertisement payload buffer */
static uint8_t m_enc_advdata[31] = {0};

static ble_gap_adv_data_t m_adv_data = {
    .adv_data = {
        .p_data = m_enc_advdata,
        .len    = 0
    },
    .scan_rsp_data = {
        .p_data = NULL,
        .len    = 0
    }
};

void init_ble(void)
{
    ret_code_t err_code;

    /* 1. Enable SoftDevice */
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    /* 2. Configure BLE stack RAM */
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    /* 3. CRITICAL: Enable DC/DC converters for major power savings.
     *
     *    REG1 (1.1V core): Saves ~20–30% vs internal LDO.
     *    REG0 (VDDH→VDD): Saves additional power when VDDH > VDD.
     *
     *    NOTE: Both require external inductors on the board.
     *    The SuperMini (nice!nano v2 clone) has both inductors. */
    err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);    /* REG1 */
    APP_ERROR_CHECK(err_code);

    err_code = sd_power_dcdc0_mode_set(NRF_POWER_DCDC_ENABLE);   /* REG0 (nRF52840) */
    APP_ERROR_CHECK(err_code);
}

void stopAdvertisement(void)
{
    if (m_adv_handle != BLE_GAP_ADV_SET_HANDLE_NOT_SET) {
        (void)sd_ble_gap_adv_stop(m_adv_handle);
    }
}

void setMacAddress(uint8_t *addr)
{
    ble_gap_addr_t gap_addr;
    memcpy(gap_addr.addr, addr, sizeof(gap_addr.addr));
    gap_addr.addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;

    ret_code_t err_code = sd_ble_gap_addr_set(&gap_addr);
    APP_ERROR_CHECK(err_code);
}

void setAdvertisementData(uint8_t *data, uint8_t dlen)
{
    if (dlen > sizeof(m_enc_advdata)) dlen = sizeof(m_enc_advdata);
    memcpy(m_enc_advdata, data, dlen);
    m_adv_data.adv_data.len = dlen;
}

void startAdvertisement(int interval_ms, int tx_power)
{
    ret_code_t err_code;
    ble_gap_adv_params_t m_adv_params;

    memset(&m_adv_params, 0, sizeof(m_adv_params));

    /* NONCONNECTABLE_NONSCANNABLE: sends raw payload without Flags AD.
     * Required for Apple OF protocol (31-byte manufacturer payload). */
    m_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    m_adv_params.p_peer_addr     = NULL;
    m_adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval        = MSEC_TO_UNITS(interval_ms, UNIT_0_625_MS);
    m_adv_params.duration        = 0;  /* Infinite */
    m_adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &m_adv_params);
    APP_ERROR_CHECK(err_code);

    /* Set TX power for this specific advertising handle.
     * Lower power = less range but significant current savings.
     *   +4 dBm → 6.2 mA peak
     *    0 dBm → 4.8 mA peak  (~23% savings)
     *   -4 dBm → 4.1 mA peak  (~34% savings)
     *   -8 dBm → 3.7 mA peak  (~40% savings) */
    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV,
                                        m_adv_handle,
                                        (int8_t)tx_power);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}