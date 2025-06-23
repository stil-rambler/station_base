#include "button.h"

// Константы для пинов
constexpr uint8_t dirPin = 5;
constexpr uint8_t stepPin = 2;
constexpr uint8_t dirPin2 = 6;
constexpr uint8_t stepPin2 = 3;
constexpr uint8_t enPin = 8;
constexpr uint8_t button_pod = 9;    // концевик подъемника
constexpr uint8_t button_kar = 10;   // концевик карусели
constexpr uint8_t button_tolat1 = 11; // концевик выгрузки

// Объекты кнопок (концевиков)
button liftEndStop(button_pod);    // Подъемник
button carouselEndStop(button_kar); // Карусель
button pusherEndStop(button_tolat1); // Толкатель

// Состояние системы
struct SystemState {
  int carouselPosition = 0;
  bool liftAtTop = false;
  bool liftAtBottom = false;
  bool carouselAtHome = false;
  bool pusherRetracted = false;
} systemState;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH); // Изначально моторы выключены
  
  Serial.begin(9600);
  Serial.println("System ready");
}

// Универсальная функция управления шаговым двигателем
bool stepMotor(int steps, uint8_t direction, uint8_t motorPin, uint8_t dirPin, 
              uint8_t enablePin, unsigned int stepDelay, bool checkEndStop = false) {
  if (steps == 0) return true;

  digitalWrite(enablePin, LOW);
  digitalWrite(dirPin, direction);
  
  bool completed = true;
  for (int i = 0; i < abs(steps); i++) {
    // Проверка концевика, если требуется
    if (checkEndStop && (liftEndStop.click() || carouselEndStop.click() || pusherEndStop.click())) {
      completed = false;
      break;
    }
    
    digitalWrite(motorPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(motorPin, LOW);
    delayMicroseconds(stepDelay);
  }
  
  digitalWrite(enablePin, HIGH);
  return completed;
}

// Обновление состояния системы
void updateSystemState() {
  systemState.liftAtTop = (digitalRead(button_pod) == LOW);
  systemState.carouselAtHome = (digitalRead(button_kar) == LOW);
  systemState.pusherRetracted = (digitalRead(button_tolat1) == LOW);
}

// Обработка команд
void processCommand(const String& command) {
  updateSystemState();
  
  if (command.startsWith("K ")) { // Управление каруселью
    int steps = command.substring(2).toInt();
    uint8_t direction = steps > 0 ? HIGH : LOW;
    steps = abs(steps);
    
    Serial.print("Moving carousel ");
    Serial.print(steps);
    Serial.println(" steps");
    
    bool success = stepMotor(steps, direction, stepPin2, dirPin2, enPin, 5000, true);
    
    if (success) {
      systemState.carouselPosition += (direction == HIGH ? steps : -steps);
      Serial.println("Movement completed");
    } else {
      Serial.println("Movement interrupted by endstop");
    }
  }
  else if (command.startsWith("P ")) { // Управление подъемником
    int steps = command.substring(2).toInt();
    uint8_t direction = steps > 0 ? HIGH : LOW;
    steps = abs(steps);
    
    Serial.print("Moving lift ");
    Serial.print(steps);
    Serial.println(" steps");
    
    bool success = stepMotor(steps, direction, stepPin, dirPin, enPin, 1000, true);
    
    if (success) {
      Serial.println("Movement completed");
    } else {
      Serial.println("Movement interrupted by endstop");
      systemState.liftAtTop = (direction == HIGH);
      systemState.liftAtBottom = (direction == LOW);
    }
  }
  else if (command == "C") { // Автоматический цикл
    Serial.println("Starting automatic cycle");
    
    // Подъем
    if (!systemState.liftAtTop) {
      Serial.println("Lifting up");
      stepMotor(20000, HIGH, stepPin, dirPin, enPin, 1000, true);
      delay(1000);
    }
    
    // Опускание
    if (!systemState.liftAtBottom) {
      Serial.println("Lowering down");
      stepMotor(20000, LOW, stepPin, dirPin, enPin, 1000, true);
      delay(1000);
    }
    
    // Вращение карусели
    Serial.println("Rotating carousel");
    stepMotor(516, HIGH, stepPin2, dirPin2, enPin, 5000, true);
    
    Serial.println("Cycle completed");
  }
  else if (command == "STATUS") {
    Serial.println("System status:");
    Serial.print("Carousel position: ");
    Serial.println(systemState.carouselPosition);
    Serial.print("Lift at top: ");
    Serial.println(systemState.liftAtTop ? "YES" : "NO");
    Serial.print("Lift at bottom: ");
    Serial.println(systemState.liftAtBottom ? "YES" : "NO");
    Serial.print("Carousel at home: ");
    Serial.println(systemState.carouselAtHome ? "YES" : "NO");
    Serial.print("Pusher retracted: ");
    Serial.println(systemState.pusherRetracted ? "YES" : "NO");
  }
  else {
    Serial.print("Unknown command: ");
    Serial.println(command);
  }
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    processCommand(command);
  }
  
  // Регулярное обновление состояния
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 100) {
    updateSystemState();
    lastUpdate = millis();
  }
}
