/* AgentGlow — agent status LEDs over raw HID (VIA-compatible).
 *
 * Integration (keymap level, TYPE-9 Series III):
 *   1. Drop this file next to keymap.c and add it to SRC in rules.mk:
 *        SRC += agent_leds.c
 *      (RAW_ENABLE is already on via VIA_ENABLE.)
 *   2. This file defines via_command_kb() and
 *      rgb_matrix_indicators_advanced_user(). If your keymap already
 *      defines either, merge the bodies instead.
 *
 * Command IDs 0x51-0x53 sit outside VIA's range; anything else is
 * passed through untouched so VIA keeps working.
 *
 * TYPE-9 Series III LED map (WS2812 serpentine chain, keyboard.json):
 *
 *   visual grid        LED index
 *   [top row   ]  ->   6  7  8
 *   [middle row]  ->   5  4  3
 *   [bottom row]  ->   0  1  2
 *
 * Default agent keys = top row = LEDs 6, 7, 8 (set in the daemon's
 * config.json; nothing to configure on the firmware side).
 */

#include QMK_KEYBOARD_H
#include "raw_hid.h"

#define AG_CMD_SET_LED   0x51
#define AG_CMD_CLEAR_ALL 0x52
#define AG_CMD_HEARTBEAT 0x53

#define AG_MAX_LEDS      8
#define AG_TIMEOUT_MS    10000  /* no heartbeat -> all off */

enum { AG_MODE_OFF = 0, AG_MODE_SOLID, AG_MODE_BREATHE, AG_MODE_BLINK };

typedef struct {
    uint8_t led;
    uint8_t r, g, b;
    uint8_t mode;
} ag_entry_t;

static ag_entry_t ag_entries[AG_MAX_LEDS];
static uint8_t    ag_count = 0;
static uint32_t   ag_last_heartbeat = 0;

static void ag_set(uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t mode) {
    for (uint8_t i = 0; i < ag_count; i++) {
        if (ag_entries[i].led == led) {
            if (mode == AG_MODE_OFF) {           /* remove entry */
                ag_entries[i] = ag_entries[--ag_count];
            } else {
                ag_entries[i] = (ag_entry_t){led, r, g, b, mode};
            }
            return;
        }
    }
    if (mode != AG_MODE_OFF && ag_count < AG_MAX_LEDS) {
        ag_entries[ag_count++] = (ag_entry_t){led, r, g, b, mode};
    }
}

/* VIA calls this for every raw HID report before erroring on unknown
 * command IDs — we intercept ours and leave the rest to VIA. */
bool via_command_kb(uint8_t *data, uint8_t length) {
    switch (data[0]) {
        case AG_CMD_SET_LED:
            ag_set(data[1], data[2], data[3], data[4], data[5]);
            ag_last_heartbeat = timer_read32();
            return true;
        case AG_CMD_CLEAR_ALL:
            ag_count = 0;
            ag_last_heartbeat = timer_read32();
            return true;
        case AG_CMD_HEARTBEAT:
            ag_last_heartbeat = timer_read32();
            return true;
    }
    return false; /* not ours -> VIA handles it */
}

/* Triangle wave 0..255 for breathing, period ~2 s */
static uint8_t ag_breathe_val(void) {
    uint16_t t = (timer_read32() / 8) % 512;
    return (uint8_t)(t < 256 ? t : 511 - t);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (ag_count && timer_elapsed32(ag_last_heartbeat) > AG_TIMEOUT_MS) {
        ag_count = 0; /* daemon gone -> release the keys */
    }
    for (uint8_t i = 0; i < ag_count; i++) {
        ag_entry_t *e = &ag_entries[i];
        if (e->led < led_min || e->led >= led_max) continue;

        uint8_t r = e->r, g = e->g, b = e->b;
        switch (e->mode) {
            case AG_MODE_BREATHE: {
                uint16_t v = ag_breathe_val();
                r = (r * v) >> 8; g = (g * v) >> 8; b = (b * v) >> 8;
                break;
            }
            case AG_MODE_BLINK:
                if ((timer_read32() / 500) & 1) { r = g = b = 0; }
                break;
        }
        rgb_matrix_set_color(e->led, r, g, b);
    }
    return true;
}
