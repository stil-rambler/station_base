class Button {
  public:
    // Конструктор: настройка пина кнопки/концевика
    Button(byte pin) : _pin(pin), _lastState(HIGH), _lastDebounceTime(0), _flag(false) {
      pinMode(_pin, INPUT_PULLUP);
    }

    /**
     * Проверяет нажатие кнопки с антидребезгом
     * Возвращает true при обнаружении нажатия (фронт сигнала)
     * 
     * @return bool - было ли нажатие
     */
    bool click() {
      bool currentState = digitalRead(_pin);  // Текущее состояние кнопки
      bool result = false;
      
      // Обнаружение изменения состояния
      if (currentState != _lastState) {
        _lastDebounceTime = millis();
      }

      // Проверка времени стабильного состояния (антидребезг)
      if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY) {
        // Обнаружение фронта нажатия (HIGH -> LOW)
        if (!currentState && !_flag) {
          _flag = true;
          result = true;
        }
        
        // Обнаружение отпускания (LOW -> HIGH)
        if (currentState && _flag) {
          _flag = false;
        }
      }
      
      _lastState = currentState;
      return result;
    }

    /**
     * Проверяет текущее состояние кнопки (без ожидания фронта)
     * @return bool - true если кнопка нажата (LOW)
     */
    bool isPressed() {
      return (digitalRead(_pin) == LOW);
    }

  private:
    static const uint32_t DEBOUNCE_DELAY = 50;  // Время антидребезга (мс)
    byte _pin;              // Пин кнопки
    bool _lastState;        // Предыдущее состояние
    uint32_t _lastDebounceTime; // Время последнего изменения
    bool _flag;             // Флаг нажатия
};
