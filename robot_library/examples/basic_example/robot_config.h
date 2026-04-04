#define CAMERA_PIN_D5       17
#define CAMERA_PIN_D4       21
#define CAMERA_PIN_D3       18
#define CAMERA_PIN_D2       16
#define CAMERA_PIN_XCLK     5

// Battery Monitor LC709203F
#define BATTERY_MONITOR_I2C_SDA   8
#define BATTERY_MONITOR_I2C_SCL   9
#define BATTERY_MONITOR_I2C_ADDR  0x0B

// ADC ADS1015 для фоторезисторов
#define ADS1015_I2C_SDA     8
#define ADS1015_I2C_SCL     9
#define ADS1015_I2C_ADDR    0x4b

// Датчики линии GP25700 (4 шт)
#define LINE_SENSOR_1_PIN   41
#define LINE_SENSOR_2_PIN   40
#define LINE_SENSOR_3_PIN   39
#define LINE_SENSOR_4_PIN   38

// Контроллер двигателей DRV8834 с энкодерами OH137
#define MOTOR_LEFT_PWM      13
#define MOTOR_LEFT_DIR      12
#define MOTOR_RIGHT_PWM     11
#define MOTOR_RIGHT_DIR     10

// Энкодеры
#define ENCODER_LEFT_A      47
#define ENCODER_LEFT_B      48
#define ENCODER_RIGHT_A     43
#define ENCODER_RIGHT_B     42

// TOF дальномер VL53L0X
#define TOF_I2C_SDA         8
#define TOF_I2C_SCL         9
#define TOF_I2C_ADDR        0x52

// Цифровой датчик освещенности LTR-308
#define LTR308_I2C_SDA      8
#define LTR308_I2C_SCL      9
#define LTR308_I2C_INT      44
#define LTR308_I2C_ADDR     0x53

// Адресные светодиоды SK6812
#define LED_STRIP_PIN       3
#define LED_STRIP_COUNT     9

// Buzzer
#define BUZZER_PIN          45

//Кнопка Start
#define START_btn_PIN       46

// I2C общие настройки
#define I2C_FREQUENCY       400000

#endif // ROBOT_CONFIG_H
