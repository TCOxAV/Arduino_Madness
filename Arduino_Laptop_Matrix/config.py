# config.py
# Centralized configuration settings for the HandMatrix project

# Camera Settings
CAMERA_INDEX = 0
CAMERA_WIDTH = 1280
CAMERA_HEIGHT = 720
TARGET_FPS = 30

# MediaPipe Settings
MAX_HANDS = 2
DETECTION_CONFIDENCE = 0.5
TRACKING_CONFIDENCE = 0.5

# Serial Settings
SERIAL_BAUD_RATE = 115200
# Set to a string like 'COM3' or '/dev/ttyUSB0' to hardcode. 
# Leave as None to auto-detect Arduino.
SERIAL_PORT = None 

# Stability Settings
# How long (in seconds) the same finger count must be detected before sending
STABILITY_DELAY_SEC = 0.3

# UI Settings
UI_FONT = 0 # cv2.FONT_HERSHEY_SIMPLEX
UI_FONT_SCALE = 0.7
UI_THICKNESS = 2

# Colors (BGR format for OpenCV)
COLOR_TEXT = (255, 255, 255)         # White
COLOR_ACCENT = (255, 150, 0)         # Blue-ish Accent
COLOR_PANEL_BG = (20, 20, 20)        # Dark Gray
COLOR_PANEL_ALPHA = 0.6              # Transparency for panels

# Skeleton Colors
COLOR_LANDMARK = (0, 255, 255)       # Yellow
COLOR_CONNECTION = (200, 200, 200)   # Light Gray
COLOR_FINGERTIP = (0, 0, 255)        # Red
COLOR_PALM = (0, 255, 0)             # Green

# Status Colors
COLOR_SUCCESS = (0, 255, 0)
COLOR_WARNING = (0, 165, 255)
COLOR_ERROR = (0, 0, 255)
