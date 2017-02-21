  #include "ESP_Promiscuous.h"

 extern "C" {
   #include <user_interface.h>
 }

 byte channel = 5; // WiFi channel (1-13)
 uint8_t targetMAC1 [6] = {0x12 , 0x34, 0x56, 0x78, 0x9a, 0xbc}; // Amazon Dash Button MAC address
 int detectInterval = 8000; // msec

 unsigned long lastDetectedMillis = 0;
 boolean ADBdetectingNow = false; // Amazon Dash Button event detection flag

 static void ICACHE_FLASH_ATTR promisc_cb (uint8_t * buf , uint16_t len)
 {
     Ignore if // Amazon Dash Button event detection in (detectInterval specified number of milliseconds); if (ADBdetectingNow) return

     if (len == 12) {
       // No accurate information about MAC address and length of the head of packet.
       struct RxControl * sniffer = (struct RxControl *) buf;
       return;
     } Else if (len == 128) {
       // Management Packet
       struct sniffer_buf2 * sniffer = (struct sniffer_buf2 *) buf;
       struct MAC_header * mac = (struct MAC_header *) sniffer -> buf;

       int i;
       boolean MAC_Matching_Flag = true;

       for (i = 0; i < 6; i ++) if (mac ->! addr2 [i] = targetMAC1 [i]) MAC_Matching_Flag = false;

       if (MAC_Matching_Flag!) return; // No hit

       // Handle it.
       . Serial println ( "ADB push detected .");
       ADBdetectingNow = true;
       lastDetectedMillis = millis ();

       return;

     } Else {
       // Data Packet
       struct sniffer_buf * sniffer = (struct sniffer_buf *) buf;
       struct MAC_header * mac = (struct MAC_header *) sniffer -> buf;

       return;
     }
 }

 void setup () {  
   . Serial begin (115200);
   wifi_set_opmode (STATION_MODE);
   wifi_set_channel (channel);
   wifi_set_promiscuous_rx_cb (promisc_cb);

   // Start!
   wifi_promiscuous_enable (1);
 }

 void loop () {
   unsigned long interval;
   if (lastDetectedMillis> millis ()) // If the running time is 49.7 days beyond millis () overflows
     interval = (0xffffffff - lastDetectedMillis) + millis ();
   Else
     interval = millis () - lastDetectedMillis;

   if (interval> detectInterval)
     ADBdetectingNow = false;
 }
