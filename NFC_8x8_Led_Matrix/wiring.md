# NFC + 8x8 RGB Matrix Wiring Notes

Same deal as the matrix notes — writing this down before I inevitably rip everything apart to "clean the desk" and forget which pin went where.

This project taps an NFC tag, checks it against a saved code, and lights the matrix green or red depending on whether it matches.

---

# Parts Used

## Arduino Uno

Purpose:

* Main controller
* Runs the code
* Uploads sketches through USB

Typical current draw:

```text
~50mA to 70mA
```

Same Uno as always, nothing special here.

---

## PN532 NFC Module V3 (red PCB)

Specs:

* Reads/writes Mifare Classic tags over I2C
* This one only breaks out 4 pins (VCC, GND, SDA, SCL) — no IRQ or RESET
* DIP switch on the board sets I2C / SPI / HSU mode

Current draw:

```text
Idle / waiting for a tag   -> low, tens of mA
Actively scanning/reading  -> noticeably higher
Authenticating a block     -> the most demanding operation it does
```

This module drawing more current during authentication than during a plain UID read is exactly why a shaky power source will let simple reads work fine but make writes/auth randomly fail. Learned that one the hard way.

---

## 8x8 WS2812B RGB Matrix

Specs:

* 64 individually addressable RGB LEDs
* Single data line
* Used here just to flash green/red/blue depending on what happened

Current draw:

```text
LEDs off                 -> basically nothing
Typical use (this build)  -> low, since brightness is kept down
Full white, full bright   -> up to ~3.8A (not used here)
```

Brightness is kept low in the code on purpose — partly to save the eyes, partly because lower brightness means less current draw fighting with the NFC module for power.

---

## USB Cable

Purpose:

* Power
* Uploading code

Usually enough for this project, but if things start acting weird (module not found, auth randomly failing, garbled Serial output), a proper 5V wall adapter is worth trying before anything else. NFC + matrix + Serial all sharing one USB rail can get tight.

---

# Wiring

## PN532 NFC Module → Arduino

| PN532 Pin | Arduino Pin |
| --------- | ----------- |
| VCC       | 5V          |
| GND       | GND         |
| SDA       | A4          |
| SCL       | A5          |

DIP switch on the module needs to be set to I2C mode — check the silkscreen, it varies by seller.

No IRQ/RESET wiring needed on this board — the library just polls instead of using interrupts. Slightly less snappy in theory, not noticeable in practice at human tap speed.

## 8x8 Matrix → Arduino

| Matrix Pin | Arduino Pin              |
| ---------- | ------------------------- |
| DIN        | D6                        |
| VCC / 5V   | 5V                        |
| GND        | GND                       |

DIN doesn't have to be D6 specifically — any free digital pin works, same as the standalone matrix project. Just make sure `MATRIX_PIN` in the code matches whatever pin is actually wired.

Since both the NFC module and the matrix share the I2C bus (A4/A5) and the 5V/GND rails, they should both be tied to the same ground as the Uno — no separate ground planes here.

---

# I2C Address

Confirmed with an I2C scanner sketch:

```text
PN532 -> 0x1C (varies by board — mine reported 0x1C, not the usual 0x1D)
```

Always worth running a scanner first instead of assuming the default address, since some boards ship with SA0 wired differently than expected.

---

# Auth Key

Mifare Classic tags need a key to read/write most blocks. Default factory key on a blank/unmodified tag:

```text
FF FF FF FF FF FF
```

Used for both Key A and Key B in this project. If a tag doesn't authenticate with this at all (even after retries, even across every sector), it's probably not actually blank — mine turned out to have some leftover lock state from a previous format, and wiping it with a phone NFC app fixed it instantly.

---

# Brightness

```cpp
matrix.setBrightness(20);
```

Kept low here on purpose — this project doesn't need the matrix to be bright, just clearly green/red/blue, and lower brightness means less current draw competing with the NFC module for power on the shared 5V rail.

---

# Power Recommendations

## USB only

Fine for normal use — tap a tag, get a color, done.

## When to consider external power

If you start seeing:

* PN532 randomly not found at boot
* Authentication failing inconsistently
* Garbled/overlapping Serial output

...that's usually a power/brownout symptom, not a code bug. A proper 5V/2A+ wall adapter into the Uno's barrel jack gives way more headroom than USB off a laptop.

## Ground sharing

If ever moving to a separate power supply for the matrix specifically:

```text
Power supply GND
        |
Arduino GND
        |
Matrix GND / NFC GND
```

All grounds tied together, same as always, or nothing will make sense.

---

# Matrix Coordinate System / Layout

Same as the standalone matrix project — typewriter/progressive layout, top-left = (0,0).

```cpp
NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE
```

If the matrix lights up in a scrambled/wrong pattern, it's probably wired serpentine instead — swap `NEO_MATRIX_PROGRESSIVE` for `NEO_MATRIX_ZIGZAG` and it should sort itself out.

---

# Current Build

Hardware:

* Arduino Uno
* PN532 NFC Module V3 (I2C mode, 4-pin)
* 8x8 WS2812B RGB matrix
* Mifare Classic tag — the blue keychain thing, the sticker, or the credit-card-shaped one
* USB cable (For Data and power)
* Barrel Jack / External Power (If USB aint strong enough)
* Jumper wires

Status: works (for me). Tap a tag, `w <text>` to write a code to it, `set` to arm security mode, matrix goes green if it matches and red if it doesn't.

Took way longer to get here than expected — turns out the tag itself was the problem the whole time, not the wiring or the code. Should've tried formatting it with my phone on day one.
