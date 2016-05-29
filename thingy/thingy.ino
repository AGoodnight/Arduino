

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(A1,INPUT);
  pinMode(A0,INPUT);
  
  pinMode(4,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(2,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  int light = analogRead(A1);
  int ir = analogRead(A0);
  
  Serial.println();
  Serial.print(light);
  Serial.print('\t');
  Serial.print(ir);
  Serial.println();
  
  if(light<400){
    digitalWrite(4,HIGH);
  }else{
    digitalWrite(4,LOW);
  } 
  
  if(light<100){
    digitalWrite(3,HIGH);
  }else{
    digitalWrite(3,LOW);
  }  

  if(ir != 1023){
    digitalWrite(2,HIGH);
  }else{
    digitalWrite(2,LOW);
  }   

}
