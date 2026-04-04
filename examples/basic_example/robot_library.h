#ifndef ROBOT_LIBRARY_H
#define ROBOT_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>
#include "robot_config.h"

// Forward declarations for library dependencies
class Adafruit_ADS1015;
class Adafruit_VL53L0X;
class Adafruit_LTR308;
class ESP32Camera;

struct PhotoResistorData {
    uint16_t ch0, ch1, ch2, ch3;
};

struct LineSensorData {
    bool s1, s2, s3, s4;
    float position;  // -1.0 to 1.0
};

struct EncoderData {
    long left, right;
};

struct BatteryData {
    float voltage;
    float percentage;
    bool charging;
};

class RobotLibrary {
private:
    TwoWire* _i2c;
    
    // Camera
    ESP32Camera* _camera;
    bool _cameraInitialized;
    
    // ADS1015
    Adafruit_ADS1015* _ads;
    bool _adsInitialized;
    
    // VL53L0X
    Adafruit_VL53L0X* _tof;
    bool _tofInitialized;
    
    // LTR-308
    Adafruit_LTR308* _lightSensor;
    bool _lightInitialized;
    
    // LEDs
    uint8_t _leds[LED_COUNT][3];
    
    // Encoders
    volatile long _leftEncoderCount;
    volatile long _rightEncoderCount;
    
    // Motor pins
    uint8_t _motorLeftPwm, _motorLeftIn1, _motorLeftIn2;
    uint8_t _motorRightPwm, _motorRightIn1, _motorRightIn2;
    
    // Buzzer
    uint8_t _buzzerPin;
    
    // Line sensors
    uint8_t _linePins[4];
    
    void _updateEncoderLeft();
    void _updateEncoderRight();
    
public:
    RobotLibrary();
    
    bool begin();
    
    // Camera functions
    bool cameraInit();
    bool cameraCapture();
    void cameraDeinit();
    
    // Battery functions
    float getBatteryVoltage();
    float getBatteryPercentage();
    bool isCharging();
    BatteryData getBatteryData();
    
    // Photoresistors (ADS1015)
    PhotoResistorData getPhotoResistorData();
    uint16_t getPhotoResistorValue(uint8_t channel);
    
    // Line sensors
    LineSensorData getLineSensorData();
    float getLinePosition();
    
    // Motors
    void setMotorSpeed(int16_t left, int16_t right);
    void stopMotors();
    void setLeftMotor(int16_t speed);
    void setRightMotor(int16_t speed);
    
    // Encoders
    EncoderData getEncoderData();
    long getLeftEncoderCount();
    long getRightEncoderCount();
    void resetEncoders();
    
    // TOF distance sensor
    uint16_t getDistanceMM();
    bool isDistanceValid();
    
    // Light sensor (LTR-308)
    float getLux();
    bool isLightValid();
    
    // LEDs (SK6812)
    void setLedColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    void setAllLedsColor(uint8_t r, uint8_t g, uint8_t b);
    void showLeds();
    void clearLeds();
    
    // Buzzer
    void beep(uint16_t frequency, uint16_t duration);
    void playTone(uint16_t frequency, uint16_t duration);
    void stopBuzzer();
};

extern RobotLibrary robot;

#endif
