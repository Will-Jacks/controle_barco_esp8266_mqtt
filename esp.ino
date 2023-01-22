#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define led D8

#define sentido1MotorA D6
#define sentido2MotorA D2

#define sentido1MotorB D3
#define sentido2MotorB D4


const char *ssid = "Ura";
char *passwd = "28041999";

String str_msg;

WiFiClient wlanclient;
PubSubClient mqttClient(wlanclient);

void piscaLed() {
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
}

void piscaLedBroker() {
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
}

void wifiConnect() {
  WiFi.begin(ssid,passwd);

  Serial.println ("Conectando ao WiFi");
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print (".");
    piscaLed();
    delay(300);
  }
  Serial.println("");
  Serial.print ("Conectado ao WiFi, no IP: ");
  Serial.println (WiFi.localIP());
}

void mqttConnect(){
  mqttClient.setServer ("broker.emqx.io",1883);
  mqttClient.setCallback(mqttCallback);

  if(mqttClient.connect ("ESP-Client",NULL,NULL)) {

    Serial.println ("Conectado ao MQTT Broker.");
  } else {
    Serial.println("MQTT Broker connection failed");
    delay(200);
  }

  mqttClient.subscribe("WILL/teste");
  piscaLedBroker();
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  char message[5]={0x00};

  for(int i=0;i<length;i++)
  message[i]=(char)payload[i];

  message[length]=0x00;
  Serial.println (message);
  str_msg = String(message);

  recebeMensagem();
}

void recebeMensagem() {
  if(str_msg.equals ("Cima")) {
   delay(500);
   digitalWrite(sentido1MotorA, HIGH);
   digitalWrite(sentido1MotorB, HIGH);

   digitalWrite(sentido2MotorA, LOW);
   digitalWrite(sentido2MotorB, LOW);
 }
 if(str_msg.equals ("Tras")) {
   delay(500);
   digitalWrite(sentido1MotorA, LOW);
   digitalWrite(sentido1MotorB, LOW);

   digitalWrite(sentido2MotorA, HIGH);
   digitalWrite(sentido2MotorB, HIGH);
 }

 if(str_msg.equals ("Parar")) {
  delay(500);
  digitalWrite(sentido1MotorA, LOW);
  digitalWrite(sentido1MotorB, LOW);

  digitalWrite(sentido2MotorA, LOW);
  digitalWrite(sentido2MotorB, LOW);
 }

 if (str_msg.equals ("Liga led")) {
  digitalWrite(led, HIGH);
 }
 
}

void setup() {
  Serial.begin (9600);
  pinMode(led,OUTPUT);
  pinMode(sentido1MotorA, OUTPUT);
  pinMode(sentido2MotorA, OUTPUT);
  pinMode(sentido1MotorB, OUTPUT);
  pinMode(sentido2MotorB, OUTPUT);

  wifiConnect();
  mqttConnect();

}

void loop() {
  mqttClient.loop();
}
