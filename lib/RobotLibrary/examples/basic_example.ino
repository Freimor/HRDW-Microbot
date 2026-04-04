#include <Arduino.h>
#include "robot_library.h"

// K.K. Slider melody from Animal Crossing (simplified version)
const uint16_t kkMelody[] = {
  523, 200,   // C5
  659, 200,   // E5
  784, 200,   // G5
  659, 400,   // E5
  523, 200,   // C5
  587, 200,   // D5
  659, 200,   // E5
  523, 400,   // C5
  440, 200,   // A4
  523, 200,   // C5
  659, 200,   // E5
  523, 400,   // C5
  392, 200,   // G4
  440, 200,   // A4
  523, 200,   // C5
  440, 400,   // A4
  349, 200,   // F4
  392, 200,   // G4
  440, 200,   // A4
  392, 400,   // G4
  330, 200,   // E4
  349, 200,   // F4
  392, 200,   // G4
  330, 400,   // E4
  294, 200,   // D4
  330, 200,   // E4
  392, 200,   // G4
  330, 400,   // E4
  262, 200,   // C4
  294, 200,   // D4
  330, 200,   // E4
  294, 400,   // D4
  262, 200,   // C4
  294, 200,   // D4
  330, 200,   // E4
  262, 400,   // C4
  0, 100      // Pause
};

const int melodyLength = sizeof(kkMelody) / sizeof(kkMelody[0]) / 2;
int currentNote = 0;
unsigned long lastNoteTime = 0;
const unsigned long noteInterval = 250;

// LED animation
int currentLed = 0;
unsigned long lastLedChange = 0;
const unsigned long ledInterval = 250;

// Figure-8 movement parameters
unsigned long segmentStartTime = 0;
const unsigned long segmentDuration = 2000;  // 2 seconds per segment
int movementPhase = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("HRDW Microbot - Figure 8 with K.K. Slider");
  
  if (!robot.begin()) {
    Serial.println("Failed to initialize robot!");
    while (1);
  }
  
  Serial.println("Robot initialized successfully!");
  
  // Startup sequence
  robot.setAllLedsColor(0, 50, 0);
  robot.showLeds();
  robot.beep(1000, 200);
  delay(300);
  robot.clearLeds();
  robot.showLeds();
  
  resetEncodersAndTimers();
}

void loop() {
  // Read and display sensor data
  BatteryData battery = robot.getBatteryData();
  Serial.printf("Battery: %.2fV (%.0f%%) %s\n", 
                battery.voltage, battery.percentage, 
                battery.charging ? "[CHG]" : "");
  
  PhotoResistorData photo = robot.getPhotoResistorData();
  Serial.printf("Photo: [%d, %d, %d, %d]\n", 
                photo.ch0, photo.ch1, photo.ch2, photo.ch3);
  
  LineSensorData line = robot.getLineSensorData();
  Serial.printf("Line: [%d,%d,%d,%d] pos=%.2f\n", 
                line.s1, line.s2, line.s3, line.s4, line.position);
  
  uint16_t distance = robot.getDistanceMM();
  if (robot.isDistanceValid()) {
    Serial.printf("Distance: %dmm\n", distance);
  }
  
  float lux = robot.getLux();
  if (robot.isLightValid()) {
    Serial.printf("Light: %.1f lux\n", lux);
  }
  
  EncoderData encoders = robot.getEncoderData();
  Serial.printf("Encoders: L=%ld, R=%ld\n", encoders.left, encoders.right);
  
  // Play K.K. Slider melody
  unsigned long currentTime = millis();
  if (currentTime - lastNoteTime >= kkMelody[currentNote * 2 + 1]) {
    currentNote++;
    if (currentNote >= melodyLength) {
      currentNote = 0;
    }
    
    uint16_t freq = kkMelody[currentNote * 2];
    uint16_t duration = kkMelody[currentNote * 2 + 1];
    
    if (freq > 0) {
      robot.playTone(freq, duration);
    } else {
      robot.stopBuzzer();
    }
    
    lastNoteTime = currentTime;
  }
  
  // Animate LEDs sequentially
  if (currentTime - lastLedChange >= ledInterval) {
    robot.clearLeds();
    robot.setLedColor(currentLed, 0, 50, 100);
    robot.showLeds();
    
    currentLed = (currentLed + 1) % LED_COUNT;
    lastLedChange = currentTime;
  }
  
  // Figure-8 movement pattern
  if (currentTime - segmentStartTime >= segmentDuration) {
    movementPhase = (movementPhase + 1) % 4;
    segmentStartTime = currentTime;
  }
  
  switch (movementPhase) {
    case 0:  // Forward right arc
      robot.setMotorSpeed(150, 100);
      break;
    case 1:  // Forward left arc
      robot.setMotorSpeed(100, 150);
      break;
    case 2:  // Backward left arc
      robot.setMotorSpeed(-100, -150);
      break;
    case 3:  // Backward right arc
      robot.setMotorSpeed(-150, -100);
      break;
  }
  
  delay(50);
}

void resetEncodersAndTimers() {
  robot.resetEncoders();
  segmentStartTime = millis();
  lastNoteTime = millis();
  lastLedChange = millis();
  movementPhase = 0;
  currentNote = 0;
  currentLed = 0;
}
