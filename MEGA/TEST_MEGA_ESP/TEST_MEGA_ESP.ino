// #include <MemoryFree.h>
#include <EEPROM.h>

#define PIN_LED 13  // вывод светодиода
String inString;

// Для таймера прерывания(Мигаем диодом)
static uint32_t timer;
int ledDelay = 1000;

// Настройка
void setup() {
  // Инициализация портов и выходов
  Serial.begin(115200);
  Serial3.begin(115200);
  delay(500);
  Serial.println("MEGA 2560 + esp-01 data reciver");
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

// Чтение данных из консоли
char monitorRead() {
  char str;
  while (Serial.available()) {
    str = Serial.read();
  }
  return str;
}

// Выполнение
void loop() {
  if(millis() - timer >= ledDelay)
  {
    timer = millis();
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    Serial.println("Wait data from esp-01...");
  }
  // Проверка события на порту esp
  while (Serial3.available()) {
    // Чтение данных из порта Serial3
    char inChar = Serial3.read();
    // Вывод прочитанных данных в порт Serial
    Serial.write(inChar);
  }
}