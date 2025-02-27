// TREASURE TYPE 30
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/* COMBO FUNCTION */
#define COMBO_TERM 150        // how quickly all combo keys must be pressed in succession to trigger
//#define COMBO_MUST_HOLD_MODS // if a combo triggers a modifier, only trigger when the combo is held
//#define COMBO_HOLD_TERM 175  // how long at least one of the combo keys must be held to trigger

/* disable action features */


#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT

#define RGB_MATRIX_LED_COUNT 36
#define WS2812_DI_PIN B4
#define DRIVER_LED_TOTAL 36
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 180

#define RGB_MATRIX_STARTUP_MODE RGB_MATRIX_GRADIENT_LEFT_RIGHT // Sets the default mode, if none has been set
#define RGB_MATRIX_DEFAULT_SPD 40 // Sets the default animation speed, if none has been set

//#define RGB_MATRIX_LED_FLUSH_LIMIT 32 // limits in milliseconds how frequently an animation will update the LEDs. 16 (16ms) is equivalent to limiting to 60fps (increases keyboard responsiveness) _ Default is 16
//#define RGB_MATRIX_LED_PROCESS_LIMIT (RGB_MATRIX_LED_COUNT + 4) / 6 // limits the number of LEDs to process in an animation per task run (increases keyboard responsiveness) - Default is 5

#define RGB_MATRIX_FRAMEBUFFER_EFFECTS

#define RGB_MATRIX_SLEEP

#define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_BREATHING
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS

//#define ENABLE_RGB_MATRIX_ALPHAS_MODS
//#define ENABLE_RGB_MATRIX_BAND_SAT
//#define ENABLE_RGB_MATRIX_BAND_VAL
//#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
//#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
//#define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
//#define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
//#define ENABLE_RGB_MATRIX_CYCLE_ALL
//#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
//#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
//#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
//#define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
//#define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
//#define ENABLE_RGB_MATRIX_DUAL_BEACON
//#define ENABLE_RGB_MATRIX_RAINBOW_BEACON
//#define ENABLE_RGB_MATRIX_RAINDROPS
//#define ENABLE_RGB_MATRIX_HUE_BREATHING
//#define ENABLE_RGB_MATRIX_HUE_PENDULUMZ
//#define ENABLE_RGB_MATRIX_HUE_WAVE
//#define ENABLE_RGB_MATRIX_PIXEL_FRACTAL
//#define ENABLE_RGB_MATRIX_PIXEL_RAIN

#ifdef RGB_MATRIX_FRAMEBUFFER_EFFECTS
    #define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#endif

