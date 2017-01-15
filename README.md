## mqtt_switch Arduino based esp8266 for controlling Energenie sockets ##

Firmware for an ESP-12 that uses an Energenie Pi-mote to control Energenie sockets based on MQTT messages. Written with Arduino.

Compatible with [homebridge-mqttswitch](https://github.com/ilcato/homebridge-mqttswitch)

## Setup and operation ##

You'll need to copy the example_config.h to config.h and enter the appropriate values for your network.

Once you've got the hardware set up, as below, and you've flashed the firmware to your ESP whenever the ESP starts or reconnects to the MQTT server for whatever reason it will register its MAC address in the MQTT topic ```mqttsockets/register```.

## MQTT Topics ##

| Topic | Pub/Sub | Payload | Notes |
|-------|---------|---------|-------|
| mqttsockets/register | Pub | e.g. ```18:FE:34:BB:CC:DD``` | Whenever a client with this firmware connects to the MQTT server it will report it's MAC here |
| mqttsockets/status/18:FE:34:BB:CC:DD/1 | Pub | ```true``` or ```false``` | Status of socket 1 on ESP with MAC as specified in the topic. Reported on startup and whenever the state is changed |
| mqttsockets/status/18:FE:34:BB:CC:DD/2 | Pub | ```true``` or ```false``` | Status of socket 2 on ESP with MAC as specified in the topic. Reported on startup and whenever the state is changed |
| mqttsockets/status/18:FE:34:BB:CC:DD/3 | Pub | ```true``` or ```false``` | Status of socket 3 on ESP with MAC as specified in the topic. Reported on startup and whenever the state is changed |
| mqttsockets/status/18:FE:34:BB:CC:DD/4 | Pub | ```true``` or ```false``` | Status of socket 4 on ESP with MAC as specified in the topic. Reported on startup and whenever the state is changed |
| mqttsockets/change/18:FE:34:BB:CC:DD/1 | Sub | ```true``` or ```false``` | ESP with MAC as specified in the topic will subscribe to this topic and will change the state of socket 1 based on the payload of messages published |
| mqttsockets/change/18:FE:34:BB:CC:DD/2 | Sub | ```true``` or ```false``` | ESP with MAC as specified in the topic will subscribe to this topic and will change the state of socket 2 based on the payload of messages published |
| mqttsockets/change/18:FE:34:BB:CC:DD/3 | Sub | ```true``` or ```false``` | ESP with MAC as specified in the topic will subscribe to this topic and will change the state of socket 3 based on the payload of messages published |
| mqttsockets/change/18:FE:34:BB:CC:DD/4 | Sub | ```true``` or ```false``` | ESP with MAC as specified in the topic will subscribe to this topic and will change the state of socket 4 based on the payload of messages published |
| mqttsockets/change/18:FE:34:BB:CC:DD/0 | Sub | ```true``` or ```false``` | ESP with MAC as specified in the topic will subscribe to this topic and will change the state of all 4 sockets based on the payload of messages published |

## Hardware ##

I'm using a ESP8266 in the form of a NodeMCU 1.0 devboard, a Wemos D1 or similar would also work fine.

This is connected to an [Energenie pi-mote](https://www.amazon.co.uk/Energenie-Pi-mote-Remote-Control-Raspberry/dp/B00P3JIUQO) 

### Connections ###

* D0-D3 on the ESP are connected to D0-D3 of the encoder on the Energenie board, see the [Pi-mote schematic](https://energenie4u.co.uk/res/pdfs/ENER314%20UM.pdf) for details of the Pi-mote pinout.
* D5 on the ESP is connected to "Modulator Enable" line again see the [Pi-mote schematic](https://energenie4u.co.uk/res/pdfs/ENER314%20UM.pdf) for details.
* We want ASK/OOK (not FSK) mode so wire a ground from the ESP to the !OOK/FSK pin on the pi-mote.
* Wire up 3.3V and ground from the ESP to 3.3v and ground on the pi-mote

## Other software ##

I use this with a [Homebridge server](https://github.com/nfarina/homebridge) running in Docker container (and a mqtt Moqsuitto server) on my home Synology NAS. Homebridge allows you to control IoT things with Apple Home/Siri.

## Example homebridge config ##
```json
{
    "bridge": {
        "name": "Homebridge",
        "username": "11:22:33:44:55:66",
        "port": 51826,
        "pin": "111-22-333"
    },

    "description": "This is an example configuration file for homebridge with mqttswitch plugin.",
	
    "accessories": [
    	{
        	"accessory": "mqttswitch",
            "name": "lounge-socket1",
            "url": "mqtt://192.168.0.2:1883",
			"username": "",
			"password": "",
 			"caption": "lounge-socket1",
 			"topics": {
 				"statusGet": 	"mqttsockets/status/18:FE:34:BB:CC:DD/1",
 				"statusSet": 	"mqttsockets/change/18:FE:34:BB:CC:DD/1"
 			}
    	},
    	{
        	"accessory": "mqttswitch",
            "name": "lounge-socket2",
            "url": "mqtt://192.168.0.2:1883",
			"username": "",
			"password": "",
 			"caption": "lounge-socket2",
 			"topics": {
 				"statusGet": 	"mqttsockets/status/18:FE:34:BB:CC:DD/2",
 				"statusSet": 	"mqttsockets/change/18:FE:34:BB:CC:DD/2"
 			}
    	},		
    	{
        	"accessory": "mqttswitch",
            "name": "lounge-socket3",
            "url": "mqtt://192.168.0.2:1883",
			"username": "",
			"password": "",
 			"caption": "lounge-socket3",
 			"topics": {
 				"statusGet": 	"mqttsockets/status/18:FE:34:BB:CC:DD/3",
 				"statusSet": 	"mqttsockets/change/18:FE:34:BB:CC:DD/3"
 			}
    	},
		{
        	"accessory": "mqttswitch",
            "name": "lounge-socket4",
            "url": "mqtt://192.168.0.2:1883",
			"username": "",
			"password": "",
 			"caption": "lounge-socket4",
 			"topics": {
 				"statusGet": 	"mqttsockets/status/18:FE:34:BB:CC:DD/4",
 				"statusSet": 	"mqttsockets/change/18:FE:34:BB:CC:DD/4"
 			}
    	}
    ]	
}
```
