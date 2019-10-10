

void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, Ssid);
  EEPROM.get(0 + sizeof(Ssid), Password);
  EEPROM.get(0 + sizeof(Ssid) + sizeof(Password), VerificationCode);
  char ok[2 + 1];
  EEPROM.get(0 + sizeof(Ssid) + sizeof(Password)+ sizeof(VerificationCode), ok);
  EEPROM.end();
  
  if (String(ok) != String("OK")) {
    Ssid[0] = 0;
    Password[0] = 0;
  }
  
  if(strlen(Ssid) > 0)
  {      
  }
  else
  {
    Ssid[0] = 0;
    Password[0] = 0;
  }
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, Ssid);
  EEPROM.put(0 + sizeof(Ssid), Password);
  if(strlen(VerificationCode) > 0)
  {   
    EEPROM.put(0 + sizeof(Ssid) +  sizeof(Password), VerificationCode);
  }
  char ok[2 + 1] = "OK";
  EEPROM.put(0 + sizeof(Ssid) + sizeof(Password) + sizeof(VerificationCode), ok);
  EEPROM.commit();
  EEPROM.end();
}

void clearCredentials() {
  char Ssid_d[32] = "";
  char Password_d[32] = "";
  EEPROM.begin(512);
  EEPROM.put(0, Ssid_d);
  EEPROM.put(0 + sizeof(Ssid_d), Password_d);
  char ok[2 + 1] = "";
  EEPROM.put(0 + sizeof(Ssid_d) + sizeof(Password_d), ok);
  EEPROM.commit();
  EEPROM.end();
  for(byte index = 0; index < 32; index++)
  {
    Ssid[index] = 0;
    Password[index] = 0;
  }
}

bool ConnectWifi()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(Ssid, Password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    return false;
  }
  else
  {
    ConnectServer();
    return true;
  }
}

void ConnectServer()
{
  if (WiFi.status() == WL_CONNECTED)
  {
     if (client.connect(Host, Port))
     {
       client.println("HelloFromCreXDevice");
       client.println(WiFi.macAddress());
       client.println(VerificationCode);
     }
     else
     {
       //Serial.println("ServerErrorConnected");
     }
  }
}

void CheckConnectWifiAndServer()
{
  if(WiFi.status() == WL_CONNECTED && IsSetupAp == false)
  {
    if (!client.connected())
    {
      ConnectServer();
    }
  }
  if(WiFi.status() != WL_CONNECTED && IsSetupAp == false)
  {
    IsConnect = false;
  }
}

void ReconnectWiFi()
{
  ConnectWifi();
}

void SetupAPWifi()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(SoftAP_Ssid, SoftAP_Password);
  delay(400);

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  
  dnsServer.start(DNS_PORT, "*", apIP);

  if (!MDNS.begin(MyHostname))
  {

  }
  else
  {
    MDNS.addService("http", "tcp", 80);
  }
  
  if(WiFi.status() == WL_CONNECTED)
  {
    MDNS.update();
  }

  ArduinoOTA.begin();
}

void WifiModeConnect()
{
  CheckConnectWifiAndServer();
  if(IsConnect == true)
  {
    return;
  }
  if(strlen(Ssid) > 0 && IsSetupAp == false)
  {
    if(ConnectWifi())
    {
      IsConnect = true;
    }
  }
  else
  {
    SetupAPWifi();
    IsConnect = true;
  }
}

void ForwardDataToSerial()
{
  if(!client.connected())
  {
    return;
  }

  while (client.available())
  {
    char inChar = (char)client.read();
    Serial.print(inChar);  
  }
  /*while (client.available())
  {
    char inChar = (char)client.read();

    if (inChar == '\n')
    {
      isStringClientComplete = true;
      break;
    }

    if (inChar != '\r')
    {
      receiveClientString += inChar;
    }
  }

  if(isStringClientComplete)
  {
    Serial.println(receiveClientString);
    isStringClientComplete = true;
    receiveClientString = "";
  }*/
}

void ForwardDataToClient()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    client.print(inChar);  
  }
  /*while (Serial.available())
  {
    char inChar = (char)Serial.read();

    if (inChar == '\n')
    {
      isStringSerialComplete = true;
      break;
    }

    if (inChar != '\r')
    {
      receiveSerialString += inChar;
    }
  }

  if(isStringSerialComplete && client.connected())
  {
    
    client.println(receiveSerialString);  
    isStringSerialComplete = true;
    receiveSerialString = "";
  }*/
}
