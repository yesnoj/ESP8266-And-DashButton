#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

extern "C" {
#include <user_interface.h>
}

#define RELAY_Pin 5
#define WLAN_SSID       "xxx"
#define WLAN_PASS       "xxx"
#define ipAddressString "192.168.1.107";
#define netMaskString   "255.255.0.0";
#define gateWayString   "192.168.1.1";
//-------------------------------------------------------------------------


ESP8266WebServer server(666);
struct softap_config config;
uint8_t monitoringMAC1[6] = {0x34 , 0xD2, 0x70, 0x1A, 0x80, 0x15};
boolean detectingMAC1 = false;
int detectInterval = 8000; //msec
unsigned long lastDetectedMillis = 0;


void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt) {
  if (evt->event != EVENT_SOFTAPMODE_PROBEREQRECVED) {
    return;
    }
  
  uint8_t* mac = evt->event_info.ap_probereqrecved.mac;
  if (memcmp(mac, monitoringMAC1, 6) == 0 && detectInterval < (millis() - lastDetectedMillis)) {
    lastDetectedMillis = millis();
    detectingMAC1 = true;
    }
}

void setup() {
  Serial.begin(115200);
  initializePins();
  initializePage();
  //setStaticIP(); //funziona,ma è solo un test per verificarne il funzionamento
  connectWiFi();
  server.on("/Domotica", initializePage);
  server.on("/LED", webOnOff);
  server.on("/RELE", webOnOff);
  server.begin();
}

void loop() {
  if (detectingMAC1) {
    webOnOff();
  }
}

void connectWiFi() {
  WiFi.disconnect();
  delay(10);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(100);
  Serial.print("\nConnecting to ");
  Serial.print(WLAN_SSID);
  // Wait for connection
  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ <= 10) {//wait 10 seconds
    Serial.print(".");
    delay(1000);
  }
  if (i == 11) {
    Serial.print("\nCould not connect to network...");
    while (1) delay(500);
  }
  Serial.print("\nConnected to IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.softAP(WLAN_SSID, "xxx");
  wifi_set_event_handler_cb(wifi_handle_event_cb);
}


//Aggiorna la pagina del webserver
void updateHTML(String IP) {
  pinMode(LED_BUILTIN, OUTPUT);
  String webpage = "";
  webpage += "<div id=\"header\"><h1>MiniServer ESP8266</h1></div>";

  webpage += "<h2>Client connected :</h2>";
  webpage +=  IP;

  webpage += "<div id=\"section\"><h2>LED Status</h2>";
  webpage += "LED has been switched : " + String((digitalRead(LED_BUILTIN) == HIGH) ? "ON" : "OFF");
  webpage += "<br><br>";
  webpage += "<input type=\"button\" value=\"LED " + String((digitalRead(LED_BUILTIN) == HIGH) ? "ON" : "OFF") + " \" + onclick=\"window.location.href=\'/LED\'\">";

  webpage += "</div>";

  server.send(200, "text/html", webpage);
  delay(1000);
}

//Restituisce l'IP del client
String checkClient() {
  WiFiClient clientConnected = server.client();
  String ipClient = clientConnected.remoteIP().toString();
  return ipClient;
}


//Inizializza la pagina web con i valori attuali dei pin
void initializePage() {
  updateHTML(checkClient());
}

//Inizializza i pin a LOW
void initializePins() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}



//Gestione degli eventi quando si accede dalla pagina web
void webOnOff() {
  String requestUrl = server.uri();
  Serial.println("Connected client: " + checkClient());
  updateHTML(checkClient());
  if (requestUrl.indexOf("LED") != -1 || detectingMAC1) {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.println("Switched LED ON/OFF");
  }
  detectingMAC1 = false;
  initializePage();
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
