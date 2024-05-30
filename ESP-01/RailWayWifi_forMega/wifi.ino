// Алгоритм работы:
// Если это первое включение (configured = false), включается точка доступа с паролем 8266wifi и ожидает настройки в веб интерфейсе
// После применения настроек в EEPROM сохраняются логин, пароль и параметр сообщающий о том, что устройство настроено(configured = true)(перечисление кодов настроек указано в основном файле).
// Затем устройство перезапускается, подключается к указаной сети и ждёт запросов в веб интерфейсе

// Если при включении нет связи с установленой сетью, включается режим точки доступа и производится сканирование доступных сетей.
// Если указанная в настройках сеть стала доступна, подключается к ней.

// Если сеть пропала во время работы устройства, начинается попытка подключиться

//Проверка соединения
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 30 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  return false;
}

void setAvailibleNetworks()
{
  wifiSsidList = "";

  String htmlCode("<div class=\"ssid\">"
                  " <input type=\"radio\" id=\"ssid_1\" name=\"ssid\" value=\"ssid_name_1\"><label for=\"ssid_1\">SSID Name 1</label>"
                  " </div>"
                 );

  for (int i = 0; i < wifiNum; ++i)
  {
    // Print SSID and RSSI for each network found
    wifiSsidList += "<div class=\"ssid\">"
                    "<input type=\"radio\" id=\"ssid_";
    wifiSsidList += '0' + i;
    wifiSsidList += "\" name=\"ssid\" value=\"";
    wifiSsidList += WiFi.SSID(i);
    wifiSsidList += "\"><label for=\"ssid_";
    wifiSsidList += '0' + i;
    wifiSsidList += "\">";
    wifiSsidList += WiFi.SSID(i);
    wifiSsidList += " (";
    wifiSsidList += WiFi.RSSI(i);
    wifiSsidList += ") - ";
    wifiSsidList += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "not secured" : "secured";
    wifiSsidList += "</label></div>";
  };
}

void scanWiFi()
{
  Serial.println("scan wifi");
  // Сканируем доступные сети
  wifiNum = WiFi.scanNetworks();
  Serial.println("scan done");
}

// Создание точки доступа
void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  scanWiFi();

  // если сетей нет, сообщаем об этом. Если сети нашлись, выводим в терминал список
  if (wifiNum == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(wifiNum);
    Serial.println(" networks found");
    for (int i = 0; i < wifiNum; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }

  Serial.println("");
  setAvailibleNetworks();

  delay(100);
  WiFi.softAP(apsid, appass);
  
  // WiFi.softAPConfig(apip, apip, apnet);
  apMode = true;
  Serial.println("softap MODE");
  Serial.print("AP SSID: ");
  Serial.println(apsid);
  Serial.print("AP Pass: ");
  Serial.println(appass);
  Serial.println(WiFi.softAPIP());
  Serial.println("Waiting for configuration");
}

void setClientConfig()
{
  Serial.println("Configuring client");
  IPAddress ipAdr;
  IPAddress subnetAdr;
  IPAddress gatewayAdr;
  IPAddress dnsAdr;

  if(getLanConfig(ipAdr, gatewayAdr, subnetAdr, dnsAdr))
  {
    Serial.println("Configured!");
    WiFi.config(ipAdr, gatewayAdr, subnetAdr, dnsAdr);

    Serial.print("ip: ");
    Serial.println(ipAdr.toString());
    Serial.print("mask: ");
    Serial.println(gatewayAdr.toString());
    Serial.print("gw: ");
    Serial.println(subnetAdr.toString());
    Serial.print("dns: ");
    Serial.println(dnsAdr.toString());
  }
  else
  {
    Serial.println("Not configured, default settings aplied");
  }  
}

void wifiConnect()
{
  Serial.print("Connecting to WiFi - ");
  Serial.println(esid.c_str());
  WiFi.disconnect();
  WiFi.begin(esid.c_str(), epass.c_str());

  setClientConfig();

  Serial.println("Saved WiFi settings:");
  Serial.println(esid);
  Serial.println(epass);

  if (testWifi())
  {
    Serial.println("Succesfully Connected!!!");
    connected = true;
    apMode = false;
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    return;
  }
  else
  {
    Serial.println("Couldn't connect to Wi-Fi");
  }

  if (!connected)
  {
    Serial.println("Turning the HotSpot On");
    setupAP();// Setup HotSpot

    while ((WiFi.status() != WL_CONNECTED))
    {
      Serial.print(".");
      delay(1000);
      server.handleClient();

      if (WiFi.status() == WL_CONNECT_FAILED)
      {
        Serial.println("Wrong password, WiFi config reset");
        states &= (~WIFI_CONFIGURED);
        writeStates();
        ESP.reset();
      }
      
      if (states & WIFI_CONFIGURED)
      {
        scanWiFi();

        for (int i = 0; i < wifiNum; i++)
        {
          if (WiFi.SSID(i) == esid.c_str())
          {
            Serial.println("WiFi founded, restarting!");
            ESP.reset();
          }
        }
      }
    }
  }

  Serial.println();
  Serial.println("Waiting.");
}
