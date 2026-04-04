# Robot Library for ESP32S3-N16R8

Полная библиотека функций для робота на базе микроконтроллера **ESP32S3-N16R8**.

## Поддерживаемые компоненты

| Компонент | Модель | Интерфейс |
|-----------|--------|-----------|
| Камера | OV5640 (DFRobot AI CAM) | DVP |
| Монитор батареи | LC709203F | I2C |
| АЦП для фоторезисторов | ADS1015 | I2C |
| Датчики линии | GP25700 (4 шт) | GPIO |
| Контроллер двигателей | DRV8834 | PWM + GPIO |
| Энкодеры | OH137 | GPIO (interrupt) |
| TOF дальномер | VL53L0X | I2C |
| Датчик освещенности | LTR-308 | I2C |
| Адресные светодиоды | SK6812 | GPIO |
| Buzzer | Passive | PWM |

## Установка

### Arduino IDE

1. Скопируйте папку `robot_library` в папку `libraries` вашей Arduino IDE:
   - Windows: `Documents\Arduino\libraries\`
   - macOS: `~/Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`

2. Перезапустите Arduino IDE

3. Откройте пример: **File → Examples → RobotLibrary → basic_example**

### PlatformIO

Добавьте в `platformio.ini`:

```ini
lib_deps = 
    Wire
```

Скопируйте файлы из `src/` в ваш проект.

## Быстрый старт

```cpp
#include <robot_library.h>

void setup() {
  // Инициализация всех компонентов
  robot.begin();
  
  // Тест светодиодов
  robot.setAllLedsColor(0, 255, 0);
  robot.showLeds();
}

void loop() {
  // Чтение данных с датчиков
  BatteryData battery = robot.getBatteryData();
  Serial.print("Voltage: ");
  Serial.println(battery.battery_voltage);
  
  // Управление двигателями
  robot.setMotorSpeed(100, 100);
  
  delay(100);
}
```

## API Reference

### Инициализация

| Функция | Описание |
|---------|----------|
| `robot.begin()` | Инициализирует все компоненты |

### Камера OV5640

| Функция | Описание |
|---------|----------|
| `robot.cameraInit()` | Инициализация камеры |
| `robot.cameraCapture()` | Захват кадра |
| `robot.cameraDeinit()` | Деинициализация камеры |

### Battery Monitor LC709203F

| Функция | Описание |
|---------|----------|
| `robot.batteryInit()` | Инициализация монитора батареи |
| `robot.getBatteryData()` | Возвращает структуру с данными о батарее |
| `robot.getBatteryVoltage()` | Напряжение батареи (В) |
| `robot.getBatteryPercentage()` | Процент заряда (%) |
| `robot.isCharging()` | Статус зарядки |

### Фоторезисторы (ADS1015)

| Функция | Описание |
|---------|----------|
| `robot.photoResistorsInit()` | Инициализация ADS1015 |
| `robot.getPhotoResistorData()` | Данные со всех 4 каналов |
| `robot.getPhotoResistorValue(channel)` | Значение с указанного канала (0-3) |

### Датчики линии GP25700

| Функция | Описание |
|---------|----------|
| `robot.lineSensorsInit()` | Инициализация датчиков линии |
| `robot.getLineSensorData()` | Данные со всех 4 датчиков |
| `robot.getLineSensorValue(num)` | Значение датчика (1-4) |
| `robot.getLinePosition()` | Позиция линии (0-3 или 255) |

### Двигатели DRV8834 + Энкодеры OH137

| Функция | Описание |
|---------|----------|
| `robot.motorsInit()` | Инициализация двигателей и энкодеров |
| `robot.setMotorSpeed(left, right)` | Установка скорости (-255..255) |
| `robot.setLeftMotorSpeed(speed)` | Скорость левого двигателя |
| `robot.setRightMotorSpeed(speed)` | Скорость правого двигателя |
| `robot.stopMotors()` | Остановка обоих двигателей |
| `robot.getEncoderData()` | Данные с обоих энкодеров |
| `robot.getLeftEncoderCount()` | Счетчик левого энкодера |
| `robot.getRightEncoderCount()` | Счетчик правого энкодера |
| `robot.resetEncoders()` | Сброс счетчиков энкодеров |

### TOF дальномер VL53L0X

| Функция | Описание |
|---------|----------|
| `robot.tofInit()` | Инициализация дальномера |
| `robot.getTOFData()` | Структура с данными о расстоянии |
| `robot.getDistanceMM()` | Расстояние в мм |
| `robot.isDistanceValid()` | Валидность данных |

### Датчик освещенности LTR-308

| Функция | Описание |
|---------|----------|
| `robot.lightSensorInit()` | Инициализация датчика |
| `robot.getLightSensorData()` | Структура с данными об освещенности |
| `robot.getLux()` | Освещенность в Lux |
| `robot.isLightValid()` | Валидность данных |

### Светодиоды SK6812

| Функция | Описание |
|---------|----------|
| `robot.ledsInit()` | Инициализация LED ленты |
| `robot.setLedColor(index, r, g, b)` | Цвет конкретного светодиода |
| `robot.setAllLedsColor(r, g, b)` | Цвет всех светодиодов |
| `robot.clearLeds()` | Выключить все светодиоды |
| `robot.showLeds()` | Обновить ленту |
| `robot.setLedBrightness(brightness)` | Яркость (0-255) |

### Buzzer

| Функция | Описание |
|---------|----------|
| `robot.buzzerInit()` | Инициализация buzzer |
| `robot.beep(freq, duration)` | Сигнал указанной частоты и длительности |
| `robot.playTone(freq, duration)` | Воспроизведение тона |
| `robot.stopBuzzer()` | Остановка звука |

## Структуры данных

```cpp
typedef struct {
    int16_t left;
    int16_t right;
} EncoderData;

typedef struct {
    uint16_t ch0, ch1, ch2, ch3;
} PhotoResistorData;

typedef struct {
    bool sensor1, sensor2, sensor3, sensor4;
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
```

## Конфигурация пинов

Пины настроены в файле `robot_config.h`. Основные назначения:

- **I2C**: SDA=18, SCL=38 (общая шина для всех I2C устройств)
- **Камера**: pins 0, 1, 2, 38, 39, 40, 41, 42-48
- **Двигатели**: PWM=7,10; IN1=8,11; IN2=9,12
- **Энкодеры**: A=13,15; B=14,16
- **Датчики линии**: 3, 46, 5, 6
- **LED лента**: 21
- **Buzzer**: 22
- **TOF XSHUT**: 17

## Примеры

Примеры использования доступны в папке `examples/`:

- `basic_example.ino` - Базовый пример со всеми компонентами

## Требования

- Arduino ESP32 Core >= 2.0.0
- Библиотека Wire (входит в состав Arduino)

## Лицензия

MIT License
