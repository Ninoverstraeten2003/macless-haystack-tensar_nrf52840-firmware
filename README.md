# MaclessHaystack Firmware for Tensar nRF52840 Clone

TENSTAR 2pcs NRF52840 Development Board Compatible With Nice!Nano V2.0 Bluetooth Split Keyboard Pro Micro Red Board: https://nl.aliexpress.com/item/1005009890279520.html

Using:
- Adafruit_nRF52_Bootloader 0.9.2 with S140 v6.1.1 : https://github.com/adafruit/Adafruit_nRF52_Bootloader/releases/tag/0.9.2
- Nordic SDK 15.3.0
- Battery: CR2477 (non rechargeable)

| Component | Marking | Current Draw | Fix Priority |
| :--- | :--- | :--- | :--- |
| Silicon diode | W5 | ~60 µA | ⚠️ CRITICAL |
| Blue Power LED | | 1–2 mA | Remove physically or cut PCB trace |

Reverse Leakage Source: https://github.com/joric/nrfmicro/wiki/ALternatives

## Generate keys

```bash
# Generate 1 key (output in keygen/output/)
python3 keygen/keys.py
```

The advertisement key (`adv_b64`) is printed at the end:
```
→ Pass to firmware: make BASE64_KEY=<adv_b64> openhaystack
```

## Build

```bash
make clean
make BASE64_KEY=<adv_b64> openhaystack
```

## Flash

```bash
cp -X flash.uf2 /Volumes/NICENANO/
```

## Frontend
https://myfindr2.ninoverstraeten.com


