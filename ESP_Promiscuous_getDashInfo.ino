#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "ESP_Promiscuous.h"

extern "C" {
#include <user_interface.h>
}

#define RELAY_Pin 5
byte channel = 5; // WiFi channel (1-13)
String webpage = "";
static const char* ssid = "Telecom-02031983";
static const char* password = "euscimmoarimirarlestelle";
static uint8_t dash1ButtonMAC [6] = {0xAC , 0x63, 0xBE, 0x78, 0x80, 0x26};
unsigned long lastMillis = 0;
boolean dash1Found = false;

//non usati per ora
static const char* ipAddressString = "192.168.1.107";
static const char* netMaskString = "255.255.0.0";
static const char* gateWayString = "192.168.1.1";

ESP8266WebServer server(666);

static void ICACHE_FLASH_ATTR promisc_cb (uint8_t * buf , uint16_t len)
{
  if (len == 12) {
    // No accurate information about MAC address and length of the head of packet.
    struct RxControl * sniffer = (struct RxControl *) buf;
    return;
  } else if (len == 128) {
    // Management Packet
    struct sniffer_buf2 * sniffer = (struct sniffer_buf2 *) buf;
    struct MAC_header * mac = (struct MAC_header *) sniffer -> buf;
    int i;
    boolean MAC_Matching_Flag = true;
    for (i = 0; i < 6; i ++)
      if (mac -> addr2 [i] != dash1ButtonMAC [i])
        MAC_Matching_Flag = false;
    if (!MAC_Matching_Flag) return; // No hit
    if (millis () - lastMillis < 7000) return; // In order to avoid a duplicate detection; 1 cycle needs around 6,500 msec.
    lastMillis = millis ();
    // Handle it.
    wifi_promiscuous_enable (0); // After obtaining the event, interrupt the promiscuous mode for WiFi connection.
    dash1Found = true;
  } else {
    // Data Packet
    struct sniffer_buf * sniffer = (struct sniffer_buf *) buf;
    struct MAC_header * mac = (struct MAC_header *) sniffer -> buf;
    return;
  }
}

void WifiSetup () {
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid,password);
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
  Serial.println(WiFi.localIP()); // Use the IP address printed here to connect to the server e.g. http://192.168.0.42
}

void setup () {
  connectToWIFI();
}

void connectToWIFI() {
  WiFi.disconnect (); // to avoid WDT reset
  Serial.begin (115200);
  wifi_set_opmode (STATION_MODE);
  wifi_set_channel (channel);
  wifi_set_promiscuous_rx_cb (promisc_cb);
  WifiSetup ();
  WiFi.disconnect ();
  // Start!
  wifi_promiscuous_enable (1);
}



void loop () {
  static int cnt = 0;
  delay (100); // msec
  cnt ++;
  if (cnt == 5) {
    cnt = 0;
    wifi_promiscuous_enable (1); // promiscuous mode Resume
    // Serial.println ("on");
  } else {
    wifi_promiscuous_enable (0); // promiscuous mode stop
    // Serial.println ("off");
  }
  if (!dash1Found) {
    return;
  }
  else {
    webOnOff();
    dash1Found = false;
  }
  WifiSetup ();
  WiFi.disconnect ();
  wifi_promiscuous_enable (1); // promiscuous mode Resume
}


//Inizializza i pin a LOW
void initializePins() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}


//Gestione degli eventi quando si accede dalla pagina web
void webOnOff() {
  String requestUrl = server.uri();
  if (dash1Found) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.println("Switched LED ON/OFF");
  }
}
