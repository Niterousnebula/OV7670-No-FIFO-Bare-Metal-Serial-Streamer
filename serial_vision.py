import serial
import numpy as np
import cv2
import time

# ==========================================
# IMPORTANT: Update this to match your ESP32's COM port!
# ==========================================
SERIAL_PORT = 'COM5'  
BAUD_RATE = 1000000

FRAME_WIDTH = 320
FRAME_HEIGHT = 240
FRAME_SIZE = FRAME_WIDTH * FRAME_HEIGHT # 76800 bytes
HEADER = b'\xde\xad\xbe\xef\x12\x34\x56\x78'

try:
    # Open the serial port
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
    print(f"Listening on {SERIAL_PORT} at {BAUD_RATE} baud...")
except Exception as e:
    print(f"Failed to connect to {SERIAL_PORT}: {e}")
    print("Ensure the Arduino IDE Serial Monitor is CLOSED.")
    exit()

def sync_and_read():
    """Reads the serial buffer until the 8-byte header is found, then grabs the frame."""
    header_buffer = bytearray(8)
    while True:
        # Read one byte at a time
        char = ser.read(1)
        
        # If the serial times out and sends nothing, skip and keep waiting
        if not char:
            continue
            
        # Shift the buffer and append the new byte
        header_buffer[:-1] = header_buffer[1:]
        header_buffer[-1:] = char
        
        if bytes(header_buffer) == HEADER:
            # Sync marker found! Read the exact byte count for one QVGA frame
            frame_data = ser.read(FRAME_SIZE)
            
            if len(frame_data) == FRAME_SIZE:
                return frame_data
            else:
                print(f"Frame dropped. Expected {FRAME_SIZE} bytes, got {len(frame_data)}")
                return None

print("Stream starting. Press 'q' in the video window to quit.")

prev_frame_time = time.time()

while True:
    raw_data = sync_and_read()
    
    if raw_data:
        # Calculate FPS
        new_frame_time = time.time()
        fps = 1 / (new_frame_time - prev_frame_time)
        prev_frame_time = new_frame_time
        
        # 1. Convert the raw bytes into a flat 1D numpy array
        img_array = np.frombuffer(raw_data, dtype=np.uint8)
        
        # 2. Reshape and COPY to make it writable for OpenCV
        img = img_array.reshape((FRAME_HEIGHT, FRAME_WIDTH)).copy()
        
        # 3. Overlay FPS on the frame
        fps_text = f"FPS: {int(fps)}"
        cv2.putText(img, fps_text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255), 2)
        
        # 4. Display it using OpenCV
        cv2.imshow("ESP32-OV7670 Raw Stream", img)
        
    # Wait 1ms for the 'q' key to be pressed to gracefully exit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

ser.close()
cv2.destroyAllWindows()