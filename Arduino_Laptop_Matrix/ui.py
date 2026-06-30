import cv2
import numpy as np
import config

class UIManager:
    """
    Handles all visual overlays and drawing functions for the main display.
    """
    def __init__(self):
        self.font = config.UI_FONT
        self.font_scale = config.UI_FONT_SCALE
        self.thickness = config.UI_THICKNESS

    def draw_skeleton(self, frame, hand_landmarks):
        """
        Draws the hand skeleton on the frame with custom colors.
        """
        if not hand_landmarks:
            return

        h, w, c = frame.shape

        # Draw Connections
        connections = [
            (0, 1), (1, 2), (2, 3), (3, 4),        # Thumb
            (0, 5), (5, 6), (6, 7), (7, 8),        # Index
            (5, 9), (9, 10), (10, 11), (11, 12),   # Middle
            (9, 13), (13, 14), (14, 15), (15, 16), # Ring
            (13, 17), (0, 17), (17, 18), (18, 19), (19, 20) # Pinky
        ]

        for connection in connections:
            idx1, idx2 = connection
            lm1 = hand_landmarks[idx1]
            lm2 = hand_landmarks[idx2]
            
            x1, y1 = int(lm1.x * w), int(lm1.y * h)
            x2, y2 = int(lm2.x * w), int(lm2.y * h)
            
            cv2.line(frame, (x1, y1), (x2, y2), config.COLOR_CONNECTION, 2)

        # Draw Landmarks
        for id, lm in enumerate(hand_landmarks):
            cx, cy = int(lm.x * w), int(lm.y * h)
            
            color = config.COLOR_LANDMARK
            radius = 5
            
            if id in [4, 8, 12, 16, 20]: # Fingertips
                color = config.COLOR_FINGERTIP
                radius = 7
            elif id == 0: # Palm/Wrist
                color = config.COLOR_PALM
                radius = 8
                
            cv2.circle(frame, (cx, cy), radius, color, cv2.FILLED)

    def draw_overlay(self, frame, fps, raw_count, stable_count, serial_connected, port_name):
        """
        Draws a modern, dark translucent overlay with system info.
        """
        h, w, _ = frame.shape
        
        # Create a translucent panel at the top
        overlay = frame.copy()
        cv2.rectangle(overlay, (0, 0), (w, 60), config.COLOR_PANEL_BG, cv2.FILLED)
        cv2.rectangle(overlay, (0, h - 40), (w, h), config.COLOR_PANEL_BG, cv2.FILLED)
        
        # Apply alpha
        cv2.addWeighted(overlay, config.COLOR_PANEL_ALPHA, frame, 1 - config.COLOR_PANEL_ALPHA, 0, frame)
        
        # --- Top Panel ---
        # FPS
        cv2.putText(frame, f"FPS: {int(fps)}", (20, 40), self.font, self.font_scale, config.COLOR_TEXT, self.thickness)
        
        # Finger Count
        count_str = str(stable_count) if stable_count is not None else "--"
        cv2.putText(frame, f"Fingers: {count_str}", (200, 40), self.font, self.font_scale, config.COLOR_ACCENT, self.thickness + 1)
        
        # Tracking Status
        tracking_status = "Tracking" if raw_count is not None else "Searching..."
        track_color = config.COLOR_SUCCESS if raw_count is not None else config.COLOR_WARNING
        cv2.putText(frame, f"Status: {tracking_status}", (400, 40), self.font, self.font_scale, track_color, self.thickness)
        
        # --- Bottom Panel ---
        # Arduino Status
        arduino_status = "Connected" if serial_connected else "Disconnected"
        ard_color = config.COLOR_SUCCESS if serial_connected else config.COLOR_ERROR
        port_str = f"({port_name})" if port_name else ""
        
        cv2.putText(frame, f"Arduino: {arduino_status} {port_str}", (20, h - 15), self.font, 0.6, ard_color, 1)
        
        # Exit instruction
        cv2.putText(frame, "Press ESC to Exit", (w - 200, h - 15), self.font, 0.6, config.COLOR_TEXT, 1)

    def show(self, window_name, frame):
        """
        Displays the frame in a resizable window.
        """
        cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
        cv2.imshow(window_name, frame)
