import time
import config
import math

class FingerCounter:
    """
    Logic for counting fingers based on MediaPipe landmarks.
    Includes a stability state machine to avoid flickering.
    """
    def __init__(self):
        # Tip IDs for Thumb, Index, Middle, Ring, Pinky
        self.tip_ids = [4, 8, 12, 16, 20]
        
        # State machine variables
        self.current_stable_count = None
        self.candidate_count = None
        self.candidate_start_time = 0

    def dist(self, lm1, lm2):
        return math.hypot(lm1.x - lm2.x, lm1.y - lm2.y)

    def count_fingers(self, hand_landmarks):
        """
        Counts the number of extended fingers given a single hand's landmarks.
        Returns the number of fingers (0-5).
        """
        # Tasks API returns a simple list of landmarks (each has .x, .y, .z)
        # instead of a .landmark property.
        
        thumb_tip = hand_landmarks[4]
        thumb_ip = hand_landmarks[3]
        pinky_mcp = hand_landmarks[17]
        
        # Thumb check: compare distance from tip to pinky base vs ip to pinky base.
        # When the thumb is extended, it moves away from the pinky base.
        # When folded across the palm, it moves closer to the pinky base.
        thumb_extended = self.dist(thumb_tip, pinky_mcp) > self.dist(thumb_ip, pinky_mcp)
        
        fingers_extended = 0
        if thumb_extended: fingers_extended += 1
        
        wrist = hand_landmarks[0]
        
        # Fingers: compare distance from tip to wrist vs pip to wrist
        # This is more robust against hand rotation than checking Y coordinates.
        for tip, pip in [(8,6), (12,10), (16,14), (20,18)]:
            if self.dist(hand_landmarks[tip], wrist) > self.dist(hand_landmarks[pip], wrist):
                fingers_extended += 1
                
        return min(fingers_extended, 5)

    def get_stable_count(self, raw_count):
        """
        Requires the raw count to remain identical for config.STABILITY_DELAY_SEC
        before accepting it as the stable count.
        Returns the stable count, or None if no hand detected/unstable.
        """
        if raw_count is None:
            pass

        now = time.time()
        
        if self.candidate_count != raw_count:
            # The count changed, reset the candidate and timer
            self.candidate_count = raw_count
            self.candidate_start_time = now
        else:
            # The count has been consistent
            if (now - self.candidate_start_time) >= config.STABILITY_DELAY_SEC:
                self.current_stable_count = self.candidate_count
                
        return self.current_stable_count
