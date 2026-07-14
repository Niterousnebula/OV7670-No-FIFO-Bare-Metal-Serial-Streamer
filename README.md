#OV7670 (No FIFO) Bare-Metal Serial Streamer

A stable, raw video streaming pipeline for the notorious non-FIFO OV7670 camera module and the ESP32. Bypasses the standard Wi-Fi/JPEG driver crashes and streams directly to a Python OpenCV window over USB.

## The Problem

If you have tried connecting an OV7670 without a hardware FIFO buffer directly to an ESP32 using jumper wires, you have likely hit the dreaded `cam_hal: FB-SIZE` error. 

Because of the parasitic capacitance of unshielded breadboard wires, the ESP32's DMA hardware misses pixel clock edges. The camera's `VSYNC` signal fires and closes the frame window before the ESP32 can catch all 76,800 bytes, resulting in a dropped frame and a crashed stream. Furthermore, the ESP32's highly fragmented internal RAM often causes `malloc` crashes when attempting to allocate memory for standard software JPEG compression.

## The Solution

This repository bypasses the hardware and memory limitations using three techniques:

1. **The I2C Clock Divider Bypass:** After the ESP32 camera driver initializes, the Arduino sketch injects `0x01` into the OV7670's `CLKRC` register (`0x11`). This manually halves the camera's internal pixel clock, giving the ESP32's DMA bucket twice as much time to catch the bytes before `VSYNC` fires.
2. **Raw Serial Dumping:** Bypasses the ESP32's internal JPEG compressor entirely. This code dumps the raw grayscale bytes directly over the serial cable at `1,000,000` baud, keeping the memory footprint small and stable.
3. **Motion-Blur Header Injection:** Uses a complex 8-byte synchronization header (`0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x34, 0x56, 0x78`) to prevent the Python script from crashing when fast camera panning creates artificial pixel gradients that accidentally mimic standard, shorter headers.

---

## Getting Started

### 1. Arduino Setup

1. Wire your OV7670 to the ESP32 (Ensure your pinout matches `camera_pins.h`).
2. Open `Vision_Project.ino` in the Arduino IDE.
3. Upload the sketch to your ESP32.
4. **CRITICAL:** Close the Arduino IDE Serial Monitor completely to free up the COM port.

### 2. Python Setup

1. Install the required Python libraries:
   ```bash
   pip install pyserial numpy opencv-python

2. run the script 
