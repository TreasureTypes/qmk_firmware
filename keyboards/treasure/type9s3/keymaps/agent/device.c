// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "raw_hid.h"
#include <string.h>

/* LED commands are implemented in agent.c. */
bool agent_command(uint8_t *data, uint8_t length);

/* Private HID IDs: check for VIA collisions when upgrading QMK. */
#define CMD_GET_INFO 0x54
#define CMD_ENTER_BOOTLOADER 0x55

#define DEVICE_PROTOCOL_VERSION 2
#define DEVICE_MODEL_ID         0x1C /* MPD003CE */
#define FIRMWARE_VERSION_MAJOR  1
#define FIRMWARE_VERSION_MINOR  0
#define FIRMWARE_VERSION_PATCH  2

bool via_command_kb(uint8_t *data, uint8_t length) {
    if (!length) return true; /* nothing for VIA to parse */
    switch (data[0]) {
        case CMD_GET_INFO:
            if (length != 32) return true;
            memset(data, 0, length);
            data[0] = CMD_GET_INFO;
            /* Model signature, followed by protocol and firmware versions. */
            data[1] = 'T'; data[2] = '9'; data[3] = 'S'; data[4] = '3';
            data[5] = DEVICE_PROTOCOL_VERSION;
            data[6] = FIRMWARE_VERSION_MAJOR;
            data[7] = FIRMWARE_VERSION_MINOR;
            data[8] = FIRMWARE_VERSION_PATCH;
            data[9] = RGB_MATRIX_LED_COUNT;
            data[10] = 0x0F; /* Supported modes 0..3, one bit per mode. */
            /* Single-byte model identifier; remaining reserved bytes stay zero. */
            data[11] = DEVICE_MODEL_ID;
            raw_hid_send(data, length);
            return true;
        case CMD_ENTER_BOOTLOADER:
            /* Accidental-command guard, not authentication. No HID reply. */
            if (length == 32 && data[1] == 'D' && data[2] == 'F' && data[3] == 'U' && data[4] == '!') {
                reset_keyboard();
            }
            return true;
    }
    return agent_command(data, length); /* unknown commands pass through to VIA */
}
