#ifndef CAMERA_PINS_H
#define CAMERA_PINS_H

// Pin definition exactly matching your OV7670 to ESP32 DevKit wiring
#define PWDN_GPIO_NUM     4
#define RESET_GPIO_NUM    5
#define XCLK_GPIO_NUM     32
#define SIOD_GPIO_NUM     14 // SDA
#define SIOC_GPIO_NUM     27 // SCL

#define Y9_GPIO_NUM       18 // D7
#define Y8_GPIO_NUM       19 // D6
#define Y7_GPIO_NUM       21 // D5
#define Y6_GPIO_NUM       22 // D4
#define Y5_GPIO_NUM       23 // D3
#define Y4_GPIO_NUM       13 // D2
#define Y3_GPIO_NUM       35 // D1 (Input-only)
#define Y2_GPIO_NUM       34 // D0 (Input-only)

#define VSYNC_GPIO_NUM    25 // VS
#define HREF_GPIO_NUM     26 // HS
#define PCLK_GPIO_NUM     33 // PLK

#endif