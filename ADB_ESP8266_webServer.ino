#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>


extern "C" {
  #include <user_interface.h>
}

#define RELAY_Pin D1
#define LED_Pin 2
#define WLAN_SSID       "ESPap"
#define WLAN_PASS       "thereisnospoon"

struct softap_config config;
uint8_t monitoringMAC1[6] = {0x34 , 0xD2, 0x70, 0x1A, 0x80, 0x15};
boolean detectingMAC1 = false;
int detectInterval = 8000; //msec
unsigned long lastDetectedMillis = 0;

ESP8266WebServer server(80);

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
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  connectWiFi();
  initializePage();
  server.on("/", initializePage);
  server.on("/RELE", webOnOff);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  if (detectingMAC1) {
    webOnOff();
  }
}

void connectWiFi() {
  WiFi.disconnect();
  delay(10);
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
  Serial.println(WiFi.softAPIP());
  wifi_set_event_handler_cb(wifi_handle_event_cb);
}


void initializePage() {
  updateHTML(checkClient());
}


String checkClient() {
  WiFiClient clientConnected = server.client();
  String ipClient = clientConnected.remoteIP().toString();
  return ipClient;
}


//Aggiorna la pagina del webserver
void updateHTML(String IP) {
  pinMode(RELAY_Pin, OUTPUT);
  String webpage = "";
  webpage += "<div id=\"header\"><h1>MiniServer ESP8266</h1></div>";

  webpage += "<h2>Client connected :</h2>";
  webpage +=  IP;
  
  webpage += "<div id=\"section\"><h2>RELE Status</h2>";
  webpage += "RELE has been switched : " + String((digitalRead(RELAY_Pin) == HIGH) ? "ON" : "OFF");
  webpage += "<br><br>";
  webpage += "<input type=\"button\" value=\"RELE " + String((digitalRead(RELAY_Pin) == HIGH) ? "ON" : "OFF") + " \" + onclick=\"window.location.href=\'/RELE\'\">";
    
  webpage += "</div>";

  server.send(200, "text/html", webpage);
  delay(1000);
}


void initializePins() {
  pinMode(LED_Pin, OUTPUT);
  pinMode(RELAY_Pin, OUTPUT);
  
  digitalWrite(LED_Pin, HIGH);   // turn off LED with voltage HIGH
  digitalWrite(RELAY_Pin, LOW);  // turn off relay with voltage LOW
}

//Gestione degli eventi quando si accede dalla pagina web
void webOnOff() {
  String requestUrl = server.uri();
  Serial.println("Connected client: " + checkClient());
  updateHTML(checkClient());
  if (requestUrl.indexOf("RELE") != -1 || detectingMAC1) {
    pinMode(RELAY_Pin, OUTPUT);
    pinMode(LED_Pin, OUTPUT);
    digitalWrite(RELAY_Pin, !digitalRead(RELAY_Pin));
    digitalWrite(LED_Pin, !digitalRead(LED_Pin));
    Serial.println("Switched ON/OFF");
    detectingMAC1 = false;
  }
  initializePage();
}
