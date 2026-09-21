# Agent status LEDs

Build with `qmk compile -kb treasure/type9s3 -km agent`.

This keymap adds temporary per-LED RGB overrides while retaining VIA. All
nine LEDs can be controlled simultaneously. The override array uses 45 bytes
of RAM (five bytes per LED); there is no dynamic allocation or EEPROM write.

## Existing wire protocol

Send 32-byte QMK Raw HID reports, padding unused bytes with zero. When the
host HID library requires a report ID, prepend a zero report ID; it is not
part of the payload offsets below. Commands are fire-and-forget, with no reply.

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

Every valid private command refreshes one shared timeout for all overrides.
Send heartbeats comfortably within ten seconds (for example, every second).
After more than ten seconds without a valid command, the next indicator
render releases all overrides and normal RGB effects resume. Clearing or
releasing an LED also restores the normal effect; it does not force black.

Indicators require RGB Matrix to be enabled and rendering. Turning RGB off
or suspending the keyboard also stops the agent indicators. Timeout cleanup
runs when indicator rendering resumes.

The private IDs have no version negotiation or reserved VIA namespace.
When updating QMK, check `quantum/via.h` and `quantum/via.c` for command
collisions and rebuild. Keep the host daemon compatible with this format.
