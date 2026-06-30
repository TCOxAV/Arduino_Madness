import cv2
import config

class CameraManager:
    """
    Handles the webcam initialization and frame reading.
    """
    def __init__(self):
        self.cap = cv2.VideoCapture(config.CAMERA_INDEX)
        
        # Request specific resolution and framerate
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, config.CAMERA_WIDTH)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, config.CAMERA_HEIGHT)
        self.cap.set(cv2.CAP_PROP_FPS, config.TARGET_FPS)

        self.actual_width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        self.actual_height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        
    def is_opened(self):
        return self.cap.isOpened()

    def read_frame(self):
        """
        Reads a frame from the camera.
        Returns (success, frame)
        """
        success, frame = self.cap.read()
        if success:
            # Flip horizontally for a mirror effect (more intuitive for hands)
            frame = cv2.flip(frame, 1)
        return success, frame

    def release(self):
        """
        Releases the camera resources.
        """
        if self.cap.isOpened():
            self.cap.release()
