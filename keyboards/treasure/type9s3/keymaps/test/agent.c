// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#define CMD_SET_LED   0x51
#define CMD_CLEAR_ALL 0x52
#define CMD_HEARTBEAT 0x53

/* Called by the VIA dispatcher in device.c. */
bool agent_command(uint8_t *data, uint8_t length);

#define LED_MAX_LEDS      RGB_MATRIX_LED_COUNT
#define LED_TIMEOUT_MS    10000  /* no heartbeat -> release overrides */

enum { LED_MODE_OFF = 0, LED_MODE_SOLID, LED_MODE_BREATHE, LED_MODE_BLINK };

typedef struct {
    uint8_t led;
    uint8_t r, g, b;
    uint8_t mode;
} led_entry_t;

static led_entry_t led_entries[LED_MAX_LEDS];
static uint8_t    led_count = 0;
static uint32_t   led_last_heartbeat = 0;

static void status_led_set(uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t mode) {
    for (uint8_t i = 0; i < led_count; i++) {
        if (led_entries[i].led == led) {
            if (mode == LED_MODE_OFF) {           /* remove entry */
                led_entries[i] = led_entries[--led_count];
            } else {
                led_entries[i] = (led_entry_t){led, r, g, b, mode};
            }
            return;
        }
    }
    if (mode != LED_MODE_OFF && led_count < LED_MAX_LEDS) {
        led_entries[led_count++] = (led_entry_t){led, r, g, b, mode};
    }
}

bool agent_command(uint8_t *data, uint8_t length) {
    if (!length) return true;
    switch (data[0]) {
        case CMD_SET_LED:
            /* Consume malformed private commands without changing state. */
            if (length < 6 || data[1] >= LED_MAX_LEDS || data[5] > LED_MODE_BLINK) return true;
            status_led_set(data[1], data[2], data[3], data[4], data[5]);
            led_last_heartbeat = timer_read32();
            return true;
        case CMD_CLEAR_ALL:
            led_count = 0;
            led_last_heartbeat = timer_read32();
            return true;
        case CMD_HEARTBEAT:
            led_last_heartbeat = timer_read32();
            return true;
    }
    return false; /* not ours -> VIA handles it */
}

/* Triangle wave 0..255 for breathing, period 4.096 s */
static uint8_t led_breathe_val(void) {
    uint16_t t = (timer_read32() / 8) % 512;
    return (uint8_t)(t < 256 ? t : 511 - t);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (led_count && timer_elapsed32(led_last_heartbeat) > LED_TIMEOUT_MS) {
        led_count = 0; /* daemon gone -> release the keys */
    }
    for (uint8_t i = 0; i < led_count; i++) {
        led_entry_t *e = &led_entries[i];
        if (e->led < led_min || e->led >= led_max) continue;

        uint8_t r = e->r, g = e->g, b = e->b;
        switch (e->mode) {
            case LED_MODE_BREATHE: {
                uint16_t v = led_breathe_val();
                r = (r * v) >> 8; g = (g * v) >> 8; b = (b * v) >> 8;
                break;
            }
            case LED_MODE_BLINK:
                if ((timer_read32() / 500) & 1) { r = g = b = 0; }
                break;
        }
        rgb_matrix_set_color(e->led, r, g, b);
    }
    return true;
}
