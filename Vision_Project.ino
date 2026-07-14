#include "esp_camera.h"
#include <Wire.h>
#include "camera_pins.h"

void setup() {
  // Ultra-high baud rate to push 76,800 bytes per frame
  Serial.begin(1000000);
  
  // IMPORTANT: Turn off debug output so system logs don't corrupt the Python stream
  Serial.setDebugOutput(false); 
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  // 24MHz master clock: Overclocking to the datasheet limit
  config.xclk_freq_hz = 24000000; 
  
  // Grayscale and QVGA to bypass driver bugs and fit in internal RAM
  config.pixel_format = PIXFORMAT_GRAYSCALE; 
  config.frame_size = FRAMESIZE_QVGA; // 320x240
  
  // Memory allocation settings (Forced to internal RAM)
  config.jpeg_quality = 12; 
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_DRAM; 

  // Initialize the camera hardware
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return; // Removed print so it doesn't break serial sync
  }
  
  // --- MANUALLY HALVE THE FRAME RATE ---
  Wire.begin(SIOD_GPIO_NUM, SIOC_GPIO_NUM); // Start I2C ONLY ONCE
  Wire.beginTransmission(0x21);             // Default OV7670 I2C Address
  
  Wire.write(0x11);                         // CLKRC Register (Internal Clock)
  Wire.write(0x01);                         // 0x01 divides the clock by 2. (Use 0x03 to divide by 4)
  
  Wire.endTransmission();
}

void loop() {
  // Grab the frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    return; // Silently retry if a frame drops
  }

  // 1. Send the complex 8-byte synchronization header to prevent motion crashes
  const uint8_t header[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x34, 0x56, 0x78};
  Serial.write(header, 8);
  
  // 2. Dump the raw 76,800 Grayscale bytes directly to Python
  Serial.write(fb->buf, fb->len);
  
  // 3. Return the buffer to the DMA hardware to catch the next frame
  esp_camera_fb_return(fb);
}