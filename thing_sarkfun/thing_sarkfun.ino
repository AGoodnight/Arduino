const int led = 4;
const int btn = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  pinMode(btn,INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(btn) == HIGH){
    digitalWrite(led,HIGH);
  }else{
    digitalWrite(led,LOW);
  }
}
