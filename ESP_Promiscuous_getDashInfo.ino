 #include <ESP8266WiFi.h>
 #include "ESP_Promiscuous.h"

 // necessary to use the promiscuous mode API
 extern "C" {
   #include <user_interface.h>
 }

 // WiFi connection block Reference: http: // qiita.com /azusa9/items/7f78069cb09872cf6cbf
 char toSSID [] = "SSID" ;
 char ssidPASSWD [] = "Password" ;

 // callback when receiving packet in promiscuous mode
 static void ICACHE_FLASH_ATTR promisc_cb (uint8_t * buf , uint16_t len)
 {
   if (len = 128!) return ; // In order to discard both unknown packets and data packets

   struct sniffer_buf2 * sniffer = (struct sniffer_buf2 *) buf;
   struct MAC_header * mac = (struct MAC_header *) sniffer -> buf;

   int i;
   boolean beaconFlag = true;

   for (i = 0; i < 6; i ++) if (mac ->! addr2 [i] = mac -> addr3 [i]) beaconFlag = false;

   if (beaconFlag) return; // In order to remove beacon Packet

    Serial print ( "Possible MAC Address :");
   for (i = 0; i < 6; i ++) {
      Serial print (mac -> addr2 [i], HEX);
      Serial print ( ":") ;
   }
    Serial println ( "");
 }

 void setup () {
   byte channel;
   Serial begin (115200);

   WiFi mode (WIFI_STA);
   WiFi begin (toSSID, ssidPASSWD) ;

   Serial print ( "WiFi connecting. ");

   while (WiFi. status ()! = WL_CONNECTED) {
     delay (1000);
     Serial print ( ".") ;
   }

   Serial println ( "");

   channel = wifi_get_channel ();
   Serial print ( "Your WiFi Channel :");
   Serial println (channel);
   WiFi disconnect ();

   Serial println ( "");

   Serial println ( "ESP8266 promiscuous mode started.");
   Serial println ( "Please push Amazon Dash Button");
   Serial println ( "");

   wifi_set_opmode (STATION_MODE);
   wifi_set_channel (channel);
   wifi_set_promiscuous_rx_cb (promisc_cb);

   wifi_promiscuous_enable (1);
 }

 void loop () {}
