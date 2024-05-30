// Запуск веб сервера


bool is_authenticated() {
  Serial.println("Enter is_authenticated");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("name=admin") != -1) {
      Serial.println("Authentication Successful as admin");
      return true;
    } else if (cookie.indexOf("name=user") != -1) {
      Serial.println("Authentication Successful as user");
      return true;
    }
  }
  Serial.println("Authentication Failed");
  return false;
}

void launchWeb() {
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());

  createWebServer();
  // Команда серверу собирать указанные хедеры
  server.collectHeaders("User-Agent", "Cookie");
  // Start the server
  server.begin();
  Serial.println("Server started");
}

// Заполнение  веб сервера
void createWebServer() {
  server.on("/", homePage);
  // server.on("/", handleRoot);
  server.on("/login", loginPage);
  server.on("/settings", settingsPage);
  server.on("/scan", scanWiFiPage);
  server.on("/restart", restartPage);
  server.on("/setWiFi", setWiFiPage);
  server.on("/setClientIp", setClientIpPage);
  server.on("/setAPsid", setAPSidPage);
  server.on("/setAPPass", setAPPassPage);
  server.on("/resetAPsid", resetAPSidPage);
  server.on("/resetAPPass", resetAPPassPage);
  server.on("/reset", resetSettings);

  server.on("/door1", door1);
  server.on("/door2", door2);

  server.on("/setAdminPass", setAdminPass);
  server.on("/setUserPass", setUserPass);

  server.onNotFound(notFoundPage);
}

//--------------- Действия с контроллером -----------------------
void door1() {
  if (is_authenticated()) {
    Serial.write("Open door 1");
    digitalWrite(LED_BUILTIN, LOW);  // Если включать плюсом сделать HIGH
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.write("Door 1 is open");
    returnHome();
  } else {
    toLoginPage();
  }
}

void door2() {
  if (is_authenticated()) {
    Serial.write("Open door 2");
    digitalWrite(IO0, LOW);  // Если включать плюсом сделать HIGH
    delay(1000);
    digitalWrite(IO0, HIGH);
    Serial.write("Door 2 is open");
    returnHome();
  } else {
    toLoginPage();
  }
}

//--------------- Страницы сервера -----------------------
void setClientIpPage() {
  if (is_authenticated()) {
    Serial.println("Getting Client ip adress");

    int ip1 = server.arg("ip1").toInt();
    int ip2 = server.arg("ip2").toInt();
    int ip3 = server.arg("ip3").toInt();
    int ip4 = server.arg("ip4").toInt();

    Serial.print(ip1);
    Serial.print(".");
    Serial.print(ip2);
    Serial.print(".");
    Serial.print(ip3);
    Serial.print(".");
    Serial.print(ip4);
    Serial.println();

    IPAddress ipAdr(ip1, ip2, ip3, ip4);
    Serial.println(ipAdr.toString());

    Serial.println("Getting Client mask");

    ip1 = server.arg("mask1").toInt();
    ip2 = server.arg("mask2").toInt();
    ip3 = server.arg("mask3").toInt();
    ip4 = server.arg("mask4").toInt();

    Serial.print(ip1);
    Serial.print(".");
    Serial.print(ip2);
    Serial.print(".");
    Serial.print(ip3);
    Serial.print(".");
    Serial.print(ip4);
    Serial.println();

    IPAddress maskAdr(ip1, ip2, ip3, ip4);
    Serial.println(maskAdr.toString());

    Serial.println("Getting Client gateway");

    ip1 = server.arg("gw1").toInt();
    ip2 = server.arg("gw2").toInt();
    ip3 = server.arg("gw3").toInt();
    ip4 = server.arg("gw4").toInt();

    Serial.print(ip1);
    Serial.print(".");
    Serial.print(ip2);
    Serial.print(".");
    Serial.print(ip3);
    Serial.print(".");
    Serial.print(ip4);
    Serial.println();

    IPAddress gwAdr(ip1, ip2, ip3, ip4);
    Serial.println(gwAdr.toString());

    Serial.println("Getting Client DNS");

    ip1 = server.arg("dns1").toInt();
    ip2 = server.arg("dns2").toInt();
    ip3 = server.arg("dns3").toInt();
    ip4 = server.arg("dns4").toInt();

    Serial.print(ip1);
    Serial.print(".");
    Serial.print(ip2);
    Serial.print(".");
    Serial.print(ip3);
    Serial.print(".");
    Serial.print(ip4);
    Serial.println();

    IPAddress dnsAdr(ip1, ip2, ip3, ip4);
    Serial.println(dnsAdr.toString());

    if (IPAddress::isValid(ipAdr.toString()) && IPAddress::isValid(maskAdr.toString()) && IPAddress::isValid(gwAdr.toString()) && IPAddress::isValid(dnsAdr.toString())) {
      Serial.println("Config is valid!");
      writeLanConfig(ipAdr, maskAdr, gwAdr, dnsAdr);
      setClientConfig();
      toNewIp(ipAdr);
    } else {
      Serial.println("Config is NOT valid!");
    }

    states |= AP_IP;
    writeStates();
    if (apMode) {
      toNewIp(dnsAdr);
    } else {
      returnSettings();
    }
  } else {
    toLoginPage();
  }
}

void toLoginPage() {
  server.sendHeader("Location", "/login");
  server.sendHeader("Cache-Control", "no-cache");
  server.send(301);
}

void homePage() {
  Serial.println("Home page is opened");
  String header;

  if (!is_authenticated()) {
    toLoginPage();
    return;
  }
  showHomePage();
}

void loginPage() {
  bool wrongPasswd = false;
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }

  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "name=0");
    server.send(301);
    return;
  }

  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == adminName && server.arg("PASSWORD") == adminPass) {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "name=admin; max-age=2592000");  //2 592 000 сек. - месяц
      // Посмотреть какие можно установить флаги
      server.send(301);
      Serial.println(adminName);
      Serial.println("Log in Successful");
      return;
    }

    if (server.arg("USERNAME") == userName && server.arg("PASSWORD") == userPass && userPass != "user") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "name=user; max-age=2592000");  //2 592 000 сек. - месяц
      // Посмотреть какие можно установить флаги
      server.send(301);
      Serial.println(userName);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    wrongPasswd = true;
    Serial.println("Log in Failed");
  }
  showLoginPage(wrongPasswd);
}

void returnHome()  // Возвращает на домашнюю страницу
{
  content = "<meta http-equiv=\"Refresh\" content=\"1; URL=/\">";
  server.send(200, "text/html", content);
}

void settingsPage() {

  if (is_authenticated()) {
    Serial.println("Settings page is opened");
    showSettingsPage();
  } else {
    toLoginPage();
  }
}

void resetSettings() {
  if (is_authenticated()) {
    clearEEPROM();
    restartPage();
  } else {
    toLoginPage();
  }
}

void returnSettings()  // Возвращает на страницу settings
{
  content = "<meta http-equiv=\"Refresh\" content=\"1; URL=/settings\">";
  server.send(200, "text/html", content);
}

void setWiFiPage() {
  if (is_authenticated()) {
    esid = server.arg("ssid");
    epass = server.arg("pass");
    states |= WIFI_CONFIGURED;
    writeStates();

    writeEsid();
    writeEpass();

    ESP.reset();
  } else {
    toLoginPage();
  }
}

void setAPSidPage() {
  if (is_authenticated()) {
    Serial.println("Write AP SSID");
    apsid = server.arg("ssid");
    states |= AP_SSID;
    writeStates();

    writeToEEPROM(apsidBlock, apsid);
    returnSettings();
  } else {
    toLoginPage();
  }
}

void setAPPassPage() {
  if (is_authenticated()) {
    Serial.println("Write AP password");
    appass = server.arg("pass");
    states |= AP_PASS;
    writeStates();

    writeToEEPROM(appassBlock, appass);
    returnSettings();
  } else {
    toLoginPage();
  }
}

void resetAPSidPage() {
  if (is_authenticated()) {
    Serial.println("Reset AP SSID");
    apsid = "8266wifi";
    states &= (~AP_SSID);
    writeStates();

    writeToEEPROM(apsidBlock, apsid);
    returnSettings();
  } else {
    toLoginPage();
  }
}

void resetAPPassPage() {
  if (is_authenticated()) {
    Serial.println("Reset AP password");
    appass = "8266wifi";
    states &= (~AP_PASS);
    writeStates();

    writeToEEPROM(appassBlock, appass);
    returnSettings();
  } else {
    toLoginPage();
  }
}

void toNewIp(IPAddress adr)  // Возвращает на страницу settings
{
  content = "<meta http-equiv=\"Refresh\" content=\"1; URL=http://" + adr.toString() + "/settings\">";
  server.send(200, "text/html", content);
}

void scanWiFiPage() {
  if (is_authenticated()) {
    IPAddress ip = WiFi.softAPIP();

    scanWiFi();
    setAvailibleNetworks();

    returnSettings();
  } else {
    toLoginPage();
  }
  // homePage();
}

void restartPage() {
  if (is_authenticated()) {
    returnHome();
    delay(100);
    Serial.println("Device rebooting...");
    ESP.reset();
  } else {
    toLoginPage();
  }
}

void notFoundPage() {
  if (is_authenticated()) {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
  } else {
    toLoginPage();
  }
}

void setAdminPass() {
  if (is_authenticated()) {
    adminPass = server.arg("pass");
    writeAdminPass();
    settingsPage();
  } else {
    toLoginPage();
  }
}

void setUserPass() {
  if (is_authenticated()) {
    userPass = server.arg("pass");
    Serial.print("Change userpass from web. New pass: ");
    Serial.println(userPass);
    writeUserPass();
    settingsPage();
  } else {
    toLoginPage();
  }
}
