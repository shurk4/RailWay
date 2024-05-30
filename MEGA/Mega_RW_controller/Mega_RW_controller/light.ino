// Свет в домах
void lightHousesOnOff() {
  if (pwrs[1] == 1) {
    digitalWrite(ext1, LOW);
    digitalWrite(ext2, LOW);
    digitalWrite(ext3, LOW);
    digitalWrite(ext4, LOW);
    digitalWrite(ext5, LOW);
    digitalWrite(ext6, LOW);
    pwrs[1] = 0;
    worldState &= (~HOUSE1_LIGHT);
    worldState &= (~HOUSE2_LIGHT);
  } else {
    digitalWrite(ext1, HIGH);
    digitalWrite(ext2, HIGH);
    digitalWrite(ext3, HIGH);
    digitalWrite(ext4, HIGH);
    digitalWrite(ext5, HIGH);
    digitalWrite(ext6, HIGH);
    pwrs[1] = 1;
    worldState |= HOUSE1_LIGHT;
    worldState |= HOUSE2_LIGHT;
  }
}

// Фонари вкл(плавное включение)
void lightOutUp() {
  worldState |= STREET_LIGHT;
  digitalWrite(light1, HIGH);
  digitalWrite(light2, LOW);
  lgt = 90;
  analogWrite(lightPWM, lgt);
  delay(100);
  lgt = 110;
  analogWrite(lightPWM, lgt);
  delay(100);
  lgt = 130;
  analogWrite(lightPWM, lgt);
  delay(100);
  lgt = 150;
  analogWrite(lightPWM, lgt);
  delay(100);
  lgt = 255;
  analogWrite(lightPWM, lgt);
}

// Фонари выкл(плавное выключение)
void lightOutDown() {
  worldState &= (~STREET_LIGHT);
  digitalWrite(light1, HIGH);
  digitalWrite(light2, LOW);
  lgt = 150;
  analogWrite(lightPWM, lgt);
  delay(100);
  lgt = 130;
  analogWrite(lightPWM, lgt);
  delay(100);
  lgt = 110;
  analogWrite(lightPWM, lgt);
  delay(100);
  lgt = 90;
  analogWrite(lightPWM, lgt);
  delay(100);
  lgt = 0;
  analogWrite(lightPWM, lgt);
}