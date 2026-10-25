# TYPE-9 Series III status LEDs and firmware control

Build with `qmk compile -kb treasure/type9s3 -km agent`.

This keymap adds temporary per-LED RGB overrides while retaining VIA. All
nine LEDs can be controlled simultaneously. The override array uses 45 bytes
of RAM (five bytes per LED); there is no dynamic allocation or EEPROM write.

## Wire protocol

Send 32-byte QMK Raw HID reports, padding unused bytes with zero. When the
host HID library requires a report ID, prepend a zero report ID; it is not
part of the payload offsets below. Only GET_INFO replies; LED commands remain fire-and-forget.

| Byte 0 | Command | Remaining payload |
| --- | --- | --- |
| `0x51` | Set LED | Byte 1: LED index; bytes 2–4: red, green, blue (0–255); byte 5: mode |
| `0x52` | Clear all overrides | None |
| `0x53` | Heartbeat | None |

Modes: `0` releases the LED, `1` is solid, `2` breathes with a 4.096-second
cycle, and `3` blinks with 500 ms on / 500 ms off. Colors are applied directly;
the RGB Matrix brightness setting does not scale the overrides.

LED indices, viewed from above:

```text
6 7 8
5 4 3
0 1 2
```

Short set commands, indices outside 0–8, and modes outside 0–3 are ignored
without refreshing the timeout. Empty reports are consumed. Other command
IDs pass through to VIA unchanged.

Every valid LED command (0x51–0x53) refreshes one shared timeout for all overrides.
Send heartbeats comfortably within ten seconds (for example, every second).
After more than ten seconds without a valid command, the next indicator
render releases all overrides and normal RGB effects resume. Clearing or
releasing an LED also restores the normal effect; it does not force black.

Indicators require RGB Matrix to be enabled and rendering. Turning RGB off
or suspending the keyboard also stops the status indicators. Timeout cleanup
runs when indicator rendering resumes.

The private IDs have no reserved VIA namespace.
When updating QMK, check `quantum/via.h` and `quantum/via.c` for command
collisions and rebuild. Keep the host daemon compatible with this format.


## Firmware information and update entry

`keymap.c` contains only key mappings. `agent.c` handles LED commands and
rendering; `device.c` dispatches HID commands and handles firmware information
and DFU entry. Both source files are compiled through `rules.mk`. The existing `agent` keymap directory
name is retained so build commands continue to work.

Send a zero-padded 32-byte report with byte 0 set to `0x54` (GET_INFO).
The firmware replies using the received buffer, without persistent RAM state:

| Response byte | Meaning | Initial value |
| --- | --- | --- |
| 0 | Command | `0x54` |
| 1–4 | Model signature, ASCII | `T9S3` |
| 5 | Protocol version | `1` |
| 6–8 | Firmware major, minor, patch | `1, 0, 0` |
| 9 | LED count | `9` |
| 10 | Supported modes, bit N represents mode N | `0x0F` |
| 11–31 | Reserved | `0` |

Increment the firmware version constants in `device.c` for each release.
Increment the protocol version for incompatible wire-format changes. The
application must check the model and protocol before using new commands;
a VIA protocol version is not this firmware version. GET_INFO does not
refresh the LED timeout.

After the user requests an update, send `0x55, 'D', 'F', 'U', '!'`, padded
with zeros to 32 bytes (ENTER_BOOTLOADER). The firmware calls QMK's
`reset_keyboard()` to enter the existing Atmel DFU bootloader. There is no
reply: the application must close its HID handle and wait for the DFU
USB device, then flash using its host-side updater. Reports of the wrong
length or with a wrong confirmation sequence are ignored. The sequence
prevents accidental activation; it is not authentication.

No firmware download or flashing code is added to the application firmware.
The updater and its packaging must be implemented separately on the host.

With the default keymap, manual DFU entry remains available: hold top-left,
then top-right, then bottom-right, then press bottom-left (`QK_BOOT`). VIA
remapping can change this sequence. Test DFU entry and recovery on hardware
before shipping a release.
