#include <Thread.h>
#include <ThreadController.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Time.h>
#include <TimeLib.h>

#define PORT 300

const char* topic = "$DEVICE/snoop/motion";
const char* ssid     = "ATT121";
const char* password = "9804522874";
//const char *BROKER = "192.168.1.79"; 
const char *BROKER = "192.168.77.176";
const char* streamId   = "....................";
const char* privateKey = "....................";

WiFiClient WiFi_Client;
PubSubClient pubsubClient(WiFi_Client);

char pubString[] = "";
char message_buff[] = "";
int detected = 0;
Thread mqttThread = Thread();
ThreadController controller = ThreadController();

// ------------------- Device Methods -----------------
void PubSubClientCallback(char* topic, byte* payload, long unsigned length){
}

void setup() {

  //setTime(21,05,00,14,1,2012);
  Serial.begin(9600);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pubsubClient.setServer(BROKER,1881);
  pubsubClient.setCallback(PubSubClientCallback);
  mqttThread.setInterval(1500);
  mqttThread.onRun(mqtt_cb);
  controller.add(&mqttThread);

  delay(1500);

}

void mqtt_cb(){

  detected = digitalRead(4);
  char packetBuffer[] = "true";
  String packet = String(packetBuffer);

  while (!pubsubClient.connected()) {
    if (pubsubClient.connect("Snoop")) {
      //pubsubClient.subscribe("$DEVICE/snoop/motion"); 
      if(pubsubClient.publish(topic, packetBuffer )){
        Serial.print("published:");
        Serial.print(topic);
        Serial.print(" :: ");
        Serial.println(packetBuffer);
      }else{
        Serial.println("Did not publish, too large perhaps");
      }
    }
    delay(1000);
    pubsubClient.disconnect();
  }
    
}

void loop() {
  detected = digitalRead(4);
  Serial.println("Detected: "+String(detected));
  controller.run();
}
