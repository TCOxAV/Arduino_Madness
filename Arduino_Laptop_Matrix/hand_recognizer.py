import cv2
import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision
import config
import os

class HandRecognizer:
    """
    Wrapper around MediaPipe Hands Tasks API to process frames and extract hand landmarks.
    """
    def __init__(self):
        model_path = os.path.join(os.path.dirname(__file__), 'hand_landmarker.task')
        if not os.path.exists(model_path):
            raise FileNotFoundError(f"Model file not found at {model_path}. Please download hand_landmarker.task.")

        base_options = python.BaseOptions(model_asset_path=model_path)
        options = vision.HandLandmarkerOptions(
            base_options=base_options,
            num_hands=config.MAX_HANDS,
            min_hand_detection_confidence=config.DETECTION_CONFIDENCE,
            min_hand_presence_confidence=config.TRACKING_CONFIDENCE,
            min_tracking_confidence=config.TRACKING_CONFIDENCE,
            running_mode=vision.RunningMode.VIDEO
        )
        self.detector = vision.HandLandmarker.create_from_options(options)

    def process_frame(self, frame, timestamp_ms):
        """
        Converts the BGR frame to RGB, wraps it in mp.Image, and processes it.
        Returns the HandLandmarkerResult object.
        """
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=frame_rgb)
        
        # Use detect_for_video which leverages continuous temporal tracking for massive speed/stability gains
        results = self.detector.detect_for_video(mp_image, timestamp_ms)
        
        return results

    def close(self):
        """
        Releases MediaPipe resources.
        """
        self.detector.close()
