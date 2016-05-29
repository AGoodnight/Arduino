void setup() {
  // put your setup code here, to run once:

  pinMode(4,INPUT);
  pinMode(3,INPUT);
  pinMode(2,OUTPUT);
//  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  

  if( analogRead(0) > 0 ){
    digitalWrite(2,HIGH);
  }
  delay(1000);

}
