#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// 0 is the special socket for all sockets
#define ALL_SOCKETS 0
#define BUFFER_LENGTH 100

char stringBuffer1[BUFFER_LENGTH];
char stringBuffer2[BUFFER_LENGTH];

WiFiClient espClient;
PubSubClient client(espClient);

bool onArray[5][4]={
 {1,1,0,1},
 {1,1,1,1},
 {0,1,1,1},
 {1,0,1,1},
 {0,0,1,1}
};

bool offArray[5][4]={
 {1,1,0,0},
 {1,1,1,0},
 {0,1,1,0},
 {1,0,1,0},
 {0,0,1,0}
};

bool socketState[4]={0,0,0,0};

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String strTopic = String(topic);
  int requestedSocket = strTopic.charAt(strTopic.length()-1)-'0';
  boolean requestedState = false;
  if ((char)payload[0] == 't'){
    requestedState = true;
  }

  changeSocket(requestedSocket, requestedState);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += WiFi.macAddress();
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      String mac = WiFi.macAddress();
      mac.toCharArray(stringBuffer1, BUFFER_LENGTH);
      client.publish("mqttsockets/register", stringBuffer1);
      publishStatus();
      setupSubscriptions();         
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setupSubscriptions(){
  Serial.println("Subscriptions");
  for(char i=0;i<=4;i++){  
    String topic = "mqttsockets/change/"+WiFi.macAddress()+"/"+String(char('0'+i));
    Serial.println(topic);
    topic.toCharArray(stringBuffer1, BUFFER_LENGTH);
    client.subscribe(stringBuffer1, 1);
  }
}

void publishStatus(){
  Serial.println("Publishes");
  for(char i=1;i<=4;i++){
    String topic = "mqttsockets/status/"+WiFi.macAddress()+"/"+String(char('0'+i));
    Serial.print(topic);
    Serial.print(" : ");
    topic.toCharArray(stringBuffer1, BUFFER_LENGTH);
    Serial.println(getStateAsString(i));
    client.publish(stringBuffer1, getStateAsString(i));  
  }
}

const char* getStateAsString(int socket){
  if(getState(socket)){
    return "true";
  }else{
    return "false";
  }
}

int getState(int socket){
  return socketState[socket-1];
}

void setState(int socket, bool state){
  if(socket==0){
    for(int i=0;i<4;i++){
      socketState[i]=state;
    }
  }else{
    socketState[socket-1]=state;
  }
  publishStatus();
}

void sendSignal(int socket, bool state){
  bool *switchArray;
  if(state){
    switchArray = onArray[socket];
  }else{
    switchArray = offArray[socket];
  }
  Serial.println("Sending Signal");  
  Serial.println(switchArray[0]);  
  Serial.println(switchArray[1]);  
  Serial.println(switchArray[2]);  
  Serial.println(switchArray[3]);  
  digitalWrite(D0, switchArray[0]);
  digitalWrite(D1, switchArray[1]);
  digitalWrite(D2, switchArray[2]);
  digitalWrite(D3, switchArray[3]); 
  delay(100);
  digitalWrite(D5, HIGH);
  delay(500);
  digitalWrite(D5, LOW);
  delay(100);
  digitalWrite(D0, LOW);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);   
  Serial.println("Send Done");  
}
void changeSocket(int socket, bool state){
  socket = constrain(socket, 0, 4);
  setState(socket, state);
  sendSignal(socket, state);
}

void turnOnSocket(int socket){
  changeSocket(socket, true);
}

void turnOffSocket(int socket){
  changeSocket(socket, false);
}

void setup() {
    
  // put your setup code here, to run once:
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);

  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();

  setup_wifi();

  //Serial.println(WiFi.macAddress());
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
