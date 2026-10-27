// Copyright 2023 Treasure
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT_ortho_3x3(
        KC_KP_1,   KC_KP_2, KC_KP_3,
        KC_KP_4, KC_KP_5, KC_KP_6,
        KC_KP_7, KC_KP_8, KC_KP_9
    )
};