#ifndef ROBOT_LIBRARY_H
#define ROBOT_LIBRARY_H

#include "Arduino.h"
#include "robot_config.h"
#include "Wire.h"

// ============================================
// Структуры данных
// ============================================

typedef struct {
    int16_t left;
    int16_t right;
} EncoderData;

typedef struct {
    uint16_t ch0;
    uint16_t ch1;
    uint16_t ch2;
    uint16_t ch3;
} PhotoResistorData;

typedef struct {
    bool sensor1;
    bool sensor2;
    bool sensor3;
    bool sensor4;
} LineSensorData;

typedef struct {
    float distance_mm;
    bool valid;
} TOFData;

typedef struct {
    float lux;
    bool valid;
} LightSensorData;

typedef struct {
    float battery_voltage;
    float battery_percentage;
    bool charging;
    bool valid;
} BatteryData;

// ============================================
// Класс RobotLibrary
// ============================================

class RobotLibrary {
public:
    RobotLibrary();
    
    // Инициализация всех компонентов
    bool begin();
    
    // ========================================
    // Камера OV5640
    // ========================================
    bool cameraInit();
    bool cameraCapture();
    void cameraDeinit();
    
    // ========================================
    // Battery Monitor LC709203F
    // ========================================
    bool batteryInit();
    BatteryData getBatteryData();
    float getBatteryVoltage();
    float getBatteryPercentage();
    bool isCharging();
    
    // ========================================
    // Фоторезисторы через ADS1015
    // ========================================
    bool photoResistorsInit();
    PhotoResistorData getPhotoResistorData();
    uint16_t getPhotoResistorValue(uint8_t channel);
    
    // ========================================
    // Датчики линии GP25700
    // ========================================
    bool lineSensorsInit();
    LineSensorData getLineSensorData();
    bool getLineSensorValue(uint8_t sensorNum);
    uint8_t getLinePosition(); // Возвращает позицию 0-3 или 255 если нет линии
    
    // ========================================
    // Контроллер двигателей DRV8834 с энкодерами
    // ========================================
    bool motorsInit();
    void setMotorSpeed(int16_t leftSpeed, int16_t rightSpeed);
    void setLeftMotorSpeed(int16_t speed);
    void setRightMotorSpeed(int16_t speed);
    void stopMotors();
    EncoderData getEncoderData();
    int32_t getLeftEncoderCount();
    int32_t getRightEncoderCount();
    void resetEncoders();
    
    // ========================================
    // TOF дальномер VL53L0X
    // ========================================
    bool tofInit();
    TOFData getTOFData();
    uint16_t getDistanceMM();
    bool isDistanceValid();
    
    // ========================================
    // Цифровой датчик освещенности LTR-308
    // ========================================
    bool lightSensorInit();
    LightSensorData getLightSensorData();
    float getLux();
    bool isLightValid();
    
    // ========================================
    // Адресные светодиоды SK6812
    // ========================================
    bool ledsInit();
    void setLedColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    void setAllLedsColor(uint8_t r, uint8_t g, uint8_t b);
    void clearLeds();
    void showLeds();
    void setLedBrightness(uint8_t brightness); // 0-255
    
    // ========================================
    // Buzzer
    // ========================================
    bool buzzerInit();
    void beep(uint16_t frequency, uint16_t duration_ms);
    void playTone(uint16_t frequency, uint16_t duration_ms);
    void stopBuzzer();
    
    // ========================================
    // Утилиты
    // ========================================
    void delayMs(uint32_t ms);
    uint32_t getMillis();
    void printDebug(const char* message);
    
private:
    bool _initialized;
    
    // Состояние компонентов
    bool _cameraReady;
    bool _batteryReady;
    bool _photoResistorsReady;
    bool _lineSensorsReady;
    bool _motorsReady;
    bool _tofReady;
    bool _lightSensorReady;
    bool _ledsReady;
    bool _buzzerReady;
    
    // Переменные для энкодеров
    volatile int32_t _leftEncoderCount;
    volatile int32_t _rightEncoderCount;
    
    // I2C объект
    TwoWire* _i2c;
    
    // Внутренние функции
    void _setupI2C();
    void _setupEncoderInterrupts();
    static void IRAM_ATTR _leftEncoderISR();
    static void IRAM_ATTR _rightEncoderISR();
};

// Глобальный экземпляр
extern RobotLibrary robot;

#endif // ROBOT_LIBRARY_H
