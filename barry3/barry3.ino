/*
// BoxBot - PlantBuddy Source Code
*/

#include "Thread.h"
#include "ThreadController.h"
#include <SPI.h>
#include <Ethernet2.h>
#include <PubSubClient.h>
#include <Adafruit_BMP085.h>

// defines and variable for sensor/control mode
#define DEBUG 1
#define MODE_OFF    0  // Motion not detected
#define MODE_ON     1  // Motion detected

// ------------------- Method Dependencies -----------------
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
const char *BROKER = "192.168.1.79";

// ------------------- Ethernet Server -----------------
// Initialize the Ethernet server library
// with the IP address and port you
Adafruit_BMP085 bmp;
EthernetClient ethernetClient;
PubSubClient pubsubClient(ethernetClient);
Thread myThread = Thread();

int temperature;
int pressure;
int altitude;
int motion;
int delayTime = 500;
char pubString[] = "";
char message_buff[] = "";


// ------------------- Device Methods -----------------
void threadCallback(){
  Serial.println("Thread executed");
}

void PubSubClientCallback(char* topic, byte* payload, long unsigned length){
  
  int i = 0;

  Serial.println("Message arrived to server");
  Serial.println("topic: " + String(topic));
  Serial.println("Length: " + String(length,DEC));
  
  
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  


}

// ------------------- Device Routine -----------------
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  
  pinMode(A0,INPUT);
  Serial.begin(9600);

  pubsubClient.setServer(BROKER,3000);
  pubsubClient.setCallback(PubSubClientCallback);
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Allow the hardware to sort itself out
  delay(1500);
}

void reconnect() {
  
  Serial.println( "Moisture level is: "+sensorRead(A0) );

  while (!pubsubClient.connected()) {
    if (pubsubClient.connect("Barry")) {
      String moisture = sensorRead(A0);
      //start a thread here
      pubsubClient.subscribe("office/plant/moisture");
      moisture.toCharArray(message_buff, moisture.length()+1); 
      pubsubClient.publish("office/plant/moisture", message_buff );
      pubsubClient.disconnect();
    }else{
      delay(500);
    }
  }
}

String sensorRead(int in){
  int sensorReading = analogRead(A0);
  String reading = String(sensorReading);
  return reading;
}

void loop() {
  
  
  if (!pubsubClient.connected()) {
   reconnect();
  }
  pubsubClient.loop();

}
