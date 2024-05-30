enum States {
  WIFI_CONFIGURED = 1,
  ADMIN_PASS = 2,
  USER_PASS = 4,
  AP_SSID = 8,
  AP_PASS = 16,
  AP_IP = 32,
  LAN_CONFIG = 64
};

// Чтение EEPROM
void readEEPROM() {
  // Чтение основных настроек
  Serial.println("Read states");
  readStates();

  if (states < 0) {
    states = 0;
  }

  if (states & WIFI_CONFIGURED) {
    // Чтение SSID
    Serial.println("Read ESID");
    readFromEEPROM(esidBlock, esid);

    // Чтение пароля WiFi
    Serial.println("Read WiFi pass");
    readFromEEPROM(epassBlock, epass);
  }

  if (states & ADMIN_PASS) {
    readAdminPass();
  }

  if (states & USER_PASS) {
    readUserPass();
  }

  if (states & AP_SSID) {
    readApsid();
  }

  if (states & AP_PASS) {
    readAppass();
  }

  // if (states & AP_IP)
  // {
  //   readAPip();
  // }
}

void writeLanConfig(IPAddress _ip, IPAddress _mask, IPAddress _gw, IPAddress _dns) {
  String adr = _ip.toString();
  writeToEEPROM(ip, adr);
  adr = _mask.toString();
  writeToEEPROM(mask, adr);
  adr = _gw.toString();
  writeToEEPROM(gw, adr);
  adr = _dns.toString();
  writeToEEPROM(dns, adr);

  states |= LAN_CONFIG;

  EEPROM.commit();
}

void ipToPartsString(String _ip, String &_ip1, String &_ip2, String &_ip3, String &_ip4){
  Serial.print("*  ipToPartsString, adress: ");
  Serial.println(_ip);

  int ipLength = _ip.length();
  int currentPart = 1;
  String part = "";

  for (int i = 0; i <= ipLength; i++) {
    if (_ip[i] == '.' || i == ipLength) {
      switch (currentPart) {
        case 1:
          _ip1 = part;
          break;
        case 2:
          _ip2 = part;
          break;
        case 3:
          _ip3 = part;
          break;
        case 4:
          _ip4 = part;
          break;
      }

      part = "";
      currentPart++;
    } else {
      part += _ip[i];
    }
  }
}

void ipToInt(String &_ip, int &_ip1, int &_ip2, int &_ip3, int &_ip4) {
  Serial.print("*   ipToInt, adress: ");
  Serial.println(_ip);


  int ipLength = _ip.length();
  int currentPart = 1;
  String part = "";

  for (int i = 0; i <= ipLength; i++) {
    if (_ip[i] == '.' || i == ipLength) {
      switch (currentPart) {
        case 1:
          _ip1 = part.toInt();
          break;
        case 2:
          _ip2 = part.toInt();
          break;
        case 3:
          _ip3 = part.toInt();
          break;
        case 4:
          _ip4 = part.toInt();
          break;
      }

      part = "";
      currentPart++;
    } else {
      part += _ip[i];
    }
  }
}

bool getLanConfig(IPAddress &_ip, IPAddress &_mask, IPAddress &_gw, IPAddress &_dns) {
  if (states & LAN_CONFIG) {
    String adr;
    readFromEEPROM(ip, adr);
    _ip.fromString(adr);
    ipToInt(adr, clientIp1, clientIp2, clientIp3, clientIp4);

    readFromEEPROM(mask, adr);
    _mask.fromString(adr);
    ipToInt(adr, clientMask1, clientMask2, clientMask3, clientMask4);

    readFromEEPROM(gw, adr);
    _gw.fromString(adr);
    ipToInt(adr, clientGw1, clientGw2, clientGw3, clientGw4);

    readFromEEPROM(dns, adr);
    _dns.fromString(adr);
    ipToInt(adr, clientDns1, clientDns2, clientDns3, clientDns4);

    int clientDns1;
    int clientDns2;
    int clientDns3;
    int clientDns4;

    return true;
  }
  return false;
}

void writeStates() {
  EEPROM.put(0, states);
  EEPROM.commit();
}

void readStates() {
  EEPROM.get(0, states);
}

void writeToEEPROM(int &block, String &str) {
  Serial.println("Writing to EEPROM:");
  Serial.print(str);
  Serial.println();

  int stringSize = str.length();
  EEPROM.write(block, stringSize);

  for (int i = 0; i < stringSize; i++) {
    EEPROM.write(block + 1 + i, str[i]);
  }

  EEPROM.commit();
  Serial.println("Write to eeprom COMPLITED!");
  Serial.println();
}

void readFromEEPROM(int &block, String &str) {
  Serial.println("Read from EEPROM");
  Serial.println();

  str = "";
  int stringSize = EEPROM.read(block);

  for (int i = 0; i < stringSize; i++) {
    str += char(EEPROM.read(block + 1 + i));
  }

  Serial.println("Read from eeprom COMPLITED!");
  Serial.println();
  Serial.println(str);
  Serial.println();

  showHomePage();
}

// очистка EEPROM
void clearEEPROM() {
  Serial.println("clearing eeprom");
  for (int i = 0; i < eepromSize; ++i) {
    EEPROM.write(i, 0);
  }
  delay(10);
  EEPROM.commit();
  ESP.reset();
}

void writeEsid() {
  Serial.println("Write esid");
  writeToEEPROM(esidBlock, esid);
}

void writeEpass() {
  Serial.println("Write WiFi pass");
  writeToEEPROM(epassBlock, epass);
}

void writeAdminPass() {
  Serial.println("Write admin pass");
  writeToEEPROM(adminPassBlock, adminPass);
  states |= ADMIN_PASS;
  writeStates();
}

void readAdminPass() {
  Serial.println("Read admin pass");
  readFromEEPROM(adminPassBlock, adminPass);
}

void writeUserPass() {
  Serial.print("Write user pass: ");
  Serial.println(userPass);

  writeToEEPROM(userPassBlock, userPass);

  states |= USER_PASS;
  writeStates();
}

void readUserPass() {
  Serial.println("Read user pass: ");
  readFromEEPROM(userPassBlock, userPass);
  Serial.println(userPass);
}

void writeApsid() {
  Serial.println("Write AP ssid");
  writeToEEPROM(apsidBlock, apsid);
}

void readApsid() {
  Serial.println("Read AP ssid");
  String tempStr = "";
  readFromEEPROM(apsidBlock, tempStr);
  if(tempStr.length() > 0 && tempStr.length() < 16)
  {
    apsid = tempStr;
  }
}

void writeAppass() {
  Serial.println("Write AP pass");
  writeToEEPROM(appassBlock, appass);
}

void readAppass() {
  Serial.println("Read AP pass");
  String tempStr = "";
  readFromEEPROM(appassBlock, tempStr);
  if(tempStr.length() > 0 && tempStr.length() < 16)
  {
    appass = tempStr;
  }
}

// void writeAPip()
// {
//   Serial.println("Write AP ip adress");

//   EEPROM.write(apip1Block, apip1);
//   EEPROM.write(apip2Block, apip2);
//   EEPROM.write(apip3Block, apip3);
//   EEPROM.write(apip4Block, apip4);

//   EEPROM.commit();
// }

// void readAPip()
// {
//   Serial.println("Read AP ip adress");

//   apip1Block = EEPROM.read(apip1);
//   apip2Block = EEPROM.read(apip2);
//   apip3Block = EEPROM.read(apip3);
//   apip4Block = EEPROM.read(apip4);
//   apip = (apip1, apip2, apip3, apip4);
// }
