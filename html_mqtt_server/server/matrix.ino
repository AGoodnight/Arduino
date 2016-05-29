static const uint8_t PROGMEM
connected_bmp1[] =
  { B00011000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  connected_bmp2[] =
  { B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  connected_bmp3[] =
  { B00011000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  connected_bmp4[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  connected_bmp5[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00111100,
    B01111110 },
  connected_bmp6[] =
  { B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00111100,
    B01111110 },
  connected_bmp7[] =
  { B00000000,
    B00011000,
    B00011000,
    B00011000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 };

static const uint8_t PROGMEM
disconnected_bmp1[] =
  { B00011000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  disconnected_bmp2[] =
  { B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  disconnected_bmp3[] =
  { B00011000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 },
  disconnected_bmp4[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000 },
  disconnected_bmp5[] =
  { B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000 },
  disconnected_bmp6[] =
  { B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00111100,
    B01111110 },
  disconnected_bmp7[] =
  { B00000000,
    B00011000,
    B00011000,
    B00011000,
    B00000000,
    B00011000,
    B00111100,
    B01111110 };


void connectedAnim(){
  
  int wait = 1000-analogRead(A0);
  boolean subscribed = pubsub.subscribe("update");
  //Serial.println(subscribed);
  matrix.clear();
  matrix.drawBitmap(0, 0, connected_bmp1, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(wait);

  matrix.clear();
  matrix.drawBitmap(0, 0, connected_bmp2, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(wait);

  matrix.clear();
  matrix.drawBitmap(0, 0, connected_bmp3, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(wait);

  matrix.clear();
  matrix.drawBitmap(0, 0, connected_bmp4, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(wait);

  matrix.clear();
  matrix.drawBitmap(0, 0, connected_bmp5, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(wait);

  matrix.clear();
  matrix.drawBitmap(0, 0, connected_bmp6, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(wait);
}


void disconnectedAnim(){
  
  int wait = 1000-analogRead(A0);
  //Serial.println(subscribed);
  matrix.clear();
  matrix.drawBitmap(0, 0, disconnected_bmp5, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(wait);
  matrix.clear();
  matrix.drawBitmap(0, 0, disconnected_bmp4, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(wait);
}


