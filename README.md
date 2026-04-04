# Robot Library for ESP32S3-N16R8

Библиотека функций для робота на базе микроконтроллера **ESP32S3-N16R8**.

## Компоненты

| Компонент | Модель | Интерфейс |
|-----------|--------|-----------|
| Микроконтроллер | ESP32S3-N16R8 | - |
| Камера | OV5640 (DFRobot AI CAM) | DVP |
| Монитор батареи | LC709203F | I2C |
| АЦП для фоторезисторов | ADS1015 | I2C |
| Фоторезисторы | 4 шт | ADS1015 |
| Датчики линии | GP25700 (4 шт) | GPIO |
| Контроллер двигателей | DRV8834 | PWM + DIR |
| Энкодеры | OH137 (2 шт) | GPIO Interrupt |
| TOF дальномер | VL53L0X | I2C |
| Датчик освещенности | LTR-308 | I2C |
| Адресные светодиоды | SK6812 (4 шт) | GPIO |
| Buzzer | Пьезо | PWM |

## Структура проекта

```
robot-library/
├── include/
│   ├── robot_config.h      # Конфигурация пинов
│   └── robot_library.h     # Заголовочный файл библиотеки
├── src/
│   └── robot_library.cpp   # Реализация библиотеки
├── examples/
│   └── basic_example.ino   # Пример использования
└── README.md               # Этот файл
```

## Установка

1. Скопируйте папки `include` и `src` в ваш проект Arduino/PlatformIO
2. Для PlatformIO добавьте в `platformio.ini`:
   ```ini
   lib_deps = 
       pololu/vl53l0x
   build_flags = 
       -D USE_ESP_CAMERA
   ```

## Быстрый старт

```cpp
#include "robot_library.h"

void setup() {
    // Инициализация всех компонентов
    robot.begin();
}

void loop() {
    // Чтение датчиков
    float voltage = robot.getBatteryVoltage();
    uint16_t distance = robot.getDistanceMM();
    float lux = robot.getLux();
    
    // Управление двигателями
    robot.setMotorSpeed(100, 100); // Левый, Правый (-255 до 255)
    
    // Светодиоды
    robot.setAllLedsColor(255, 0, 0); // Красный цвет
    robot.showLeds();
    
    delay(1000);
}
```

## API Библиотеки

### Инициализация
- `bool begin()` - Инициализация всех компонентов

### Батарея (LC709203F)
- `float getBatteryVoltage()` - Напряжение батареи (В)
- `float getBatteryPercentage()` - Процент заряда (%)
- `bool isCharging()` - Статус зарядки

### Фоторезисторы (ADS1015)
- `PhotoResistorData getPhotoResistorData()` - Данные со всех 4 каналов
- `uint16_t getPhotoResistorValue(uint8_t channel)` - Значение канала (0-3)

### Датчики линии (GP25700)
- `LineSensorData getLineSensorData()` - Данные со всех датчиков
- `bool getLineSensorValue(uint8_t sensorNum)` - Значение датчика (1-4)
- `uint8_t getLinePosition()` - Позиция линии (0-3 или 255 если нет)

### Двигатели (DRV8834) + Энкодеры (OH137)
- `void setMotorSpeed(int16_t left, int16_t right)` - Установка скорости
- `void setLeftMotorSpeed(int16_t speed)` - Скорость левого мотора
- `void setRightMotorSpeed(int16_t speed)` - Скорость правого мотора
- `void stopMotors()` - Остановка двигателей
- `int32_t getLeftEncoderCount()` - Позиция левого энкодера
- `int32_t getRightEncoderCount()` - Позиция правого энкодера
- `void resetEncoders()` - Сброс энкодеров

### TOF дальномер (VL53L0X)
- `uint16_t getDistanceMM()` - Расстояние в мм
- `bool isDistanceValid()` - Валидность данных

### Датчик освещенности (LTR-308)
- `float getLux()` - Освещенность в Lux
- `bool isLightValid()` - Валидность данных

### Светодиоды (SK6812)
- `void setLedColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b)` - Цвет светодиода
- `void setAllLedsColor(uint8_t r, uint8_t g, uint8_t b)` - Цвет всех светодиодов
- `void clearLeds()` - Выключить все
- `void showLeds()` - Обновить ленту
- `void setLedBrightness(uint8_t brightness)` - Яркость (0-255)

### Buzzer
- `void beep(uint16_t frequency, uint16_t duration_ms)` - Короткий сигнал
- `void playTone(uint16_t frequency, uint16_t duration_ms)` - Воспроизведение тона
- `void stopBuzzer()` - Остановить звук

### Камера (OV5640)
- `bool cameraInit()` - Инициализация камеры
- `bool cameraCapture()` - Захват кадра
- `void cameraDeinit()` - Деинициализация

## Конфигурация пинов

Пины настроены в файле `robot_config.h`. При необходимости измените под вашу схему.

## Зависимости

- **Arduino ESP32 Core** - базовая платформа
- **Wire.h** - I2C коммуникация (входит в core)
- **Pololu VL53L0X Library** (опционально) - для улучшенной работы TOF

## Лицензия

MIT License
