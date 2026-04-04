/*
 * Robot Library - Figure Eight Example
 * 
 * Этот скетч демонстрирует:
 * 1. Движение по траектории "восьмерка"
 * 2. Воспроизведение мелодии из Animal Crossing на buzzer
 * 3. Поочередное мигание адресными светодиодами
 * 
 * Оборудование:
 * - ESP32S3-N16R8
 * - DRV8834 мотор контроллер с энкодерами
 * - SK6812 адресные светодиоды (4 шт)
 * - Passive Buzzer
 */

#include <robot_library.h>

// ============================================
// Константы для движения по восьмерке
// ============================================

// Параметры восьмерки
#define FIGURE_EIGHT_DURATION_MS    8000    // Общая длительность цикла восьмерки
#define SEGMENT_TIME_MS             2000    // Время на каждый сегмент

// Скорости моторов для разных сегментов восьмерки
// Формат: {левый мотор, правый мотор}
const int16_t figureEightPatterns[4][2] = {
  {150, 150},   // Сегмент 0: движение вперед (низ петли)
  {80, 180},    // Сегмент 1: поворот влево (первая петля)
  {150, 150},   // Сегмент 2: движение вперед (верх петли/пересечение)
  {180, 80}     // Сегмент 3: поворот вправо (вторая петля)
};

// ============================================
// Мелодия из Animal Crossing (тема K.K. Slider)
// Упрощенная версия основной темы
// ============================================

// Частоты нот (Гц)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_REST 0

// Структура для ноты мелодии
struct Note {
  uint16_t frequency;
  uint16_t duration_ms;
};

// Мелодия Animal Crossing (основная тема)
const Note animalCrossingMelody[] = {
  {NOTE_G4,  200}, {NOTE_A4,  200}, {NOTE_B4,  200}, {NOTE_C5,  400},
  {NOTE_B4,  200}, {NOTE_A4,  200}, {NOTE_G4,  200}, {NOTE_A4,  400},
  {NOTE_E4,  200}, {NOTE_F4,  200}, {NOTE_G4,  200}, {NOTE_A4,  400},
  {NOTE_G4,  200}, {NOTE_F4,  200}, {NOTE_E4, 200},  {NOTE_D4,  400},
  {NOTE_C4,  200}, {NOTE_D4,  200}, {NOTE_E4,  200}, {NOTE_F4,  400},
  {NOTE_E4,  200}, {NOTE_D4,  200}, {NOTE_C4,  200}, {NOTE_D4,  400},
  {NOTE_G4,  200}, {NOTE_A4,  200}, {NOTE_B4,  200}, {NOTE_C5,  400},
  {NOTE_D5,  300}, {NOTE_C5,  100}, {NOTE_B4,  200}, {NOTE_A4,  400},
  {NOTE_REST, 200}, // Пауза
  {NOTE_E4,  200}, {NOTE_F4,  200}, {NOTE_G4,  200}, {NOTE_A4,  400},
  {NOTE_B4,  200}, {NOTE_C5,  200}, {NOTE_D5,  200}, {NOTE_E5,  400},
  {NOTE_D5,  200}, {NOTE_C5,  200}, {NOTE_B4,  200}, {NOTE_C5,  400},
  {NOTE_A4,  400}, {NOTE_REST, 200}
};

const int melodyLength = sizeof(animalCrossingMelody) / sizeof(Note);

// ============================================
// Конфигурация светодиодов
// ============================================

// Цвета для светодиодов (RGB)
const uint8_t ledColors[4][3] = {
  {255, 0, 0},    // LED 0: Красный
  {0, 255, 0},    // LED 1: Зеленый
  {0, 0, 255},    // LED 2: Синий
  {255, 255, 0}   // LED 3: Желтый
};

// Тайминги для мигания
#define LED_BLINK_INTERVAL_MS   250   // Интервал переключения светодиодов
#define LED_BRIGHTNESS          50    // Яркость (0-255)

// ============================================
// Глобальные переменные
// ============================================

RobotLibrary robot;

uint32_t lastLedUpdate = 0;
uint8_t currentLedIndex = 0;
bool ledsOn = true;

uint32_t melodyStartTime = 0;
int currentNoteIndex = 0;
uint32_t noteStartTime = 0;
bool melodyPlaying = false;

uint32_t figureEightStartTime = 0;
int currentSegment = 0;

// ============================================
// Функции
// ============================================

/**
 * @brief Инициализация всех компонентов робота
 */
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== Robot Library - Figure Eight Example ===");
  Serial.println("Starting initialization...");
  
  // Инициализация библиотеки
  if (!robot.begin()) {
    Serial.println("ERROR: Failed to initialize robot library!");
    
    // Мигаем красным при ошибке
    robot.setAllLedsColor(255, 0, 0);
    robot.showLeds();
    
    while (1) {
      robot.beep(1000, 200);
      delay(500);
    }
  }
  
  Serial.println("✓ Robot library initialized successfully");
  
  // Тест светодиодов
  Serial.println("Testing LEDs...");
  for (int i = 0; i < 4; i++) {
    robot.setLedColor(i, ledColors[i][0], ledColors[i][1], ledColors[i][2]);
    robot.showLeds();
    delay(200);
  }
  robot.clearLeds();
  robot.showLeds();
  
  // Тест buzzer
  Serial.println("Testing Buzzer...");
  robot.playTone(NOTE_A4, 300);
  delay(350);
  robot.playTone(NOTE_E4, 300);
  delay(350);
  
  // Сброс энкодеров
  robot.resetEncoders();
  
  Serial.println("✓ All tests passed!");
  Serial.println("Starting figure eight pattern with music and LED show...");
  Serial.println();
  
  // Инициализация таймеров
  figureEightStartTime = robot.getMillis();
  melodyStartTime = robot.getMillis();
  melodyPlaying = true;
}

/**
 * @brief Обновление состояния светодиодов
 * Поочередное включение/выключение светодиодов
 */
void updateLEDs() {
  uint32_t currentTime = robot.getMillis();
  
  if (currentTime - lastLedUpdate >= LED_BLINK_INTERVAL_MS) {
    lastLedUpdate = currentTime;
    
    if (ledsOn) {
      // Выключаем все и включаем следующий
      robot.clearLeds();
      robot.setLedColor(
        currentLedIndex,
        ledColors[currentLedIndex][0],
        ledColors[currentLedIndex][1],
        ledColors[currentLedIndex][2]
      );
    } else {
      // Выключаем текущий
      robot.setLedColor(currentLedIndex, 0, 0, 0);
    }
    
    robot.showLeds();
    
    // Переключаем состояние
    ledsOn = !ledsOn;
    
    // Если только что выключили, переходим к следующему светодиоду
    if (!ledsOn) {
      currentLedIndex = (currentLedIndex + 1) % 4;
    }
    
    // Отладочная информация
    Serial.print("LED: ");
    Serial.print(currentLedIndex);
    Serial.println(ledsOn ? " ON" : " OFF");
  }
}

/**
 * @brief Воспроизведение мелодии
 * Проигрывает ноты последовательно с повторением
 */
void updateMelody() {
  if (!melodyPlaying) return;
  
  uint32_t currentTime = robot.getMillis();
  
  // Проверка, пора ли играть следующую ноту
  if (currentTime - noteStartTime >= animalCrossingMelody[currentNoteIndex].duration_ms) {
    // Переход к следующей ноте
    currentNoteIndex++;
    
    // Если дошли до конца мелодии, начинаем сначала
    if (currentNoteIndex >= melodyLength) {
      currentNoteIndex = 0;
      Serial.println("--- Melody loop ---");
    }
    
    // Играем ноту
    uint16_t freq = animalCrossingMelody[currentNoteIndex].frequency;
    uint16_t duration = animalCrossingMelody[currentNoteIndex].duration_ms;
    
    if (freq > 0) {
      robot.playTone(freq, duration);
      Serial.print("♪ Note: ");
      Serial.print(freq);
      Serial.print(" Hz (");
      Serial.print(duration);
      Serial.println(" ms)");
    } else {
      robot.stopBuzzer();
      Serial.println("♫ Rest (pause)");
    }
    
    noteStartTime = currentTime;
  }
}

/**
 * @brief Движение по траектории восьмерки
 * Меняет скорости моторов по сегментам
 */
void updateFigureEight() {
  uint32_t currentTime = robot.getMillis();
  uint32_t elapsedTime = currentTime - figureEightStartTime;
  
  // Определяем текущий сегмент (0-3)
  int newSegment = (elapsedTime / SEGMENT_TIME_MS) % 4;
  
  // Если сегмент изменился, выводим информацию
  if (newSegment != currentSegment) {
    currentSegment = newSegment;
    
    Serial.print("\n→ Segment ");
    Serial.print(currentSegment);
    Serial.print(": Left=");
    Serial.print(figureEightPatterns[currentSegment][0]);
    Serial.print(", Right=");
    Serial.println(figureEightPatterns[currentSegment][1]);
    
    // Получаем данные с энкодеров
    EncoderData encoders = robot.getEncoderData();
    Serial.print("  Encoders - L: ");
    Serial.print(encoders.left);
    Serial.print(", R: ");
    Serial.println(encoders.right);
    
    // Получаем данные с датчика расстояния
    TOFData tof = robot.getTOFData();
    if (tof.valid) {
      Serial.print("  Distance: ");
      Serial.print(tof.distance_mm);
      Serial.println(" mm");
    }
  }
  
  // Устанавливаем скорости моторов для текущего сегмента
  robot.setMotorSpeed(
    figureEightPatterns[currentSegment][0],
    figureEightPatterns[currentSegment][1]
  );
}

/**
 * @brief Основной цикл
 */
void loop() {
  // Обновляем движение по восьмерке
  updateFigureEight();
  
  // Обновляем мелодию
  updateMelody();
  
  // Обновляем светодиоды
  updateLEDs();
  
  // Небольшая задержка для стабильности
  delay(10);
}
