/*
 * Robot Library - Basic Example
 * 
 * This example demonstrates how to use the RobotLibrary
 * for ESP32S3-N16R8 based robot.
 */

#include <robot_library.h>

void setup() {
  // Инициализация всех компонентов робота
  if (!robot.begin()) {
    Serial.println("Failed to initialize robot!");
    while (1);
  }
  
  Serial.println("Robot initialized successfully!");
  
  // Тест светодиодов - мигнем зеленым
  robot.setAllLedsColor(0, 255, 0);
  robot.showLeds();
  delay(500);
  robot.clearLeds();
  robot.showLeds();
  
  // Тест buzzer
  robot.beep(1000, 200);
  
  Serial.println("Setup complete. Starting main loop...");
}

void loop() {
  // ========================================
  // Чтение данных с датчиков
  // ========================================
  
  // Battery
  BatteryData battery = robot.getBatteryData();
  if (battery.valid) {
    Serial.print("Battery: ");
    Serial.print(battery.battery_voltage);
    Serial.print("V (");
    Serial.print(battery.battery_percentage);
    Serial.print("%)");
    if (battery.charging) {
      Serial.print(" [CHARGING]");
    }
    Serial.println();
  }
  
  // Photoresistors
  PhotoResistorData photo = robot.getPhotoResistorData();
  Serial.printf("Photo: CH0=%d, CH1=%d, CH2=%d, CH3=%d\n", 
                photo.ch0, photo.ch1, photo.ch2, photo.ch3);
  
  // Line sensors
  LineSensorData line = robot.getLineSensorData();
  uint8_t linePos = robot.getLinePosition();
  Serial.printf("Line: S1=%d, S2=%d, S3=%d, S4=%d, Pos=%d\n",
                line.sensor1, line.sensor2, line.sensor3, line.sensor4, linePos);
  
  // TOF distance
  TOFData tof = robot.getTOFData();
  if (tof.valid) {
    Serial.printf("Distance: %.1f mm\n", tof.distance_mm);
  }
  
  // Light sensor
  LightSensorData light = robot.getLightSensorData();
  if (light.valid) {
    Serial.printf("Light: %.1f lux\n", light.lux);
  }
  
  // Encoders
  EncoderData encoders = robot.getEncoderData();
  Serial.printf("Encoders: L=%ld, R=%ld\n", encoders.left, encoders.right);
  
  // ========================================
  // Управление двигателями (пример)
  // ========================================
  
  // Движение вперед
  robot.setMotorSpeed(100, 100);
  delay(100);
  
  // Проверка препятствия спереди
  if (robot.getDistanceMM() < 200 && robot.isDistanceValid()) {
    // Препятствие близко - останавливаемся
    robot.stopMotors();
    robot.setAllLedsColor(255, 0, 0); // Красный
    robot.beep(2000, 100);
  } else {
    // Путь свободен
    robot.setAllLedsColor(0, 0, 255); // Синий
  }
  
  robot.showLeds();
  
  // ========================================
  // Задержка перед следующим циклом
  // ========================================
  delay(100);
}
