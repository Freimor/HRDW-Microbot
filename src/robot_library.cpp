#include "robot_library.h"
#include <Wire.h>

// Глобальный экземпляр
RobotLibrary robot;

// ============================================
// Конструктор и деструктор
// ============================================

RobotLibrary::RobotLibrary() 
    : _initialized(false)
    , _cameraReady(false)
    , _batteryReady(false)
    , _photoResistorsReady(false)
    , _lineSensorsReady(false)
    , _motorsReady(false)
    , _tofReady(false)
    , _lightSensorReady(false)
    , _ledsReady(false)
    , _buzzerReady(false)
    , _leftEncoderCount(0)
    , _rightEncoderCount(0)
    , _i2c(nullptr)
{
}

// ============================================
// Инициализация всех компонентов
// ============================================

bool RobotLibrary::begin() {
    if (_initialized) {
        return true;
    }
    
    // Инициализация Serial для отладки
    Serial.begin(115200);
    delay(1000);
    Serial.println("=== Robot Library Initializing ===");
    
    // Настройка I2C
    _setupI2C();
    
    // Инициализация компонентов по порядку
    _batteryReady = batteryInit();
    Serial.print("Battery Monitor: ");
    Serial.println(_batteryReady ? "OK" : "FAILED");
    
    _photoResistorsReady = photoResistorsInit();
    Serial.print("Photo Resistors (ADS1015): ");
    Serial.println(_photoResistorsReady ? "OK" : "FAILED");
    
    _lineSensorsReady = lineSensorsInit();
    Serial.print("Line Sensors: ");
    Serial.println(_lineSensorsReady ? "OK" : "FAILED");
    
    _motorsReady = motorsInit();
    Serial.print("Motors & Encoders: ");
    Serial.println(_motorsReady ? "OK" : "FAILED");
    
    _tofReady = tofInit();
    Serial.print("TOF Sensor (VL53L0X): ");
    Serial.println(_tofReady ? "OK" : "FAILED");
    
    _lightSensorReady = lightSensorInit();
    Serial.print("Light Sensor (LTR-308): ");
    Serial.println(_lightSensorReady ? "OK" : "FAILED");
    
    _ledsReady = ledsInit();
    Serial.print("LEDs (SK6812): ");
    Serial.println(_ledsReady ? "OK" : "FAILED");
    
    _buzzerReady = buzzerInit();
    Serial.print("Buzzer: ");
    Serial.println(_buzzerReady ? "OK" : "FAILED");
    
    _cameraReady = cameraInit();
    Serial.print("Camera (OV5640): ");
    Serial.println(_cameraReady ? "OK" : "FAILED");
    
    _initialized = true;
    Serial.println("=== Initialization Complete ===");
    
    return true;
}

// ============================================
// Внутренние функции
// ============================================

void RobotLibrary::_setupI2C() {
    Wire.begin(BATTERY_MONITOR_I2C_SDA, BATTERY_MONITOR_I2C_SCL, I2C_FREQUENCY);
    _i2c = &Wire;
}

void RobotLibrary::_setupEncoderInterrupts() {
    pinMode(ENCODER_LEFT_A, INPUT_PULLUP);
    pinMode(ENCODER_RIGHT_A, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_A), _leftEncoderISR, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_A), _rightEncoderISR, RISING);
}

void IRAM_ATTR RobotLibrary::_leftEncoderISR() {
    if (digitalRead(ENCODER_LEFT_B) == HIGH) {
        robot._leftEncoderCount++;
    } else {
        robot._leftEncoderCount--;
    }
}

void IRAM_ATTR RobotLibrary::_rightEncoderISR() {
    if (digitalRead(ENCODER_RIGHT_B) == HIGH) {
        robot._rightEncoderCount++;
    } else {
        robot._rightEncoderCount--;
    }
}

// ============================================
// Камера OV5640
// ============================================

bool RobotLibrary::cameraInit() {
    // Примечание: Для работы камеры требуется библиотека ESP32 Camera
    // Здесь приведена базовая структура инициализации
    
    #ifdef USE_ESP_CAMERA
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAMERA_PIN_D0;
    config.pin_d1 = CAMERA_PIN_D1;
    config.pin_d2 = CAMERA_PIN_D2;
    config.pin_d3 = CAMERA_PIN_D3;
    config.pin_d4 = CAMERA_PIN_D4;
    config.pin_d5 = CAMERA_PIN_D5;
    config.pin_d6 = CAMERA_PIN_D6;
    config.pin_d7 = CAMERA_PIN_D7;
    config.pin_xclk = CAMERA_PIN_XCLK;
    config.pin_pclk = CAMERA_PIN_PCLK;
    config.pin_vsync = CAMERA_PIN_VSYNC;
    config.pin_href = CAMERA_PIN_HREF;
    config.pin_sscb_sda = CAMERA_PIN_SIOD;
    config.pin_sscb_scl = CAMERA_PIN_SIOC;
    config.pixformat = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
    
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return false;
    }
    #endif
    
    return true;
}

bool RobotLibrary::cameraCapture() {
    #ifdef USE_ESP_CAMERA
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        return false;
    }
    // Обработка кадра...
    esp_camera_fb_return(fb);
    return true;
    #else
    return false;
    #endif
}

void RobotLibrary::cameraDeinit() {
    #ifdef USE_ESP_CAMERA
    esp_camera_deinit();
    #endif
    _cameraReady = false;
}

// ============================================
// Battery Monitor LC709203F
// ============================================

bool RobotLibrary::batteryInit() {
    Wire.beginTransmission(BATTERY_MONITOR_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    return true;
}

BatteryData RobotLibrary::getBatteryData() {
    BatteryData data = {0.0f, 0.0f, false, false};
    
    if (!_batteryReady) return data;
    
    Wire.beginTransmission(BATTERY_MONITOR_I2C_ADDR);
    Wire.write(0x0A); // Регистр напряжения
    if (Wire.endTransmission(false) == 0) {
        Wire.requestFrom(BATTERY_MONITOR_I2C_ADDR, (uint8_t)2);
        if (Wire.available() >= 2) {
            uint16_t voltage_raw = Wire.read() | (Wire.read() << 8);
            data.battery_voltage = (float)voltage_raw * 0.001f; // мВ -> В
            data.valid = true;
        }
    }
    
    Wire.beginTransmission(BATTERY_MONITOR_I2C_ADDR);
    Wire.write(0x0D); // Регистр SOC (State of Charge)
    if (Wire.endTransmission(false) == 0) {
        Wire.requestFrom(BATTERY_MONITOR_I2C_ADDR, (uint8_t)1);
        if (Wire.available() >= 1) {
            data.battery_percentage = (float)Wire.read();
        }
    }
    
    // Проверка статуса заряда
    Wire.beginTransmission(BATTERY_MONITOR_I2C_ADDR);
    Wire.write(0x00); // Регистр статуса
    if (Wire.endTransmission(false) == 0) {
        Wire.requestFrom(BATTERY_MONITOR_I2C_ADDR, (uint8_t)2);
        if (Wire.available() >= 2) {
            uint16_t status = Wire.read() | (Wire.read() << 8);
            data.charging = (status & 0x0002) != 0;
        }
    }
    
    return data;
}

float RobotLibrary::getBatteryVoltage() {
    BatteryData data = getBatteryData();
    return data.battery_voltage;
}

float RobotLibrary::getBatteryPercentage() {
    BatteryData data = getBatteryData();
    return data.battery_percentage;
}

bool RobotLibrary::isCharging() {
    BatteryData data = getBatteryData();
    return data.charging;
}

// ============================================
// Фоторезисторы через ADS1015
// ============================================

bool RobotLibrary::photoResistorsInit() {
    Wire.beginTransmission(ADS1015_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    // Настройка ADS1015: одиночные измерения, gain=2/3V
    Wire.beginTransmission(ADS1015_I2C_ADDR);
    Wire.write(0x01); // Регистр конфигурации
    Wire.write(0xC2); // MSB: Single-shot, AIN0, Gain=2/3V
    Wire.write(0x83); // LSB: 128SPS
    return Wire.endTransmission() == 0;
}

PhotoResistorData RobotLibrary::getPhotoResistorData() {
    PhotoResistorData data = {0, 0, 0, 0};
    
    if (!_photoResistorsReady) return data;
    
    for (uint8_t ch = 0; ch < 4; ch++) {
        data.ch0 + ch = getPhotoResistorValue(ch);
    }
    
    return data;
}

uint16_t RobotLibrary::getPhotoResistorValue(uint8_t channel) {
    if (!_photoResistorsReady || channel > 3) return 0;
    
    // Настройка канала для измерения
    Wire.beginTransmission(ADS1015_I2C_ADDR);
    Wire.write(0x01); // Регистр конфигурации
    
    uint8_t config_msb = 0xC0 | ((channel & 0x03) << 4);
    Wire.write(config_msb);
    Wire.write(0x83);
    Wire.endTransmission();
    
    // Ожидание завершения преобразования
    delay(10);
    
    // Чтение регистра преобразования
    Wire.beginTransmission(ADS1015_I2C_ADDR);
    Wire.write(0x00); // Регистр преобразования
    Wire.endTransmission(false);
    
    Wire.requestFrom(ADS1015_I2C_ADDR, (uint8_t)2);
    if (Wire.available() >= 2) {
        uint16_t value = (Wire.read() << 4) | (Wire.read() >> 4);
        return value;
    }
    
    return 0;
}

// ============================================
// Датчики линии GP25700
// ============================================

bool RobotLibrary::lineSensorsInit() {
    pinMode(LINE_SENSOR_1_PIN, INPUT);
    pinMode(LINE_SENSOR_2_PIN, INPUT);
    pinMode(LINE_SENSOR_3_PIN, INPUT);
    pinMode(LINE_SENSOR_4_PIN, INPUT);
    return true;
}

LineSensorData RobotLibrary::getLineSensorData() {
    LineSensorData data;
    data.sensor1 = digitalRead(LINE_SENSOR_1_PIN);
    data.sensor2 = digitalRead(LINE_SENSOR_2_PIN);
    data.sensor3 = digitalRead(LINE_SENSOR_3_PIN);
    data.sensor4 = digitalRead(LINE_SENSOR_4_PIN);
    return data;
}

bool RobotLibrary::getLineSensorValue(uint8_t sensorNum) {
    if (sensorNum < 1 || sensorNum > 4) return false;
    
    switch (sensorNum) {
        case 1: return digitalRead(LINE_SENSOR_1_PIN);
        case 2: return digitalRead(LINE_SENSOR_2_PIN);
        case 3: return digitalRead(LINE_SENSOR_3_PIN);
        case 4: return digitalRead(LINE_SENSOR_4_PIN);
        default: return false;
    }
}

uint8_t RobotLibrary::getLinePosition() {
    bool s1 = digitalRead(LINE_SENSOR_1_PIN);
    bool s2 = digitalRead(LINE_SENSOR_2_PIN);
    bool s3 = digitalRead(LINE_SENSOR_3_PIN);
    bool s4 = digitalRead(LINE_SENSOR_4_PIN);
    
    if (s1) return 0;
    if (s2) return 1;
    if (s3) return 2;
    if (s4) return 3;
    
    return 255; // Нет линии
}

// ============================================
// Контроллер двигателей DRV8834 с энкодерами
// ============================================

bool RobotLibrary::motorsInit() {
    // Настройка пинов управления двигателями
    pinMode(MOTOR_LEFT_PWM, OUTPUT);
    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);
    pinMode(MOTOR_RIGHT_PWM, OUTPUT);
    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);
    
    // Настройка PWM
    ledcAttach(MOTOR_LEFT_PWM, 20000, 8);
    ledcAttach(MOTOR_RIGHT_PWM, 20000, 8);
    
    // Остановка двигателей при старте
    stopMotors();
    
    // Настройка энкодеров
    _setupEncoderInterrupts();
    
    return true;
}

void RobotLibrary::setMotorSpeed(int16_t leftSpeed, int16_t rightSpeed) {
    setLeftMotorSpeed(leftSpeed);
    setRightMotorSpeed(rightSpeed);
}

void RobotLibrary::setLeftMotorSpeed(int16_t speed) {
    // Ограничение скорости -255 до 255
    speed = constrain(speed, -255, 255);
    
    if (speed >= 0) {
        digitalWrite(MOTOR_LEFT_IN1, HIGH);
        digitalWrite(MOTOR_LEFT_IN2, LOW);
        ledcWrite(MOTOR_LEFT_PWM, speed);
    } else {
        digitalWrite(MOTOR_LEFT_IN1, LOW);
        digitalWrite(MOTOR_LEFT_IN2, HIGH);
        ledcWrite(MOTOR_LEFT_PWM, -speed);
    }
}

void RobotLibrary::setRightMotorSpeed(int16_t speed) {
    // Ограничение скорости -255 до 255
    speed = constrain(speed, -255, 255);
    
    if (speed >= 0) {
        digitalWrite(MOTOR_RIGHT_IN1, HIGH);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
        ledcWrite(MOTOR_RIGHT_PWM, speed);
    } else {
        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        digitalWrite(MOTOR_RIGHT_IN2, HIGH);
        ledcWrite(MOTOR_RIGHT_PWM, -speed);
    }
}

void RobotLibrary::stopMotors() {
    ledcWrite(MOTOR_LEFT_PWM, 0);
    ledcWrite(MOTOR_RIGHT_PWM, 0);
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
    digitalWrite(MOTOR_RIGHT_IN1, LOW);
    digitalWrite(MOTOR_RIGHT_IN2, LOW);
}

EncoderData RobotLibrary::getEncoderData() {
    EncoderData data;
    data.left = _leftEncoderCount;
    data.right = _rightEncoderCount;
    return data;
}

int32_t RobotLibrary::getLeftEncoderCount() {
    return _leftEncoderCount;
}

int32_t RobotLibrary::getRightEncoderCount() {
    return _rightEncoderCount;
}

void RobotLibrary::resetEncoders() {
    _leftEncoderCount = 0;
    _rightEncoderCount = 0;
}

// ============================================
// TOF дальномер VL53L0X
// ============================================

bool RobotLibrary::tofInit() {
    // Сброс датчика через XSHUT
    pinMode(TOF_XSHUT_PIN, OUTPUT);
    digitalWrite(TOF_XSHUT_PIN, LOW);
    delay(10);
    digitalWrite(TOF_XSHUT_PIN, HIGH);
    delay(10);
    
    Wire.beginTransmission(TOF_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    // Инициализация VL53L0X требует специфической последовательности
    // Рекомендуется использовать библиотеку Pololu VL53L0X
    
    return true;
}

TOFData RobotLibrary::getTOFData() {
    TOFData data = {0.0f, false};
    
    if (!_tofReady) return data;
    
    Wire.beginTransmission(TOF_I2C_ADDR);
    Wire.write(0x0E); // Регистр результата
    if (Wire.endTransmission(false) == 0) {
        Wire.requestFrom(TOF_I2C_ADDR, (uint8_t)2);
        if (Wire.available() >= 2) {
            uint16_t distance = (Wire.read() << 8) | Wire.read();
            data.distance_mm = (float)distance;
            data.valid = (distance < 8190); // 8190 = out of range
        }
    }
    
    return data;
}

uint16_t RobotLibrary::getDistanceMM() {
    TOFData data = getTOFData();
    return (uint16_t)data.distance_mm;
}

bool RobotLibrary::isDistanceValid() {
    TOFData data = getTOFData();
    return data.valid;
}

// ============================================
// Цифровой датчик освещенности LTR-308
// ============================================

bool RobotLibrary::lightSensorInit() {
    Wire.beginTransmission(LTR308_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    // Активация датчика
    Wire.beginTransmission(LTR308_I2C_ADDR);
    Wire.write(0x0A); // Регистр управления
    Wire.write(0x03); // Включить
    Wire.endTransmission();
    
    return true;
}

LightSensorData RobotLibrary::getLightSensorData() {
    LightSensorData data = {0.0f, false};
    
    if (!_lightSensorReady) return data;
    
    Wire.beginTransmission(LTR308_I2C_ADDR);
    Wire.write(0x0A); // Регистр статуса
    if (Wire.endTransmission(false) == 0) {
        Wire.requestFrom(LTR308_I2C_ADDR, (uint8_t)1);
        if (Wire.available() >= 1 && (Wire.read() & 0x80)) {
            // Данные готовы
            Wire.beginTransmission(LTR308_I2C_ADDR);
            Wire.write(0x00); // ALS_DATA_0
            Wire.endTransmission(false);
            Wire.requestFrom(LTR308_I2C_ADDR, (uint8_t)4);
            
            if (Wire.available() >= 4) {
                uint16_t als_low = Wire.read() | (Wire.read() << 8);
                uint16_t als_high = Wire.read() | (Wire.read() << 8);
                
                // Конвертация в Lux (зависит от настроек增益 и времени интеграции)
                float lux = (float)(als_low + als_high) * 0.5f; // Упрощенная формула
                data.lux = lux;
                data.valid = true;
            }
        }
    }
    
    return data;
}

float RobotLibrary::getLux() {
    LightSensorData data = getLightSensorData();
    return data.lux;
}

bool RobotLibrary::isLightValid() {
    LightSensorData data = getLightSensorData();
    return data.valid;
}

// ============================================
// Адресные светодиоды SK6812
// ============================================

// Массив для хранения цветов светодиодов
static uint8_t led_buffer[LED_STRIP_COUNT * 4]; // GRBA

bool RobotLibrary::ledsInit() {
    pinMode(LED_STRIP_PIN, OUTPUT);
    digitalWrite(LED_STRIP_PIN, LOW);
    
    // Инициализация буфера
    clearLeds();
    showLeds();
    
    return true;
}

void RobotLibrary::setLedColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= LED_STRIP_COUNT) return;
    
    uint8_t offset = index * 4;
    led_buffer[offset + 0] = g; // GRB порядок для SK6812
    led_buffer[offset + 1] = r;
    led_buffer[offset + 2] = b;
    led_buffer[offset + 3] = 0; // Alpha
}

void RobotLibrary::setAllLedsColor(uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = 0; i < LED_STRIP_COUNT; i++) {
        setLedColor(i, r, g, b);
    }
}

void RobotLibrary::clearLeds() {
    memset(led_buffer, 0, sizeof(led_buffer));
}

void RobotLibrary::showLeds() {
    // Простая реализация через bit-banging
    // Для production рекомендуется использовать RMT или I2S
    
    noInterrupts();
    
    for (uint16_t i = 0; i < sizeof(led_buffer); i++) {
        uint8_t byte = led_buffer[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (byte & (1 << (7 - bit))) {
                // Логическая 1: T1H=0.6us, T1L=0.6us
                digitalWrite(LED_STRIP_PIN, HIGH);
                __asm__ __volatile__("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
                digitalWrite(LED_STRIP_PIN, LOW);
                __asm__ __volatile__("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
            } else {
                // Логический 0: T0H=0.3us, T0L=0.9us
                digitalWrite(LED_STRIP_PIN, HIGH);
                __asm__ __volatile__("nop\n nop\n nop\n");
                digitalWrite(LED_STRIP_PIN, LOW);
                __asm__ __volatile__("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
            }
        }
    }
    
    interrupts();
    delayMicroseconds(50); // Reset time
}

void RobotLibrary::setLedBrightness(uint8_t brightness) {
    // Применяет яркость ко всем светодиодам
    for (uint16_t i = 0; i < sizeof(led_buffer); i += 4) {
        led_buffer[i] = (uint16_t)led_buffer[i] * brightness / 255;
        led_buffer[i + 1] = (uint16_t)led_buffer[i + 1] * brightness / 255;
        led_buffer[i + 2] = (uint16_t)led_buffer[i + 2] * brightness / 255;
    }
}

// ============================================
// Buzzer
// ============================================

bool RobotLibrary::buzzerInit() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    
    // Настройка PWM для генерации тона
    ledcAttach(BUZZER_PIN, 20000, 8);
    ledcWrite(BUZZER_PIN, 0);
    
    return true;
}

void RobotLibrary::beep(uint16_t frequency, uint16_t duration_ms) {
    playTone(frequency, duration_ms);
    delay(duration_ms);
    stopBuzzer();
}

void RobotLibrary::playTone(uint16_t frequency, uint16_t duration_ms) {
    if (frequency == 0) {
        stopBuzzer();
        return;
    }
    
    ledcDetach(BUZZER_PIN);
    ledcAttach(BUZZER_PIN, frequency, 8);
    ledcWrite(BUZZER_PIN, 128); // 50% duty cycle
}

void RobotLibrary::stopBuzzer() {
    ledcWrite(BUZZER_PIN, 0);
}

// ============================================
// Утилиты
// ============================================

void RobotLibrary::delayMs(uint32_t ms) {
    delay(ms);
}

uint32_t RobotLibrary::getMillis() {
    return millis();
}

void RobotLibrary::printDebug(const char* message) {
    Serial.println(message);
}
