/*
 * Пример использования библиотеки RobotLibrary
 * Для ESP32S3-N16R8
 */

#include "robot_library.h"

void setup() {
    // Инициализация всех компонентов робота
    if (!robot.begin()) {
        Serial.println("Failed to initialize robot!");
        while (1);
    }
    
    // Тестовая индикация успешной инициализации
    robot.setAllLedsColor(0, 255, 0); // Зеленый цвет
    robot.showLeds();
    robot.beep(1000, 100); // Короткий сигнал
    
    delay(500);
    robot.clearLeds();
    robot.showLeds();
}

void loop() {
    // ========================================
    // Чтение датчиков
    // ========================================
    
    // Батарея
    float voltage = robot.getBatteryVoltage();
    float percentage = robot.getBatteryPercentage();
    bool charging = robot.isCharging();
    
    Serial.print("Battery: ");
    Serial.print(voltage);
    Serial.print("V (");
    Serial.print(percentage);
    Serial.print("%)");
    Serial.println(charging ? " [CHARGING]" : "");
    
    // Фоторезисторы
    PhotoResistorData photo = robot.getPhotoResistorData();
    Serial.printf("Photo: [%d, %d, %d, %d]\n", 
                  photo.ch0, photo.ch1, photo.ch2, photo.ch3);
    
    // Датчики линии
    LineSensorData line = robot.getLineSensorData();
    uint8_t linePos = robot.getLinePosition();
    Serial.printf("Line: [%d,%d,%d,%d] Pos: %d\n",
                  line.sensor1, line.sensor2, line.sensor3, line.sensor4,
                  linePos == 255 ? -1 : linePos);
    
    // TOF дальномер
    uint16_t distance = robot.getDistanceMM();
    bool tofValid = robot.isDistanceValid();
    Serial.print("Distance: ");
    Serial.print(tofValid ? distance : 0);
    Serial.println(" mm");
    
    // Освещенность
    float lux = robot.getLux();
    bool lightValid = robot.isLightValid();
    Serial.print("Light: ");
    Serial.print(lightValid ? lux : 0);
    Serial.println(" lux");
    
    // Энкодеры
    int32_t leftEnc = robot.getLeftEncoderCount();
    int32_t rightEnc = robot.getRightEncoderCount();
    Serial.printf("Encoders: L=%d, R=%d\n", leftEnc, rightEnc);
    
    Serial.println("---");
    
    // ========================================
    // Управление двигателями
    // ========================================
    
    // Движение вперед
    robot.setMotorSpeed(100, 100);
    delay(1000);
    
    // Поворот направо
    robot.setMotorSpeed(100, -100);
    delay(500);
    
    // Остановка
    robot.stopMotors();
    delay(500);
    
    // ========================================
    // Управление светодиодами
    // ========================================
    
    // Эффект пробегающего огня
    for (uint8_t i = 0; i < LED_STRIP_COUNT; i++) {
        robot.clearLeds();
        robot.setLedColor(i, 255, 0, 0); // Красный
        robot.showLeds();
        delay(100);
    }
    
    robot.clearLeds();
    robot.showLeds();
    
    // ========================================
    // Проверка препятствий
    // ========================================
    
    if (tofValid && distance < 100) {
        // Препятствие ближе 10см
        robot.beep(2000, 200); // Предупреждающий сигнал
        
        // Мигание красным
        robot.setAllLedsColor(255, 0, 0);
        robot.showLeds();
        delay(200);
        robot.clearLeds();
        robot.showLeds();
    }
    
    delay(500);
}
