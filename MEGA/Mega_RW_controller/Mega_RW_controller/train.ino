// Запустить поезд по направлению из перечисления DIRECTION::TO_LEFT / DIRECTION::TO_RIGTH
void trainMove(int dir) {
  spd = 0;
  analogWrite(motor1spd, spd);
  delay(50);

  if (dir == TO_RIGHT) {
    digitalWrite(motor11, LOW);
    digitalWrite(motor12, HIGH);
    worldState &= (~TRAIN_STOPPED);
    worldState &= (~TRAIN_MOVING_LEFT);
    worldState |= TRAIN_MOVING_RIGHT;
  } else {
    digitalWrite(motor11, HIGH);
    digitalWrite(motor12, LOW);
    worldState &= (~TRAIN_STOPPED);
    worldState &= (~TRAIN_MOVING_RIGHT);
    worldState |= TRAIN_MOVING_LEFT;
  }
  spd = 100;
  analogWrite(motor1spd, spd);
  spdgl = (dir == TO_RIGHT ? 0 : 1);
}

// Скорость поезда +
void trainSpeedUp() {
  if (spd < 255) {
    spd += 5;
    //spd = constrain(spd, 0, 254); //ограничение минимальной максимальной скорости
    analogWrite(motor1spd, spd);
  } else {
    spd = 255;
    analogWrite(motor1spd, spd);
  }
}

// Скорость поезда -
void trainSpeedDown() {
  if (spd > 0) {
    spd -= 5;
    //spd = constrain(spd, 0, 254); //ограничение минимальной максимальной скорости
  } else {
    spd = 0;
  }
  trainSetSpeed(spd);
}

// Установить скорость поезда
void trainSetSpeed(int speed) {
  if (speed == 0) {
    worldState |= TRAIN_STOPPED;
    Serial.println("stopped");
  } else {
    worldState &= (~TRAIN_STOPPED);
  }
  spd = speed;
  analogWrite(motor1spd, spd);
}