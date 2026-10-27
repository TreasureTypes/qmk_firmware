// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

/*
 * Standalone startup compositor. Not included in rules.mk yet.
 *
 * Future integration:
 * - Call startup_init() once after QMK initializes RGB Matrix.
 * - Keep rendering the user's selected effect into its normal buffer.
 * - At output time, pass each target color to startup_blend_color(). Send
 *   the returned color to the LEDs WITHOUT overwriting the effect buffer.
 *   This matters for incremental effects such as PIXEL_RAIN.
 * - Use black as the target when RGB is disabled. Startup output must still
 *   run in that case; USB suspend must suppress output altogether.
 * - Apply application status indicators after this compositor if they must
 *   take priority over the startup animation.
 *
 * Adding this file to SRC alone is intentionally insufficient: there are no
 * QMK hooks here, so it cannot conflict with agent.c or change RGB settings.
 * No EEPROM writes, blocking delays, or changes to the selected RGB mode.
 */

#define STARTUP_HOLD_MS          1000U
#define STARTUP_FADE_MS           500U
#define STARTUP_RAINBOW_PERIOD_MS 2000U

void startup_init(void);
bool startup_is_active(void);
rgb_t startup_blend_color(uint8_t led, rgb_t target);

static uint32_t startup_started;
static bool startup_running;

void startup_init(void) {
    startup_started = timer_read32();
    startup_running = true;
}

bool startup_is_active(void) {
    if (startup_running &&
        timer_elapsed32(startup_started) >= STARTUP_HOLD_MS + STARTUP_FADE_MS) {
        startup_running = false;
    }
    return startup_running;
}

// Smoothstep: zero slope at both ends, with an exact 0..255 output range.
static uint8_t startup_ease(uint16_t elapsed) {
    uint32_t t = (uint32_t)elapsed * 255U / STARTUP_FADE_MS;
    return (uint8_t)(t * t * (765U - 2U * t) / 65025UL);
}

static uint8_t startup_mix(uint8_t from, uint8_t to, uint8_t amount) {
    return ((uint32_t)from * (255U - amount) +
            (uint32_t)to * amount + 127U) / 255U;
}

rgb_t startup_blend_color(uint8_t led, rgb_t target) {
    if (!startup_is_active() || led >= RGB_MATRIX_LED_COUNT) {
        return target;
    }

    uint32_t elapsed = timer_elapsed32(startup_started);
    if (elapsed >= STARTUP_HOLD_MS + STARTUP_FADE_MS) {
        startup_running = false;
        return target;
    }

    // Same horizontal hue distribution as QMK's CYCLE_LEFT_RIGHT.
    // Independent startup speed; preserve the user's brightness setting.
    hsv_t hsv = {
        .h = (uint8_t)(g_led_config.point[led].x -
                      elapsed * 256UL / STARTUP_RAINBOW_PERIOD_MS),
        .s = 255,
        .v = rgb_matrix_get_val(),
    };
    rgb_t rainbow = hsv_to_rgb(hsv);

    if (elapsed < STARTUP_HOLD_MS) {
        return rainbow;
    }

    uint8_t amount = startup_ease((uint16_t)(elapsed - STARTUP_HOLD_MS));
    return (rgb_t){
        .r = startup_mix(rainbow.r, target.r, amount),
        .g = startup_mix(rainbow.g, target.g, amount),
        .b = startup_mix(rainbow.b, target.b, amount),
    };
}
