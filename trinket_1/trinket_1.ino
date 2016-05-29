#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <Twitter.h>
#include <util.h>
#include <w1500.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();
// the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  
//the IP address for the shield:
byte ip[] = { 10, 0, 0, 177 };   

void setup() {
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  Ethernet.begin(mac, ip);
  matrix.begin(0x70);  // pass in the address
}

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000},
  neutral_bmp[] =
  { B00000000,
    B11111111,
    B00000000,
    B11111111,
    B00000000,
    B11111111,
    B00000000,
    B11111111 },
  frown_bmp[] =
  { B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111 };

void loop() {

  int light = analogRead(A0);
  int other = analogRead(A1);
  
Serial.println( analogRead(light) );
Serial.println( analogRead(other) );
Serial.println();
Serial.println();

  if(light > 1000){
    matrix.clear();
    matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(500);
  }
  else if(light > 600){
    matrix.clear();
    matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(500);
  }
  else if(light > 200){
    matrix.clear();
    matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(500);
  }
}
