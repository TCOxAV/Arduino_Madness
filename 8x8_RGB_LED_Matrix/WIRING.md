# 8x8 RGB Matrix Wiring Notes

Made this so i stop forgetting how i wired stuff after 2 days 😭

Every time i disconnect everything to "clean up the desk", i somehow end up spending 30 minutes figuring out where the data wire went.

So here's the notes.

---

# Parts Used

## Arduino Uno

Purpose:

* Main controller
* Runs the code
* Uploads sketches through USB

Typical Current Draw:

```text
~50mA to 70mA
```

Nothing fancy. Just a normal Uno.

---

## 8x8 WS2812B RGB Matrix

Specifications:

* 64 individually addressable RGB LEDs
* Controlled using a single data line
* Each LED can display its own color

Current Draw:

```text
LEDs Off                 -> Very Low
Typical Animations       -> ~200mA to 800mA
Full White Full Bright   -> Up to ~3.8A
```

Calculation:

```text
64 LEDs × 60mA = 3840mA ≈ 3.8A
```

I almost never run full white at maximum brightness tho. Its stupidly bright and honestly hurts ur eyes after a bit.

---

## USB Cable

Purpose:

* Power
* Uploading code

Usually enough for:

* Snake
* Aquarium
* Matrix Rain
* Fireworks
* Text scrolling
* Random animations i made at 2AM

---

# Wiring

## Matrix → Arduino

| Matrix Pin | Arduino Pin              |
| ---------- | ------------------------ |
| DIN        | Any Digital Pin (D2-D13) |
| VCC / 5V   | 5V                       |
| GND        | GND                      |

The data line DOES NOT have to be D6.

Most examples use D6 simply becoz thats what the author happened to connect.

These pins all work:

```text
D2
D3
D4
D5
D6
D7
D8
D9
D10
D11
D12
D13
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

All are valid.

Just make sure the code matches the actual wire.

Example:

If DIN is connected to D6:

```cpp
#define LED_PIN 6
```

If DIN is connected to D10:

```cpp
#define LED_PIN 10
```

If DIN is connected to D3:

```cpp
#define LED_PIN 3
```

This is probably one of the easiest mistakes to make when ur tired.

---

# PWM Note

Some Arduino pins support PWM:

```text
D3
D5
D6
D9
D10
D11
```

For WS2812B matrices, PWM support generally doesn't matter.

FastLED generates the required signal itself.

So both PWM and non-PWM digital pins work.

The important part is:

```text
Physical Wire
      ↕
LED_PIN in Program
```

If those match, ur good.

---

# Required Components

## 330Ω Resistor (Required)

>Can be given as 330E or 330R if u have those assorted resistor box

>Color Code: Orange, Orange, Brown, Gold/Brown

Purpose:

* Protects the first LED.
* Helps reduce signal ringing.
* Helps prevent signal spikes.
* Recommended in basically every WS2812B guide.

Connection:

```text
Arduino Digital Pin
        |
       330Ω
        |
       DIN
```

Current:

```text
Very small
Signal line only
```

Even though the Arduino is only outputting 5V, i still consider this required.

Sure, plenty of people run without it.

Plenty of people also come back later wondering why their first LED randomly started acting weird.

A resistor costs almost nothing and takes like 10 seconds to install.

Might as well do it properly.

---

# Optional Components

## 1000µF Capacitor (Optional)

Purpose:

* Helps absorb startup power surges.
* Helps stabilize power.
* Can prevent voltage dips during large brightness changes.

Connection:

```text
+5V ----||---- GND
       1000µF
```

Observe polarity.

### When I'd Use It

* External power supply
* High brightness
* Lots of white LEDs
* Large matrices
* Long LED strips

### When I Probably Wouldn't Worry About It

* Small 8x8 matrix
* Lower brightness
* USB power
* Normal animations

For this kind of setup, many people run without one and never have issues.

So i consider it optional, but definitely a nice thing to have.

---

# Brightness

Typical setting:

```cpp
FastLED.setBrightness(80);
```

Reasons:

* Less power consumption
* Less heat
* Easier on the eyes
* USB power is usually enough

Also prevents accidental retinal damage when u upload the wrong sketch.

---

# Power Recommendations

## USB Only

Usually fine for:

* Snake
* Aquarium
* Matrix Rain
* Fireworks
* Text Effects
* Most experiments

---

## External Power Supply

Recommended when:

* Running high brightness
* Large amounts of white
* Bigger matrices
* Long LED strips

Suggested Supply:

```text
5V
5A or greater preferred
```

More available current is fine.

The LEDs only draw what they need.

---

## Ground Sharing

Important.

If using an external power supply:

```text
Power Supply GND
        |
Arduino GND
        |
Matrix GND
```

All grounds should be connected together.

Otherwise u enter the fun debugging phase where absolutely nothing makes sense.

---

# Matrix Coordinate System

My code assumes:

```text
Top Left     = (0,0)
Bottom Right = (7,7)
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

Not every RGB matrix is wired the same internally.

This changes how the XY() function works.

---

## Typewriter / Progressive Layout

This is what my matrix uses.

Rows continue in the same direction.

Example:

```text
0   1   2   3
4   5   6   7
8   9  10  11
12 13 14 15
```

Example function:

```cpp
int XY(int x, int y) {
    return y * 8 + x;
}
```

---

## Serpentine Layout

Probably the most common layout online.

Rows alternate direction.

Example:

```text
0   1   2   3
7   6   5   4
8   9  10  11
15 14 13 12
```

Needs a different XY() function.

---

## Column-Major Layout

Less common.

Wired by columns instead of rows.

Example:

```text
0   4   8  12
1   5   9  13
2   6  10  14
3   7  11  15
```

Also needs a different XY() function.

---

# Current Build

Current hardware:

* Arduino Uno
* 8x8 WS2812B RGB Matrix
* USB Cable
* Jumper Wires
* 330Ω Resistor

Optional:

* 1000µF Capacitor

Current status:

Works.

Every time i learn a new FastLED function, i somehow end up making another animation instead of finishing whatever project i originally started.
