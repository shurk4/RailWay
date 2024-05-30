#include <IRremote.h>
#include <Stepper.h>
#include <Servo.h>

enum DIRECTION {
  TO_LEFT,
  TO_RIGHT
};

enum STATES {
  TRAIN_STOPPED = 1,
  TRAIN_MOVING_LEFT = 2,
  TRAIN_MOVING_RIGHT = 4,
  BARRIER_OPEN = 8,
  DEPO_ARROW = 16,
  BIG_ROUND = 32,
  STREET_LIGHT = 64,
  HOUSE1_LIGHT = 128,
  HOUSE2_LIGHT = 256
};

int worldState = 0;

// Команда принимаемая через сериал
String command = "";
// Принята команда или просто текст.
bool isCommand = false;

String sendingData = "";

// Для таймера прерывания(Мигаем диодом)
#define PIN_LED 13  // вывод светодиода
static uint32_t timer;
int ledDelay = 1000;

static uint32_t pingTimer;
int pingDelay = 9000;
bool isConnected = false;

//пины
int receiver_pin = 38;
//int power = 2; //питание
int motor11 = 34;   //мотор 1 контакт 1
int motor12 = 36;   //мотор 1 контакт 2
int motor1spd = 8;  //мотор 1 шим(скорость)
int light1 = 30;    //свет1
int light2 = 32;    //свет2
int lightPWM = 11;  //свет шим
int step11 = 12;    //шд 1 1
int step12 = 13;    //шд 1 2
int step21 = 3;     //шд 2 1
int step22 = 4;     //шд 2 2
int servopin1 = 7;  //Серво 1 - стрелка
int servopin2 = 6;  //Серво 2
int servopin3 = 5;  //Серво 3
int ext1 = 2;
int ext2 = 44;
int ext3 = 45;
int ext4 = 46;
int ext5 = 22;
int ext6 = 24;
int hall1 = 53;
int hall2 = 51;
int hall3 = 49;

//Объекты
Stepper motor1(32, step11, step12);
Stepper motor2(32, step21, step22);
Stepper motor3(32, step21, step22);
Servo servo1;
Servo servo2;
Servo servo3;

//переменные
int pwrs[] = { 0, 0, 0, 0 };  //переменная состояния питания и подсветки
int spd = 0;                  //переменная скорости двигателя
int spdgl = 0;                //Переменная направления движения
bool lgt = false;             //переменная света
bool arrowRonds = false;      //переменная стрелка
bool arrowDepo = false;       //переменная стрелка в депо
bool state = 0;               //переменная холла 1
bool shlag = 0;               //переменная шлагбаума

IRrecv receiver(receiver_pin);
decode_results output;

// Перечисление кодов пульта
#define POW 0x20DF10EF     //питание
#define OK 0x20DF22DD      //стоп
#define RIGHT 0x20DF609F   //движение вперёд
#define UP 0x20DF02FD      //скорость +
#define LEFT 0x20DFE01F    //движение назад
#define DWN 0x20DF827D     //скорость -
#define CH_UP 0x20DF00FF   //Свет вкл
#define CH_DWN 0x20DF807F  //Свет выкл
#define V_UP 0x20DF40BF    //Стрелка +
#define V_DWN 0x20DFC03F   //Стрелка -
#define FAV 0x20DF7887     //Стрелка в депо
#define RAT 0x20DF9E61     //Шлагбаум


void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial3.setTimeout(5);
  receiver.enableIRIn();
  // pinMode(power, OUTPUT);
  pinMode(motor11, OUTPUT);
  pinMode(motor12, OUTPUT);
  pinMode(motor1spd, OUTPUT);
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);
  pinMode(lightPWM, OUTPUT);
  pinMode(ext1, OUTPUT);
  pinMode(ext2, OUTPUT);
  pinMode(ext3, OUTPUT);
  pinMode(ext4, OUTPUT);
  pinMode(ext5, OUTPUT);
  pinMode(ext6, OUTPUT);

  pinMode(step11, OUTPUT);
  pinMode(step12, OUTPUT);
  pinMode(step21, OUTPUT);
  pinMode(step22, OUTPUT);

  motor1.setSpeed(3000);
  motor2.setSpeed(7000);

  //  servo1.attach(servopin1);
  //  servo2.attach(servopin2);
  //  servo3.attach(servopin3);

  pinMode(hall1, INPUT);
  pinMode(hall2, INPUT);
  pinMode(hall3, INPUT);

  initialize();
}

void loop() {
  if (millis() - timer >= ledDelay) {
    timer = millis();
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
  }

  checkIRCommand();
  checkWiFiCommand();
  checkSerial();

  // if (millis() - pingTimer >= pingDelay) {
  //   pingTimer = millis();
  //   isConnected = false;
  //   sendStates();
  // }
}