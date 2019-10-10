
boolean captivePortal()
{
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(MyHostname) + ".local"))
  {
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");
    return true;
  }
  return false;
}

/** Wifi config page handler */
void handleWifi()
{
  //if (!server.authenticate(www_username, www_password))
  //{
  //   return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
  //}
  
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  String Page;
  Page += F(
  "<html><head><title>CreX Device Config</title></head><body>"
  "<div class='container-fluid'>"
  "<div class='row'>"
  "<div class='col-md-12'>"
  "<h1>Crex Device Config</h1>");
  
  Page += String(F("<p>You are connected through the soft AP: ")) + SoftAP_Ssid + F("</p>");
  
  Page +=
    String(F(
             "\r\n<br />"
             "<table><tr><th align='left'>SoftAP Config</th></tr>"
             "<tr><td>SSID: ")) +
    String(SoftAP_Ssid) +
    F("</td></tr>"
      "<tr><td>IP: ") +
    toStringIp(WiFi.softAPIP()) +
    F("</td></tr>"
      "</table>"
      "\r\n<br />"
      "<table><tr><form method='POST' action='wifisave'><th align='left'>Aerification config</th></tr>"
      "<tr><td>Mac Address: ") +
    String(WiFi.macAddress()) +
    F("</td></tr>"
      "<tr><td>"
      "<input type='text' placeholder='") +
      String((strlen(VerificationCode) > 0)?  VerificationCode: "Verification Code") +
      F("' name='t'/>"
      "</td></tr>"
      "</table>"
      "\r\n<br />"
      "<table><tr><th align='left'>WLAN list</th></tr>");
  int n = WiFi.scanNetworks();
  if (n > 0)
  {
    for (int i = 0; i < n; i++)
    {
      Page += String(F("\r\n<tr><td>SSID ")) + WiFi.SSID(i) + ((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? F(" ") : F(" *")) + F(" (") + WiFi.RSSI(i) + F(")</td></tr>");
    }
  }
  else
  {
    Page += F("<tr><td>No WLAN found</td></tr>");
  }
  
  Page += F(
            "</table>"
            "\r\n<br /><form method='POST' action='wifisave'><h3>Connect to network:</h3>"
            "<input type='text' placeholder='network' name='n'/>"
            "<br /><input type='password' placeholder='password' name='p'/>"
            "<br /><input type='submit' value='Connect/Disconnect'/></form>"
            "</div></div></div>"
            "</body></html>");
  server.send(200, "text/html", Page);
  server.client().stop();
}

void handleWifiSave()
{
  server.arg("t").toCharArray(VerificationCode, sizeof(VerificationCode) - 1);
  server.arg("n").toCharArray(Ssid, sizeof(Ssid) - 1);
  server.arg("p").toCharArray(Password, sizeof(Password) - 1);
  server.sendHeader("Location", "wifi", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");    
  server.client().stop();
  saveCredentials();
  IsSetupAp = false;
  Serial.println(VerificationCode);
  Serial.println(Ssid);
  Serial.println(Password);
}

void handleNotFound()
{
  if (captivePortal())
  {
    return;
  }
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}
