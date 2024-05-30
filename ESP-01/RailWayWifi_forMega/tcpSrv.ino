void checkTcpClient() {
  while (client.available() > 0)  // ??? =)
  {
    char c = client.read();
    Serial.print(c);
  }
  server.handleClient();

  // buf[0] = NULL;
}

void checkSerial() {
  // WiFiClient client = tcpServer.available();
 bool sa = false;
  while (Serial.available()) {
    client.print(Serial.readString());
  }
}