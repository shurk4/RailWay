// Шлакбаум
void barrierUpDown() {
  if (shlag) {
    motor1.step(-4500);
    worldState &= (~BARRIER_OPEN);
    shlag = false;
  } else {
    motor1.step(4500);
    shlag = true;
    worldState |= BARRIER_OPEN;
  }
}

// Стрелки кругов
void roundArrow() {
  servo1.attach(servopin1);
  servo2.attach(servopin2);
  servo1.write(arrowRonds ? 0 : 60);
  servo2.write(arrowRonds ? 0 : 50);
  delay(300);
  servo1.detach();
  servo2.detach();
  servo3.detach();
  if (worldState & BIG_ROUND) {
    worldState &= (~BIG_ROUND);
  }
  else {
    worldState |= BIG_ROUND;
  }
  Serial.print("Mega: Arrow round = ");

  if (worldState & BIG_ROUND) {
    Serial.println("Big round");
  } else {
    Serial.println("Small round");
  }
}

// стрелка депо
void depoArrow() {
  servo3.attach(servopin3);
  servo3.write(arrowDepo == 0 ? 80 : 255);
  delay(500);  // !!! Зачем задержка ???
  arrowDepo = !arrowDepo;
  if (arrowDepo) {
    worldState |= DEPO_ARROW;
  } else {
    worldState &= (~DEPO_ARROW);
  }
  servo1.detach();
  servo2.detach();
  servo3.detach();
}