#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#ifndef APSSID
#define APSSID "CreXLocalWifi"
#define APPSK  "12345678"
#endif

#define RESET_PIN D0

const char *SoftAP_Ssid = APSSID;
const char *SoftAP_Password = APPSK;

const char *MyHostname = "crex";

char Ssid[32] = "";
char Password[32] = "";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);

const char* Host = "192.168.1.165";
const uint16_t Port = 23;
WiFiClient client;

char VerificationCode[32] = "";

bool IsSetupAp;
bool IsConnect;

const char* www_username = "admin";
const char* www_password = "esp8266";
const char* www_realm = "Custom Auth Realm";
String authFailResponse = "Authentication Failed";

bool isStringClientComplete;
String receiveClientString;

bool isStringSerialComplete;
String receiveSerialString;

void setup() {
  delay(1000);
  Init();
  ServerAndDNSInit();
}

void loop() {
  WifiModeConnect();
  
  if(IsResetPinPressed())
  {
    IsSetupAp = true;
    IsConnect = false;
  }
  
  if(IsSetupAp)
  {
    ArduinoOTA.handle();
    dnsServer.processNextRequest();
    server.handleClient();
  }
  else
  {
    ForwardDataToSerial();
    ForwardDataToClient();
  }
}

void Init()
{
  Serial.begin(115200);
  IsSetupAp = true;
  IsConnect = false;
  pinMode(RESET_PIN, INPUT);
  loadCredentials();
}

void ServerAndDNSInit()
{
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleWifi);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  server.on("/generate_204", handleWifi);
  server.on("/fwlink", handleWifi);
  server.onNotFound(handleNotFound);
  server.begin();
}
