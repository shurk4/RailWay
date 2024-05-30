// Обработка команд полученных через последовательный порт от ESP-01
/** Состав команды
  1 символ command[0] игнорируем
  2 символ command[1] группа параметров
    &0 - Поезд
      &00 - остановить
      &01 - начать движение вправо
      &02 - начать движение влево
      &03 - увеличить скорость
      &04 - уменьшить скорость
      &05 - установить скорость
        &05ХХХ - 3 символа собрать в строку и перевести в инт
    &1 - Механика мира
      &10 - шакбаум открыть/закрыть
      &11 - стрелка депо
      &12 - стрелки мира
    &2 - Свет
      &20 - вкл/выкл весь свет
      &21 - свет 1
      &22 - свет 2
      &23 - свет 3

    &8 - ping
    &9 - client connected
**/

// Инициализация мира

void initialize() {
  lightHousesOnOff();  // Вкл свет в домах
  delay(100);
  void lightOutUp();  // Вкл фонари
  delay(100);
  lightOutDown();      // выкл фонари
  lightHousesOnOff();  // выкл свет в домах

  barrierUpDown();  // поднять шлакбаум
  roundArrow();     // стрелки на большой круг
  // Шевельнуть стрелку депо
  depoArrow();
  depoArrow();

  // Рывок поезда
  trainMove(TO_LEFT);
  trainSetSpeed(255);
  delay(10);
  trainSetSpeed(0);
  // Рывок поезда

  delay(100);
  barrierUpDown();  // опустить шлакбаум
}

// Получает данные от ESP-01, команда - выполняется, текст просто выводится в консоль
void getSerialWiFiData() {
  while (Serial3.available()) {
    // Чтение данных из порта Serial3
    char inChar = Serial3.read();
    // Вывод прочитанных данных в порт Serial
    if (isCommand) {
      isConnected = true;
      command += inChar;
    } else if (inChar == '&') {
      Serial.println("Command recived!");
      isCommand = true;
    } else {
      Serial.write(inChar);
    }
  }

  if (command.length() > 0) {
    Serial.print("command.length()");
    Serial.println(command.length());
    Serial.println(command);
  }
}

void world(char &com) {
  switch (com) {
    case '0':
      barrierUpDown();
      break;

    case '1':
      depoArrow();
      break;

    case '2':
      roundArrow();
      break;
  }
}

void ligth(char &com) {
  switch (com) {
    case '0':
    if(worldState & STREET_LIGHT)
    {
      lightOutDown();
    }
    else
    {
      lightOutUp();
    }
      break;

    case '1':
      lightHousesOnOff();
      break;

    case '2':
      lightHousesOnOff();
      break;
  }
}

void checkWiFiCommand() {
  getSerialWiFiData();

  if (isCommand /** && (command.length() == 1 || command.length() == 5)**/) {
    Serial.print("Command recived ");
    Serial.println(command);
    switch (command[0]) {
      case '0':
        train(command[1]);
        break;

      case '1':
        world(command[1]);
        break;

      case '2':
        ligth(command[1]);
        break;

      case '8':
        Serial.println("ping");
        break;

      case '9':
        Serial.println("Client connected");
        isConnected = true;
        break;
    }

    sendStates();
  } else if (command.length() == 0) {
  } else {
    Serial.println("Mega: WrongCommand!");
  }
  isCommand = false;
  command = "";
}

void train(char &com) {
  Serial.print("Train ");
  switch (com) {
    case '0':
      trainSetSpeed(0);
      break;

    case '1':
      Serial.println("move to right");
      trainMove(TO_RIGHT);
      break;

    case '2':
      Serial.println("move to left");
      trainMove(TO_LEFT);
      break;

    case '3':
      Serial.println("speed up");
      trainSpeedUp();
      break;

    case '4':
      Serial.println("speed down");
      trainSpeedDown();
      break;

    case '5':
      Serial.println("set speed");
      if (command.length() == 6) {
        String speedStr = "";
        speedStr += command[3] + command[4] + command[5];
        int speed = speedStr.toInt();
        trainSetSpeed(speed);
      } else {
        Serial.println("Mega: WrongTrainSpeed!");
      }
      break;
  }
}

// Обработка команд с ИК пульта
void checkIRCommand() {
  if (receiver.decode(&output)) {
    unsigned int value = output.value;
    switch (value) {
      case RIGHT:  //движение вперёд
        trainMove(TO_RIGHT);
        break;

      case LEFT:  //движение назад
        trainMove(TO_LEFT);
        break;

      case UP:  //скорость +
        trainSpeedUp();
        break;

      case DWN:  //скорость -
        trainSpeedDown();
        break;

      case OK:  // остановить поезд
        trainSetSpeed(0);
        break;

      case CH_UP:  //свет фонари вкл
        lightOutUp();
        break;

      case CH_DWN:  //свет фонари выкл
        lightOutDown();
        break;

      case POW:  //Свет общий вкл/выкл (Дома?)
        lightHousesOnOff();
        break;

      case RAT:  //шлагбаум ШД1
        barrierUpDown();
        break;

      case V_UP:  //Стрелки круг +
        roundArrow();
        break;

      case V_DWN:  //Стрелки круг -
        roundArrow();
        break;

      case FAV:  //Стрелка в депо
        depoArrow();
        break;
    }
    Serial.println(value);
    receiver.resume();
  }
}

// Отправка данных
void sendStates() {
  Serial.flush();
  Serial3.flush();
  Serial.print("SENDING DATA: ");

  sendingData = "$" + String(worldState) + "&" + String(spd);
  Serial3.println(sendingData);
  Serial.println(sendingData);
  Serial.println("Data sended");
  Serial.flush();
  Serial3.flush();
}

// Отправить данные с сериал
void checkSerial() {
  while (Serial.available()) {
    String tempStr = Serial.readString();
    Serial.println(tempStr);
    Serial3.println(tempStr);
    Serial.flush();
  }
}