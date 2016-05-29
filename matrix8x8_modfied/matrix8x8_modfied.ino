static const uint8_t PROGMEM
  smile_bmp[] =
  { B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000},
  smile2_bmp[] =
  { B00000000,
    B00000000,
    B00111100,
    B00100100,
    B00100100,
    B00111100,
    B00000000,
    B00000000},
  neutral_bmp[] =
  { B00000000,
    B01111110,
    B01000010,
    B01000010,
    B01000010,
    B01000010,
    B01111110,
    B00000000 },
  frown_bmp[] =
  { B11111111,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B11111111 },
    
  other_bmp1[] =
  { B00011000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  other_bmp2[] =
  { B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  other_bmp3[] =
  { B00011000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  other_bmp4[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  other_bmp5[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00111100,
    B01111110 },
  other_bmp6[] =
  { B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00111100,
    B01111110 },
  other_bmp7[] =
  { B00000000,
    B00011000,
    B00011000,
    B00011000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 };

#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>
#include <Twitter.h>
#include <SPI.h>
#include <util.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Payload: ");
  Serial.write(payload, length);
  Serial.println();
}

static const uint8_t target[] = "192.168.1.84";
char server[] = "jessica@192.168.1.79"; 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 177);
EthernetClient client;

PubSubClient pubsub("https://A3RP95Y8XH0VYD.iot.us-east-1.amazonaws.com",1883,callback,client);

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

void setup() {

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  Serial.begin(9600);
  matrix.begin(0x70);  // pass in the address

  if (Ethernet.begin(mac) == 0) { 
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  // If button is down
 if( digitalRead(7) == HIGH ){
      char payload[] = "Button Pushed";
      pushAnim();
      boolean published = pubsub.publish("update",payload);
      Serial.println(published);
  }


  if (client.available()) {
    char payload[] = "I'm Here";
    boolean published = pubsub.publish("update",payload);
    Serial.println(published);
    connectedAnim();
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    IPErrorAnim();
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

//   if (client.connect("arduinoClient", "USERNAME", "PASSWORD")) {
//    client.publish("outTopic","hello world");
//    client.subscribe("inTopic");
//  }
  
}

static const uint8_t PROGMEM
invalidIP1[] = {
  B00000000,
  B00000000,
  B00011000,
  B00000000,
  B00000000,
  B00011000,
  B00000000,
  B00000000,
},
invalidIP2[] = {
  B00000000,
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00000000,
  B00000000,
  B00000000,
};

void IPErrorAnim(){
  matrix.clear();
  matrix.drawBitmap(0,0,invalidIP1,8,8,LED_ON);
  matrix.writeDisplay();
  delay(200);
  matrix.drawBitmap(0,0,invalidIP2,8,8,LED_ON);
  matrix.writeDisplay();
  delay(200);
}

void pushAnim(){
  matrix.clear();
    matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
    matrix.clear();
    matrix.drawBitmap(0, 0, smile2_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
    matrix.clear();
    matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
    matrix.clear();
    matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
}

void disconnectedAnim(){
  int wait = analogRead(A0);
  
    matrix.clear();
    matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, smile2_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
}

void connectedAnim(){
  
  int wait = 1000-analogRead(A0);
  boolean subscribed = pubsub.subscribe("update");
  //Serial.println(subscribed);
  
    matrix.clear();
    matrix.drawBitmap(0, 0, other_bmp1, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, other_bmp2, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, other_bmp3, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, other_bmp4, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, other_bmp5, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, other_bmp6, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
    matrix.clear();
    matrix.drawBitmap(0, 0, other_bmp7, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(wait);
}

