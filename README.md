# ESP8266-And-DashButton
Script for use the Dash Button from Amazon with an Arduino D1 mini

## Register DashButton to your network

Press the dashbutton for 6 seconds and a new network called "Amazon ConfigureMe" will appear. Connecting to it we can see different informations like Button’s serial number, MAC address, and firmware version , like in the following image:

![dash-page](https://cloud.githubusercontent.com/assets/6757126/23156330/be7a49a4-f816-11e6-9296-1c7573e04482.png)

Once we have the mac address,and always connected to "Amazon ConfigureMe" network, we can use this info to register manually the dashbutton using this GET :

http://192.168.0.1/?amzn_ssid=SPECIFIED_SSID&amzn_pw=SPECIFIED_PASSWORD


### Sniff ARP packet from the Dash Button

The script allow to ESP8266 chip to "sniff" the messages from the DashButton mac, once that is recognized a funcion is called by the Wemos, so any IFTTT or extra configurazion is required. We need only a DashButton and an Wimos D1mini
