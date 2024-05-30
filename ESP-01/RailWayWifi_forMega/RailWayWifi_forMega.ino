#include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Для ЖД
WiFiClient client;
// Буфер
const int bufSize = 30;  // ВАЖНО!!! МАКСИМАЛЬНЫЙ РАЗМЕР ПАКЕТА ПОЛУЧАЕМЫХ ДАННЫХ В ВИДЕ: юнит,значение
char buf[bufSize];

int IO0 = 0;
int IO3 = 3;  // Пин RX

// // Вэб страницы
// void homePage();
// void returnHome();
// void settingsPage();
// void resetSettings();
// void returnSettings();
// void scanWiFiPage();
// void restartPage();
// void applySettingsPage();
// void onPage();
// void offPage();

String moduleName = "Железная дорога";
String pageTitle = moduleName;

const String userName = "user";
String userPass = "user";

const String adminName = "admin";
String adminPass = "admin";

//EEPROM config
int eepromSize = 512;
//Параметры
int states = 0;  // Хранит основные параметры перечисленные в enum states

int esidBlock = 2;
//int esidMaxSize = 32; // +1 байт на размер строки

int epassBlock = 35;
//int epassMaxSize = 63; // +1 байт на размер строки

//int paramSize = 16; // размер строковых параметров
int adminPassBlock = 99;
int userPassBlock = 115;
int apsidBlock = 132;
int appassBlock = 148;
int apip1Block = 164;
int apip2Block = 165;
int apip3Block = 166;
int apip4Block = 167;
int ip = 164;
int mask = 180;
int gw = 196;
int dns = 212;

int clientIp1;
int clientIp2;
int clientIp3;
int clientIp4;

int clientMask1;
int clientMask2;
int clientMask3;
int clientMask4;

int clientGw1;
int clientGw2;
int clientGw3;
int clientGw4;

int clientDns1;
int clientDns2;
int clientDns3;
int clientDns4;

/*
  * 164-179 - ip строка
 * 180-195 - mask
 * 196-211 - gw
 * 212-227 - dns
 */
//EEPROM config

// WiFi config
String esid = "";
String epass = "";

String apsid = "8266wifi";
String appass = "8266wifi";
IPAddress apip;
IPAddress apnet;

int wifiNum;          // Количество найденных сетей WiFi
String wifiSsidList;  // Строка для списка WiFi сетей

String content;  // Строка HTML для отправки клиенту

bool apMode = false;
bool connected = false;

//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);

//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);
WiFiServer tcpServer(2024);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IO0, OUTPUT);
  pinMode(IO3, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // Если включать плюсом - сделать LOW
  digitalWrite(IO0, HIGH);

  //Подключение debug if(DEBUG)Serial Monitor
  Serial.begin(115200);
  Serial.setTimeout(5);
  // Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY); // Запуск последовательного порта в режиме передачи, освобождение пина RX IO3
  delay(2000);

  //Подключение EEPROM
  EEPROM.begin(eepromSize);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.println("Startup");

  readEEPROM();

  launchWeb();

  digitalWrite(IO3, HIGH);  // Включаем питание на пине RX IO3 - включение модуля реле
  tcpServer.begin();
}

void loop() {
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.println("Disconnected! Trying connect to SSID: " + esid);
    wifiConnect();
  }

  if (!client) {
    client = tcpServer.available();
    if (client) {
      Serial.println("Connected");
      Serial.print("&9");
    }
  }

  checkTcpClient();
  checkSerial();
  server.handleClient();
}
