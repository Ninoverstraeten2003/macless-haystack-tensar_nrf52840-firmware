#ifndef NICENANO_H
#define NICENANO_H

#include "nrf_gpio.h"

// The Blue/Red LED on nice!nano is on P0.15
#define LEDS_NUMBER    1
#define LED_1          NRF_GPIO_PIN_MAP(0,15)
#define LEDS_ACTIVE_STATE 0 // 0 = Active Low (Typical for these boards)

#define LEDS_LIST { LED_1 }

#define LEDS_MASK      (1 << LED_1)  // Bitmask for all LEDs
#define LEDS_INV_MASK  LEDS_MASK     // Invert all LEDs because they are active-low

#define BSP_LED_0      LED_1

// Buttons (Pro Micro Pin 7 = P0.06)
#define BUTTONS_NUMBER 1
#define BUTTON_1       NRF_GPIO_PIN_MAP(0,6)
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1 }

#define BSP_BUTTON_0   BUTTON_1

// UART Pins (Standard Pro Micro TX/RX)
#define RX_PIN_NUMBER  NRF_GPIO_PIN_MAP(0,8)
#define TX_PIN_NUMBER  NRF_GPIO_PIN_MAP(0,6) // Note: Pin 7 is often reused for RX/TX in some configs
#define HWFC           false

#endif // NICENANO_H