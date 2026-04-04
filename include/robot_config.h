#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#include <Arduino.h>

// ============================================
// ESP32S3-N16R8 Pin Configuration for HRDW Microbot
// ============================================

// --- Camera OV5640 (DFRobot AI CAM) ---
#define CAMERA_PIN_PWDN     -1
#define CAMERA_PIN_RESET    45
#define CAMERA_PIN_XCLK     47
#define CAMERA_PIN_SIOD     41
#define CAMERA_PIN_SIOC     40
#define CAMERA_PIN_D7       39
#define CAMERA_PIN_D6       38
#define CAMERA_PIN_D5       37
#define CAMERA_PIN_D4       36
#define CAMERA_PIN_D3       35
#define CAMERA_PIN_D2       34
#define CAMERA_PIN_D1       33
#define CAMERA_PIN_D0       32
#define CAMERA_PIN_VSYNC    46
#define CAMERA_PIN_HREF     48
#define CAMERA_PIN_PCLK     49

// --- Battery Monitor LC709203F (I2C) ---
#define BATTERY_MONITOR_I2C_ADDR  0x0B

// --- ADC ADS1015 for Photoresistors (I2C) ---
#define PHOTO_ADC_I2C_ADDR        0x48
#define PHOTO_ADC_GAIN            GAIN_TWOTHIRDS

// --- Line Sensors GP25700 (4 sensors) ---
#define LINE_SENSOR_1_PIN         1
#define LINE_SENSOR_2_PIN         2
#define LINE_SENSOR_3_PIN         3
#define LINE_SENSOR_4_PIN         4

// --- Motor Controller DRV8834 ---
#define MOTOR_LEFT_PWM            5
#define MOTOR_LEFT_IN1            6
#define MOTOR_LEFT_IN2            7
#define MOTOR_RIGHT_PWM           8
#define MOTOR_RIGHT_IN1           9
#define MOTOR_RIGHT_IN2           10

// --- Motor Encoders OH137 ---
#define ENCODER_LEFT_A            11
#define ENCODER_LEFT_B            12
#define ENCODER_RIGHT_A           13
#define ENCODER_RIGHT_B           14

// --- TOF Laser Rangefinder VL53L0X (I2C) ---
#define TOF_I2C_ADDR              0x29
#define TOF_XSHUT_PIN             15

// --- Digital Light Sensor LTR-308 (I2C) ---
#define LIGHT_SENSOR_I2C_ADDR     0x53

// --- Addressable LEDs SK6812 (4 LEDs) ---
#define LED_DATA_PIN              16
#define LED_COUNT                 4

// --- Buzzer ---
#define BUZZER_PIN                17

// --- I2C Configuration ---
#define I2C_SDA_PIN               18
#define I2C_SCL_PIN               19
#define I2C_FREQUENCY             400000

#endif
