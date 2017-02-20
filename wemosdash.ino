#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
extern "C" {
#include <user_interface.h>
}

#define RELAY_Pin 5 



String webpage = "";
//const char* ssid = "Telecom-02031983";             
//const char* password = "euscimmoarimirarlestelle";
const char* ssid = "OnePlus3";             
const char* password = "ciaomamma"; 
const char* ipAddressString = "192.168.1.107";
const char* netMaskString = "255.255.0.0";
const char* gateWayString = "192.168.1.1";
uint8_t dash1ButtonMAC [6] = {0xAC , 0x63, 0xBE, 0x78, 0x80, 0x26}; 
boolean dash1Found = false;

WiFiUDP Udp;
unsigned int localUdpPort = 67;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[] = "Hi there! Got the message 😊";  // a reply string to send back

ESP8266WebServer server(666); 

void setup() {
	initializePins();
	initializePage();
	//setStaticIP(); //funziona,ma è solo un test per verificarne il funzionamento
	connectToWIFI();
	server.on("/Domotica", initializePage);
	server.on("/LED", webOnOff);
	server.on("/RELE", webOnOff);
	//webOnOff();
	server.begin();
}

void loop(){
	server.handleClient();   // Wait for a client to connect and when they do process their request
	if (dash1Found){
		dashOnOff();
	}
}

void initializePage(){
	updateHTML(digitalRead(RELAY_Pin),checkClient());
	updateHTML(digitalRead(LED_BUILTIN),checkClient());
}

void initializePins(){
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
	pinMode(RELAY_Pin, OUTPUT);
	digitalWrite(RELAY_Pin, LOW);
}

String checkClient(){
	WiFiClient clientConnected = server.client();
	String ipClient = clientConnected.remoteIP().toString();
	return ipClient;
}


void dashOnOff() {
	checkClient();
	String requestUrl = server.uri(); 
	Serial.println("Connected client: " + checkClient());
	updateHTML(digitalRead(RELAY_Pin),checkClient());
	updateHTML(digitalRead(LED_BUILTIN),checkClient());
	if (dash1Found){
		pinMode(LED_BUILTIN, OUTPUT);
		if( digitalRead(LED_BUILTIN) == HIGH ){
			digitalWrite(LED_BUILTIN, LOW);
			Serial.println("Switched LED ON");
		}
		else {
			digitalWrite(LED_BUILTIN, HIGH);
			Serial.println("Switched LED OFF");
		}
		initializePage();
		dash1Found = false;
	}
}



void webOnOff() {
	checkClient();
	String requestUrl = server.uri(); 
	Serial.println("Connected client: " + checkClient());
	updateHTML(digitalRead(RELAY_Pin),checkClient());
	updateHTML(digitalRead(LED_BUILTIN),checkClient());
	if (requestUrl.indexOf("LED") != -1){
		pinMode(LED_BUILTIN, OUTPUT);
		if( digitalRead(LED_BUILTIN) == HIGH ){
			digitalWrite(LED_BUILTIN, LOW);
			Serial.println("Switched LED ON");
		}
		else {
			digitalWrite(LED_BUILTIN, HIGH);
			Serial.println("Switched LED OFF");
		}
	}
	if (requestUrl.indexOf("RELE") != -1){
		pinMode(RELAY_Pin, OUTPUT);
		if( digitalRead(RELAY_Pin) == HIGH ){
			digitalWrite(RELAY_Pin, LOW);
			Serial.println("Switched RELE ON");
		}
		else {
			digitalWrite(RELAY_Pin, HIGH);
			Serial.println("Switched RELE OFF");
		}
	}
	initializePage();
}

void updateHTML(int status,String IP){
	pinMode(LED_BUILTIN, OUTPUT);
	String webpage = "";
	webpage += "<div id=\"header\"><h1>MiniServer ESP8266</h1></div>";

	webpage += "<h2>Client connected :</h2>";
	webpage +=  IP;

	webpage += "<div id=\"section\"><h2>LED Status</h2>";
	webpage += "LED has been switched : " + String((digitalRead(LED_BUILTIN) == HIGH)?"ON":"OFF");
	webpage += "<br><br>";
	webpage += "<input type=\"button\" value=\"LED " + String((digitalRead(LED_BUILTIN) == HIGH)?"ON":"OFF") +" \" + onclick=\"window.location.href=\'/LED\'\">";

	Jonsey, [20.02.17 15:44]
	webpage += "<div id=\"section\"><h2>Relè Status</h2>";
	webpage += "Status : " + String((digitalRead(RELAY_Pin) == HIGH)?"ON":"OFF");
	webpage += "<br><br>";
	webpage += "<input type=\"button\" value=\"RELE " + String((digitalRead(RELAY_Pin) == HIGH)?"ON":"OFF") +" \" + onclick=\"window.location.href=\'/RELE\'\">";
	
	webpage += "</div>";

	server.send(200, "text/html", webpage);
	delay(1000);
}

void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt) {
	//printf("Free heap size: %d\n", system_get_free_heap_size());
	if (evt->event != EVENT_SOFTAPMODE_STACONNECTED) ;
	else{
		//Serial.println("EVENT_SOFTAPMODE_STACONNECTED");
		printf("station:" MACSTR "join, AID = %d\n", MAC2STR (evt->event_info.sta_connected.mac),(evt->event_info.sta_connected.aid));
	} 
	if (evt->event != EVENT_SOFTAPMODE_PROBEREQRECVED) ;
	else {
		//Serial.println("EVENT_SOFTAPMODE_PROBEREQRECVED");
		uint8_t* mac = evt->event_info.ap_probereqrecved.mac;
		if (memcmp(mac,dash1ButtonMAC,6) == 0) dash1Found = true;
	}
}


void connectToWIFI(){
	Serial.begin(115200);

	WiFi.mode(WIFI_AP_STA);
	WiFi.begin(ssid, password);
	delay(100);
	Serial.print("\nConnecting to ");
	Serial.print(ssid);
	// Wait for connection
	int i = 0;
	while (WiFi.status() != WL_CONNECTED && i++ <= 10) {//wait 10 seconds
		Serial.print(".");
		delay(1000);
	}
	if(i == 11){
		Serial.print("\nCould not connect to network...");
		while(1) delay(500);
	}
	Serial.print("\nConnected to IP address: ");
	Serial.println(WiFi.localIP()); // Use the IP address printed here to connect to the server e.g. http://192.168.0.42

	WiFi.softAP(ssid, password);
	wifi_set_event_handler_cb(wifi_handle_event_cb);
}

void setStaticIP(){
	//uint32_t ipAddress = parseIPV4string(ipAddressString);
	//uint32_t netMask = parseIPV4string(netMaskString);
	//uint32_t gateWay = parseIPV4string(gateWayString);
	IPAddress ip(192,168,1,107);
	IPAddress netmask(255,255,255,0);
	IPAddress gateway(192,168,1,1);
	Serial.print(F("Setting static ip to : "));
	Serial.println(ip);
	WiFi.config(ip, gateway, netmask);
}

String getStrMAC (uint8_t mac [6] ) {
	String res = String (mac [0 ], HEX) + ":" + String (mac [1], HEX) + ":" + String (mac [2], HEX) + ":" +
	String (mac [3], HEX ) + ":" + String (mac [4], HEX) + ":" + String (mac [5], HEX);
	return res;
}

//IN CONFLITTO CON QUALCOSA...
uint32_t parseIPV4string(const char* ipAddress) {
	char ipbytes[4];
	sscanf(ipAddress, "%uhh.%uhh.%uhh.%uhh", &ipbytes[3], &ipbytes[2], &ipbytes[1], &ipbytes[0]);
	return ipbytes[0] | ipbytes[1] « 8 | ipbytes[2] « 16 | ipbytes[3] « 24;
}
