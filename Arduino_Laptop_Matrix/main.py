import cv2
import time
from camera import CameraManager
from hand_recognizer import HandRecognizer
from finger_counter import FingerCounter
from serial_manager import SerialManager
from ui import UIManager

def main():
    print("Initializing HandMatrix...")
    
    camera = CameraManager()
    recognizer = HandRecognizer()
    counter = FingerCounter()
    serial_mgr = SerialManager()
    ui = UIManager()
    
    if not camera.is_opened():
        print("Error: Could not open camera.")
        return

    pTime = 0
    
    print("HandMatrix is running. Press ESC to exit.")
    
    start_time = time.time()
    
    while True:
        success, frame = camera.read_frame()
        if not success:
            print("Failed to read frame.")
            break
            
        timestamp_ms = int((time.time() - start_time) * 1000)
        
        # Process the frame to find hands
        results = recognizer.process_frame(frame, timestamp_ms)
        
        raw_count = None
        
        if results.hand_landmarks:
            raw_count = 0
            for hand_landmarks in results.hand_landmarks:
                # Draw the skeleton
                ui.draw_skeleton(frame, hand_landmarks)
                
                # Count the fingers and add to total
                raw_count += counter.count_fingers(hand_landmarks)

        # Get stable count (applies the 300ms filter)
        stable_count = counter.get_stable_count(raw_count)
        
        # Send to Arduino
        serial_mgr.send_value(stable_count)
        
        # Calculate FPS
        cTime = time.time()
        fps = 1 / (cTime - pTime) if pTime != 0 else 0
        pTime = cTime
        
        # Draw the modern UI overlay
        ui.draw_overlay(
            frame, 
            fps=fps, 
            raw_count=raw_count, 
            stable_count=stable_count, 
            serial_connected=serial_mgr.is_connected(),
            port_name=serial_mgr.port_name
        )
        
        # Display the result
        ui.show("HandMatrix", frame)
        
        # Check for ESC key
        if cv2.waitKey(1) & 0xFF == 27:
            break
            
    # Cleanup
    print("Shutting down...")
    camera.release()
    recognizer.close()
    serial_mgr.close()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
