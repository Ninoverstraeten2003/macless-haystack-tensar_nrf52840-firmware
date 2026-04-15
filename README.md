# MaclessHaystack nRF52840 — CR2477 Beacon

Apple FindMy beacon firmware for the TENSTAR ProMicro nRF52840 (nice!nano v2 clone), powered by a CR2477 coin cell.

**Board:** [TENSTAR NRF52840 ProMicro (AliExpress)](https://nl.aliexpress.com/item/1005009890279520.html)

## Stack

| Component | Version |
|---|---|
| Bootloader | Adafruit nRF52 Bootloader 0.9.2 (stock 0.6.0 also works) |
| SoftDevice | S140 v6.1.1 |
| SDK | Nordic SDK 15.3.0 |
| Battery | CR2477 (1000 mAh, non-rechargeable) |

## Firmware Power Profile

| Feature | Setting | Impact |
|---|---|---|
| Advertising interval | 2000 ms | ~5 µA avg BLE current |
| TX power | 0 dBm | 4.8 mA peak, ~60m range |
| DC/DC REG1 (VDD → 1.1V core) | Enabled | Saves ~20-30% vs nRF52840's own internal LDO |
| Debug LEDs | Disabled | Zero LED drain |
| Unused GPIOs | Disconnected (24 pins) | Prevents leakage |
| Peripherals (UART, SPI, I2C) | All disabled | Zero overhead |
| Logging | Disabled | Zero overhead |
| Battery monitoring | Weekly (SAADC single-shot) | Negligible |
| Advertising type | Non-connectable, non-scannable | No connection overhead |

## ⚠️ Board Modifications (TENSTAR "Bad Batch")

Some TENSTAR boards ship with a **W5 silicon diode** that has ~60 µA reverse leakage — this single component kills 80% of battery life.

Reference: [joric/nrfmicro — Alternatives](https://github.com/joric/nrfmicro/wiki/ALternatives)

### Board Components

| Component | Marking | Type | Current Draw | Action |
|---|---|---|---|---|
| **Silicon diode** | W5 | SOD-323 | **~60 µA** reverse leakage | 🔴 Remove & bridge pads |
| **Battery charger IC** | LTH7R | SOT-23-5 | ~3 µA quiescent | 🟡 Remove (CR2477 is non-rechargeable) |
| **LDO regulator** | A5X2Q (ME6217C33M5G) | SOT-23-5 | ~5 µA quiescent | 🟠 Keep (needed for USB-C flashing) |
| **RTC crystal** | DA538C | 32.768 kHz | ~0.25 µA | 🟢 Optional: use internal RC instead |
| **BLE crystal** | 32.000 MHz | Metal can | Required | ✅ Keep |
| **Unknown IC** | A19T | SOT-23 | Unknown | ❓ Needs identification |

### Power Path

```
                         MAIN POWER PATH
                         ===============
USB VBUS --> [W5 diode] --> RAW pin --> [LDO] --> VCC pin (LDO output) --> nRF52840
              60uA leak!       |        5uA
                               |      quiescent
                               |
                         SIDE BRANCH
                         -----------
                               |
                               +--> [LTH7R VIN]
                                       |  3uA quiescent
                                  [LTH7R VBAT]
                                       |
                                  B+/B- pads (LiPo, not used)

                         CORRECT BATTERY PATH
                         ====================
CR2477 (+) --> VDD pad --> nRF52840 (bypasses LDO entirely)
CR2477 (-) --> GND
```

> [!CAUTION]
> **Do NOT wire CR2477 to the VCC pin.** VCC is the LDO *output* — the internal switch chokes current during the BLE radio's 4.8 mA spike, causing the voltage to drop below 1.7 V. The nRF52840's brownout-reset fires, the CPU immediately reboots, and the loop repeats thousands of times per hour. This drains a 1000 mAh CR2477 in ~48 hours.

- **CR2477 (3.0V) cannot drive the LDO** (needs ≥3.4V input) → wire directly to the **VDD pad**, bypassing the LDO entirely
- **Removing W5 without bridging** breaks USB-C power but also eliminates 60 µA leakage
- **Bridging W5 pads** restores USB-C AND eliminates 60 µA leakage
- **LTH7R is not in the main path** — removing it breaks nothing, saves 3 µA
- **Always remove CR2477 before plugging USB-C** (no charger = backfeed risk)

### Battery Life by Configuration

| Configuration | Modification | Total µA | CR2477 Life | USB-C? |
|---|---|---|---|---|
| Stock (no mods) | — | 76 µA | ❌ ~14 months | ✅ |
| **W5 bridged** | Remove diode, solder bridge | 16 µA | ✅ ~5 years | ✅ |
| **W5 bridged + no LTH7R** | + remove charger IC | 13 µA | ✅ ~6 years | ✅ |
| W5 removed + no LTH7R + no LDO | + remove LDO (all 3 ICs gone) | 8 µA | ✅ ~7-8 years | ❌ |

> Battery life includes ~3%/year CR2477 self-discharge. Conservative estimates.

### Recommended Modification (Keeps USB-C)

1. **Remove W5 diode, bridge the pads** — biggest impact (−60 µA)
2. **Remove LTH7R charger IC** — prevents accidental CR2477 charging (−3 µA)
3. **Keep LDO in place** — preserves USB-C power for flashing
4. **Wire CR2477 (+) → VDD pad, (−) → GND** — bypasses LDO, feeds the nRF52840 rail directly

> [!WARNING]
> **VCC ≠ VDD on this board.** VCC is the LDO output (a choked, regulated rail). VDD is the direct supply rail to the nRF52840. Wiring to VCC caused a brownout-reset loop and drained a full CR2477 in 2 days.

## Generate Keys

```bash
python3 keygen/keys.py
```

The advertisement key (`adv_b64`) is printed at the end.

## Build

```bash
make clean
make BASE64_KEY=<adv_b64> openhaystack
```

## Flash

Short the reset pin to ground by a quick tap to enter bootloader, then:

```bash
cp -X flash.uf2 /Volumes/NICENANO/
```

## Frontend

https://myfindr2.ninoverstraeten.com
