# 8x8 RGB Matrix Wiring Notes

Made this so I stop forgetting how I wired everything after like 2 days

Every single time I disconnect stuff to "clean up the desk," I end up spending 30 minutes later trying to remember where the data wire went. So here's the notes, finally.

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

Nothing fancy, just a normal Uno.

---

## 8x8 WS2812B RGB Matrix

Specs:

* 64 individually addressable RGB LEDs
* Controlled over a single data line
* Each LED can be set to its own color independently

Current draw:

```text
LEDs off                 -> basically nothing
Typical animations       -> ~200mA to 800mA
Full white, full bright  -> up to ~3.8A
```

Where that number comes from:

```text
64 LEDs × 60mA = 3840mA ≈ 3.8A
```

I almost never actually run full white at max brightness though. It's stupidly bright and kind of hurts to look at after a few seconds.

---

## USB Cable

Purpose:

* Power
* Uploading code

Usually enough for:

* Snake
* Aquarium
* Matrix rain
* Fireworks
* Text scrolling
* Whatever random animation I made at 2AM

---

# Wiring

## Matrix → Arduino

| Matrix Pin | Arduino Pin               |
| ---------- | -------------------------- |
| DIN        | Any digital pin (D2–D13)   |
| VCC / 5V   | 5V                          |
| GND        | GND                         |

The data line does **not** have to be D6.

Most tutorials use D6 just because that's what the author happened to wire up.

Any of these work fine:

```text
D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13
```

Examples:

```text
Arduino D6  --------> DIN
Arduino 5V  --------> VCC
Arduino GND --------> GND
```

or

```text
Arduino D10 --------> DIN
Arduino 5V  --------> VCC
Arduino GND --------> GND
```

or

```text
Arduino D3  --------> DIN
Arduino 5V  --------> VCC
Arduino GND --------> GND
```

All valid — just make sure the code actually matches the wire.

If DIN is on D6:

```cpp
#define LED_PIN 6
```

If DIN is on D10:

```cpp
#define LED_PIN 10
```

If DIN is on D3:

```cpp
#define LED_PIN 3
```

Probably one of the easiest mistakes to make when you're tired and just copy-pasting.

---

# PWM Note

Some Arduino pins support PWM:

```text
D3, D5, D6, D9, D10, D11
```

For WS2812B matrices, this doesn't actually matter. FastLED generates the timing signal itself in software, so both PWM and non-PWM digital pins work fine.

All that actually matters:

```text
Physical wire
      ↕
LED_PIN in code
```

As long as those two agree, you're good.

---

# Required Components

## 330Ω Resistor (Required)

> Sometimes labeled 330E or 330R in assorted resistor kits
>> Color code: Orange, Orange, Brown, Gold/Brown

Purpose:

* Protects the first LED
* Helps reduce signal ringing
* Helps prevent signal spikes
* Recommended in basically every WS2812B guide out there

Connection:

```text
Arduino digital pin
        |
       330Ω
        |
       DIN
```

Current through it is tiny — it's just on the signal line, not power.

Even though the Arduino only outputs 5V, I still treat this as required. Plenty of people skip it and it works fine... until their first LED starts acting weird for no obvious reason. Resistor costs pennies and takes 10 seconds to add, so might as well.

---

# Optional Components

## 1000µF Capacitor (Optional)

Purpose:

* Absorbs startup power surges
* Helps stabilize the power line
* Can prevent voltage dips during big brightness changes

Connection:

```text
+5V ----||---- GND
       1000µF
```

Watch polarity.

### When I'd actually use one

* External power supply
* High brightness
* Lots of white pixels
* Bigger matrices / longer strips

### When I wouldn't bother

* Small 8x8 matrix
* Lower brightness
* USB power
* Normal animations

For a setup like mine, most people run without one and never run into problems. So — optional, but nice to have if you're scaling up.

---

# Brightness

Typical setting I use:

```cpp
FastLED.setBrightness(80);
```

Why:

* Less power draw
* Less heat
* Easier on the eyes
* USB power is usually enough at this level

Also saves you from accidental retinal damage the one time you upload the wrong sketch.

---

# Power Recommendations

## USB only

Usually fine for:

* Snake
* Aquarium
* Matrix rain
* Fireworks
* Text effects
* Most experiments

## External power supply

Worth using when:

* Running high brightness
* Lots of white
* Bigger matrices
* Long LED strips

Suggested:

```text
5V, 5A or more
```

More available current than you need is totally fine — the LEDs only pull what they actually need.

## Ground sharing

Important one. If you're using an external supply:

```text
Power supply GND
        |
Arduino GND
        |
Matrix GND
```

All grounds need to be tied together, or you enter the fun debugging phase where nothing makes sense anymore.

---

# Matrix Coordinate System

My code assumes:

```text
Top left     = (0,0)
Bottom right = (7,7)
```

Layout:

```text
+------+------+------+------+------+------+------+
|0,0  |1,0  |2,0  |3,0  |4,0  |5,0  |6,0  |7,0  |
+------+------+------+------+------+------+------+
|0,1  |1,1  |2,1  |3,1  |4,1  |5,1  |6,1  |7,1  |
+------+------+------+------+------+------+------+
|0,2  |1,2  |2,2  |3,2  |4,2  |5,2  |6,2  |7,2  |
+------+------+------+------+------+------+------+
|0,3  |1,3  |2,3  |3,3  |4,3  |5,3  |6,3  |7,3  |
+------+------+------+------+------+------+------+
|0,4  |1,4  |2,4  |3,4  |4,4  |5,4  |6,4  |7,4  |
+------+------+------+------+------+------+------+
|0,5  |1,5  |2,5  |3,5  |4,5  |5,5  |6,5  |7,5  |
+------+------+------+------+------+------+------+
|0,6  |1,6  |2,6  |3,6  |4,6  |5,6  |6,6  |7,6  |
+------+------+------+------+------+------+------+
|0,7  |1,7  |2,7  |3,7  |4,7  |5,7  |6,7  |7,7  |
+------+------+------+------+------+------+------+
```

---

# Matrix Wiring Layout Types

Not every RGB matrix is wired the same way internally — this changes how the `XY()` function needs to work.

## Typewriter / Progressive layout

This is what my matrix uses. Rows all continue in the same direction.

```text
0   1   2   3
4   5   6   7
8   9   10  11
12  13  14  15
```

```cpp
int XY(int x, int y) {
    return y * 8 + x;
}
```

## Serpentine layout

Probably the most common layout you'll find online. Rows alternate direction.

```text
0   1   2   3
7   6   5   4
8   9   10  11
15  14  13  12
```

Needs a different `XY()` function.

## Column-major layout

Less common — wired by columns instead of rows.

```text
0   4   8  12
1   5   9  13
2   6  10  14
3   7  11  15
```

Also needs its own `XY()` function.

---

# Current Build

Hardware:

* Arduino Uno
* 8x8 WS2812B RGB matrix
* USB cable (For data and power)
* Barrel Jack  (If usb cant handle the more need of power)
* Jumper wires
* 330Ω resistor

Optional:

* 1000µF capacitor

Status: works.

Every time I learn a new FastLED function I somehow end up making another animation instead of finishing whatever project I originally started.
