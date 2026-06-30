# HandMatrix

A modern, highly polished project that uses your webcam to detect hand gestures and instantly displays the number of fingers held up on an Arduino-driven 8x8 WS2812B LED Matrix.

## Features
- **Computer Vision:** High-performance MediaPipe-based hand tracking.
- **Robust Counting:** Built-in 300ms state machine to prevent flickering and ensure reliable inputs.
- **Auto-Detect:** Python automatically detects the Arduino COM port and seamlessly reconnects if unplugged.
- **Polished UI:** A sleek, transparent dark-mode UI with customizable skeleton drawing, FPS, and system status indicators.
- **Animations:** Hardware-accelerated (FastLED) non-blocking animations (Breathing, Fade, Grow, Pulse) on the Matrix.

## Hardware Requirements
- **Laptop / PC** with a webcam.
- **Arduino Uno R3**.
- **8x8 WS2812B LED Matrix** (Typewriter/Progressive layout).
- **USB Cable**.

## Wiring
1. **Matrix 5V** to Arduino **5V**
2. **Matrix GND** to Arduino **GND**
3. **Matrix DIN** to Arduino **D6**

## Arduino Setup
1. Open the `Arduino/HandMatrix` folder.
2. Open `HandMatrix.ino` in the Arduino IDE.
3. Install the **FastLED** library via the Library Manager (`Sketch > Include Library > Manage Libraries...`).
4. Select your Arduino Uno board and COM port.
5. Upload the sketch. The matrix should display a breathing blue '?'.

## Python Setup
1. Ensure you have Python 3.11+ installed.
2. Open a terminal in the project root directory.
3. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```
4. Run the application:
   ```bash
   python main.py
   ```

## Configuration
Edit `config.py` to customize colors, thresholds, the UI overlay, or to manually hardcode the COM port if auto-detection fails.

## Future Improvements
The modular structure of this project is designed for easy expansion:
- To add **Thumbs Up** or **Peace sign** recognition, edit `finger_counter.py` to check for specific landmark relationships instead of just counting extended fingers.
- To add **Scrolling Text** or **Emojis**, edit `digits.h` and `animations.cpp` to include new bitmaps and transition logic.

## Troubleshooting
- **No Arduino found:** Check your USB cable. Try hardcoding the `SERIAL_PORT` in `config.py` (e.g., `'COM3'` or `'/dev/ttyUSB0'`).
- **MediaPipe crashes / very slow:** Ensure your environment supports the required libraries and that your camera isn't locked by another app.
- **Matrix looks glitchy/wrong colors:** Change the `COLOR_ORDER` inside `matrix.h` (some matrices use `RGB` instead of `GRB`).

# !May have small glitches