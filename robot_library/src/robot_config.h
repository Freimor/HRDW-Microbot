#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

// ============================================
// Конфигурация пинов ESP32S3-N16R8
// ============================================

// Камера OV5640 (DFRobot AI CAM)
#define CAMERA_PIN_PCLK     40
#define CAMERA_PIN_VSYNC    41
#define CAMERA_PIN_HREF     39
#define CAMERA_PIN_SIOD     18  // SDA
#define CAMERA_PIN_SIOC     38  // SCL
#define CAMERA_PIN_D7       48
#define CAMERA_PIN_D6       47
#define CAMERA_PIN_D5       1
#define CAMERA_PIN_D4       2
#define CAMERA_PIN_D3       42
#define CAMERA_PIN_D2       43
#define CAMERA_PIN_D1       44
#define CAMERA_PIN_D0       45
#define CAMERA_PIN_XCLK     0

// Battery Monitor LC709203F
#define BATTERY_MONITOR_I2C_SDA   18
#define BATTERY_MONITOR_I2C_SCL   38
#define BATTERY_MONITOR_I2C_ADDR  0x0B

// ADC ADS1015 для фоторезисторов
#define ADS1015_I2C_SDA     18
#define ADS1015_I2C_SCL     38
#define ADS1015_I2C_ADDR    0x48

// Датчики линии GP25700 (4 шт)
#define LINE_SENSOR_1_PIN   3
#define LINE_SENSOR_2_PIN   46
#define LINE_SENSOR_3_PIN   5
#define LINE_SENSOR_4_PIN   6

// Контроллер двигателей DRV8834 с энкодерами OH137
#define MOTOR_LEFT_PWM      7
#define MOTOR_LEFT_IN1      8
#define MOTOR_LEFT_IN2      9
#define MOTOR_RIGHT_PWM     10
#define MOTOR_RIGHT_IN1     11
#define MOTOR_RIGHT_IN2     12

// Энкодеры
#define ENCODER_LEFT_A      13
#define ENCODER_LEFT_B      14
#define ENCODER_RIGHT_A     15
#define ENCODER_RIGHT_B     16

// TOF дальномер VL53L0X
#define TOF_I2C_SDA         18
#define TOF_I2C_SCL         38
#define TOF_I2C_ADDR        0x29
#define TOF_XSHUT_PIN       17

// Цифровой датчик освещенности LTR-308
#define LTR308_I2C_SDA      18
#define LTR308_I2C_SCL      38
#define LTR308_I2C_ADDR     0x53

// Адресные светодиоды SK6812
#define LED_STRIP_PIN       21
#define LED_STRIP_COUNT     4

// Buzzer
#define BUZZER_PIN          22

// I2C общие настройки
#define I2C_FREQUENCY       400000

#endif // ROBOT_CONFIG_H
