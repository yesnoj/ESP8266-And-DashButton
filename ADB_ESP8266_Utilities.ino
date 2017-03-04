#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(666);
struct softap_config config;

//Restituisce l'IP del client
String checkClient(ESP8266WebServer server) {
  WiFiClient clientConnected = server.client();
  String ipClient = clientConnected.remoteIP().toString();
  return ipClient;
}


//Non usato
void setupWifi() {
  char password[33];
  char ssid[32];
  wifi_softap_get_config(&config);
  memset(config.ssid, 0, sizeof(config.ssid));
  sprintf(ssid, "xxx", sizeof("xxx"));
  memcpy(config.ssid, ssid, strlen(ssid));
  config.ssid_len = strlen(ssid);
  memset(config.password, 0, sizeof(config.password));
  sprintf(password, "xxx", sizeof("xxx"));
  memcpy(config.password, password, strlen(password));
  config.authmode = AUTH_WPA2_PSK;
  config.ssid_hidden = 1;
  wifi_softap_set_config(&config);
}

//Non usato
void setStaticIP() {
  //uint32_t ipAddress = parseIPV4string(ipAddressString);
  //uint32_t netMask = parseIPV4string(netMaskString);
  //uint32_t gateWay = parseIPV4string(gateWayString);
  IPAddress ip(192, 168, 1, 107);
  IPAddress netmask(255, 255, 255, 0);
  IPAddress gateway(192, 168, 1, 1);
  Serial.print("Setting static ip to : ");
  Serial.println(ip);
  WiFi.config(ip, gateway, netmask);
}

//Non usato, va in conflitto con qualcosa
uint32_t parseIPV4string(char* ipAddress) {
  char ipbytes[4];
  sscanf(ipAddress, "%uhh.%uhh.%uhh.%uhh", &ipbytes[3], &ipbytes[2], &ipbytes[1], &ipbytes[0]);
  return ipbytes[0] | ipbytes[1] << 8 | ipbytes[2] << 16 | ipbytes[3] << 24;
}

String getStrMAC(uint8_t mac[6]) {
  String res = String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" +
               String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX);
  return res;
}
