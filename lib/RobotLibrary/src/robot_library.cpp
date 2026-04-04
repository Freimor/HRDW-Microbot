#include "robot_library.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_VL53L0X.h>
#include <Adafruit_LTR308.h>
#include <FastLED.h>

// Global instance
RobotLibrary robot;

// External library instances
static Adafruit_ADS1015 ads;
static Adafruit_VL53L0X tof;
static Adafruit_LTR308 lightSensor;
static CRGB leds[LED_COUNT];

// Encoder ISR handlers
static volatile long leftEncoderCount = 0;
static volatile long rightEncoderCount = 0;

void IRAM_ATTR leftEncoderISR() {
    leftEncoderCount++;
}

void IRAM_ATTR rightEncoderISR() {
    rightEncoderCount++;
}

RobotLibrary::RobotLibrary() 
    : _i2c(nullptr), _camera(nullptr), _cameraInitialized(false),
      _ads(nullptr), _adsInitialized(false), _tof(nullptr), _tofInitialized(false),
      _lightSensor(nullptr), _lightInitialized(false),
      _leftEncoderCount(0), _rightEncoderCount(0),
      _motorLeftPwm(MOTOR_LEFT_PWM), _motorLeftIn1(MOTOR_LEFT_IN1), _motorLeftIn2(MOTOR_LEFT_IN2),
      _motorRightPwm(MOTOR_RIGHT_PWM), _motorRightIn1(MOTOR_RIGHT_IN1), _motorRightIn2(MOTOR_RIGHT_IN2),
      _buzzerPin(BUZZER_PIN) {
    _linePins[0] = LINE_SENSOR_1_PIN;
    _linePins[1] = LINE_SENSOR_2_PIN;
    _linePins[2] = LINE_SENSOR_3_PIN;
    _linePins[3] = LINE_SENSOR_4_PIN;
    
    for (int i = 0; i < LED_COUNT; i++) {
        _leds[i][0] = _leds[i][1] = _leds[i][2] = 0;
    }
}

bool RobotLibrary::begin() {
    // Initialize I2C
    _i2c = &Wire;
    _i2c->begin(I2C_SDA_PIN, I2C_SCL_PIN);
    _i2c->setClock(I2C_FREQUENCY);
    
    // Initialize motor pins
    pinMode(_motorLeftPwm, OUTPUT);
    pinMode(_motorLeftIn1, OUTPUT);
    pinMode(_motorLeftIn2, OUTPUT);
    pinMode(_motorRightPwm, OUTPUT);
    pinMode(_motorRightIn1, OUTPUT);
    pinMode(_motorRightIn2, OUTPUT);
    
    // Initialize buzzer
    pinMode(_buzzerPin, OUTPUT);
    digitalWrite(_buzzerPin, LOW);
    
    // Initialize line sensor pins
    for (int i = 0; i < 4; i++) {
        pinMode(_linePins[i], INPUT);
    }
    
    // Initialize encoder pins with interrupts
    pinMode(ENCODER_LEFT_A, INPUT_PULLUP);
    pinMode(ENCODER_RIGHT_A, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_A), leftEncoderISR, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_A), rightEncoderISR, RISING);
    
    // Initialize LEDs
    FastLED.addLeds<SK6812, LED_DATA_PIN, GRB>(leds, LED_COUNT);
    FastLED.setBrightness(50);
    clearLeds();
    showLeds();
    
    // Initialize ADS1015
    if (ads.begin(PHOTO_ADC_I2C_ADDR)) {
        _adsInitialized = true;
        ads.setGain(PHOTO_ADC_GAIN);
    }
    
    // Initialize VL53L0X TOF
    pinMode(TOF_XSHUT_PIN, OUTPUT);
    digitalWrite(TOF_XSHUT_PIN, HIGH);
    delay(10);
    if (tof.begin()) {
        _tofInitialized = true;
        tof.startRangeContinuous();
    }
    
    // Initialize LTR-308
    if (lightSensor.begin()) {
        _lightInitialized = true;
        lightSensor.configure(LTR308_ALS_INTEGRATION_50MS, LTR308_GAIN_1);
        lightSensor.setAutoRange(true);
    }
    
    return true;
}

// Battery functions (LC709203F)
float RobotLibrary::getBatteryVoltage() {
    if (!_i2c) return 0.0;
    
    _i2c->beginTransmission(BATTERY_MONITOR_I2C_ADDR);
    _i2c->write(0x0A);  // VBAT register
    _i2c->endTransmission();
    
    _i2c->requestFrom(BATTERY_MONITOR_I2C_ADDR, 2);
    if (_i2c->available() >= 2) {
        uint16_t data = _i2c->read() | (_i2c->read() << 8);
        return (data * 0.001f);  // Convert to volts
    }
    return 0.0;
}

float RobotLibrary::getBatteryPercentage() {
    if (!_i2c) return 0.0;
    
    _i2c->beginTransmission(BATTERY_MONITOR_I2C_ADDR);
    _i2c->write(0x0D);  // RSOC register
    _i2c->endTransmission();
    
    _i2c->requestFrom(BATTERY_MONITOR_I2C_ADDR, 1);
    if (_i2c->available()) {
        return (float)_i2c->read();
    }
    return 0.0;
}

bool RobotLibrary::isCharging() {
    if (!_i2c) return false;
    
    _i2c->beginTransmission(BATTERY_MONITOR_I2C_ADDR);
    _i2c->write(0x0F);  // STATUS register
    _i2c->endTransmission();
    
    _i2c->requestFrom(BATTERY_MONITOR_I2C_ADDR, 2);
    if (_i2c->available() >= 2) {
        uint16_t status = _i2c->read() | (_i2c->read() << 8);
        return (status & 0x0002) != 0;  // CHG bit
    }
    return false;
}

BatteryData RobotLibrary::getBatteryData() {
    BatteryData data;
    data.voltage = getBatteryVoltage();
    data.percentage = getBatteryPercentage();
    data.charging = isCharging();
    return data;
}

// Photoresistors
PhotoResistorData RobotLibrary::getPhotoResistorData() {
    PhotoResistorData data = {0, 0, 0, 0};
    if (_adsInitialized) {
        data.ch0 = ads.readADC_SingleEnded(0);
        data.ch1 = ads.readADC_SingleEnded(1);
        data.ch2 = ads.readADC_SingleEnded(2);
        data.ch3 = ads.readADC_SingleEnded(3);
    }
    return data;
}

uint16_t RobotLibrary::getPhotoResistorValue(uint8_t channel) {
    if (!_adsInitialized || channel > 3) return 0;
    return ads.readADC_SingleEnded(channel);
}

// Line sensors
LineSensorData RobotLibrary::getLineSensorData() {
    LineSensorData data;
    int sum = 0;
    int weightedSum = 0;
    
    for (int i = 0; i < 4; i++) {
        data.s1 = (i == 0) ? digitalRead(_linePins[i]) : data.s1;
        data.s2 = (i == 1) ? digitalRead(_linePins[i]) : data.s2;
        data.s3 = (i == 2) ? digitalRead(_linePins[i]) : data.s3;
        data.s4 = (i == 3) ? digitalRead(_linePins[i]) : data.s4;
        
        bool sensor = digitalRead(_linePins[i]);
        if (sensor) {
            sum++;
            weightedSum += (i - 1.5);  // Center around 0
        }
    }
    
    data.position = (sum > 0) ? (weightedSum / (float)sum) * 0.67f : 0.0;
    return data;
}

float RobotLibrary::getLinePosition() {
    return getLineSensorData().position;
}

// Motors
void RobotLibrary::setMotorSpeed(int16_t left, int16_t right) {
    // Left motor
    if (left >= 0) {
        digitalWrite(_motorLeftIn1, HIGH);
        digitalWrite(_motorLeftIn2, LOW);
        analogWrite(_motorLeftPwm, min(left, 255));
    } else {
        digitalWrite(_motorLeftIn1, LOW);
        digitalWrite(_motorLeftIn2, HIGH);
        analogWrite(_motorLeftPwm, min(-left, 255));
    }
    
    // Right motor
    if (right >= 0) {
        digitalWrite(_motorRightIn1, HIGH);
        digitalWrite(_motorRightIn2, LOW);
        analogWrite(_motorRightPwm, min(right, 255));
    } else {
        digitalWrite(_motorRightIn1, LOW);
        digitalWrite(_motorRightIn2, HIGH);
        analogWrite(_motorRightPwm, min(-right, 255));
    }
}

void RobotLibrary::stopMotors() {
    digitalWrite(_motorLeftIn1, LOW);
    digitalWrite(_motorLeftIn2, LOW);
    analogWrite(_motorLeftPwm, 0);
    digitalWrite(_motorRightIn1, LOW);
    digitalWrite(_motorRightIn2, LOW);
    analogWrite(_motorRightPwm, 0);
}

void RobotLibrary::setLeftMotor(int16_t speed) {
    setMotorSpeed(speed, 0);
}

void RobotLibrary::setRightMotor(int16_t speed) {
    setMotorSpeed(0, speed);
}

// Encoders
EncoderData RobotLibrary::getEncoderData() {
    EncoderData data;
    data.left = _leftEncoderCount;
    data.right = _rightEncoderCount;
    return data;
}

long RobotLibrary::getLeftEncoderCount() {
    return _leftEncoderCount;
}

long RobotLibrary::getRightEncoderCount() {
    return _rightEncoderCount;
}

void RobotLibrary::resetEncoders() {
    _leftEncoderCount = 0;
    _rightEncoderCount = 0;
}

// TOF
uint16_t RobotLibrary::getDistanceMM() {
    if (!_tofInitialized) return 0;
    return tof.readRangeSingleMillimeters();
}

bool RobotLibrary::isDistanceValid() {
    if (!_tofInitialized) return false;
    return !tof.timeoutOccurred();
}

// Light sensor
float RobotLibrary::getLux() {
    if (!_lightInitialized) return 0.0;
    return lightSensor.readALS();
}

bool RobotLibrary::isLightValid() {
    return _lightInitialized;
}

// LEDs
void RobotLibrary::setLedColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < LED_COUNT) {
        leds[index] = CRGB(r, g, b);
        _leds[index][0] = r;
        _leds[index][1] = g;
        _leds[index][2] = b;
    }
}

void RobotLibrary::setAllLedsColor(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < LED_COUNT; i++) {
        setLedColor(i, r, g, b);
    }
}

void RobotLibrary::showLeds() {
    FastLED.show();
}

void RobotLibrary::clearLeds() {
    FastLED.clear();
    for (int i = 0; i < LED_COUNT; i++) {
        _leds[i][0] = _leds[i][1] = _leds[i][2] = 0;
    }
}

// Buzzer
void RobotLibrary::beep(uint16_t frequency, uint16_t duration) {
    playTone(frequency, duration);
}

void RobotLibrary::playTone(uint16_t frequency, uint16_t duration) {
    if (frequency == 0) {
        stopBuzzer();
        return;
    }
    
    uint32_t period = 1000000 / frequency;
    uint32_t halfPeriod = period / 2;
    uint32_t startTime = millis();
    
    while (millis() - startTime < duration) {
        digitalWrite(_buzzerPin, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(_buzzerPin, LOW);
        delayMicroseconds(halfPeriod);
    }
}

void RobotLibrary::stopBuzzer() {
    digitalWrite(_buzzerPin, LOW);
}

// Camera (placeholder - requires ESP32-CAM specific implementation)
bool RobotLibrary::cameraInit() {
    // Camera initialization requires esp32-camera library
    // This is a placeholder for future implementation
    _cameraInitialized = false;
    return false;
}

bool RobotLibrary::cameraCapture() {
    return _cameraInitialized;
}

void RobotLibrary::cameraDeinit() {
    _cameraInitialized = false;
}
