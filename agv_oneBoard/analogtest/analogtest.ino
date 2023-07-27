const int duration =1000;
unsigned long pre_time =0;
unsigned long cur_time =0;  // Chattering prevention



//line to arduino
const int LineSensor1=A11;
const int LineSensor2=A10;
const int LineSensor3=A9;
const int LineSensor4=A8;
const int LineSensor5=A7;
const int LineSensor6=A6;
const int LineSensor7=A5;
const int LineSensor8=A4;
const int LineSensor9=A3;
const int LineSensor10=A2;
const int LineSensor11=A1;
const int LineSensor12=A0;

// arduino to converter
const int converter1=22;
const int converter2=24;
const int converter3=26;
const int converter4=28;
const int converter5=30;
const int converter6=32;
const int converter7=34;
const int converter8=36;
const int converter9=38;
const int converter10=40;
const int converter11=42;
const int converter12=44;


const int FrontSensor1 = 48;
const int FrontSensor2 = 50;
const int FrontSensor3 = 47;
const int Stop = 52;
const int button_in = 53;
const int button_out = 51;

void Sensing(){ 
  if(!digitalRead(LineSensor1)){
    digitalWrite(converter1,HIGH); 
    Serial.println("1111111111111");
  }
  else digitalWrite(converter1,LOW);

  if(!digitalRead(LineSensor2)){
    digitalWrite(converter2,HIGH);
  }
  else digitalWrite(converter2,LOW);
  
  if(!digitalRead(LineSensor3)){
    digitalWrite(converter3,HIGH);
        Serial.println("333");
  }
  else digitalWrite(converter3,LOW);
  
  if(!digitalRead(LineSensor4)){
    digitalWrite(converter4,HIGH);
        Serial.println("4444");
  }
  else digitalWrite(converter4,LOW);
 
  if(!digitalRead(LineSensor5)){
    digitalWrite(converter5,HIGH);
        Serial.println("555");
  }
  else digitalWrite(converter5,LOW);
  
  if(!digitalRead(LineSensor6)){
    digitalWrite(converter6,HIGH);
        Serial.println("666");
  }
  else digitalWrite(converter6,LOW);
  
  if(!digitalRead(LineSensor7)){
    digitalWrite(converter7,HIGH);
        Serial.println("77777");
  }
  else digitalWrite(converter7,LOW);
 
  if(!digitalRead(LineSensor8)){
    digitalWrite(converter8,HIGH);
  }
  else digitalWrite(converter8,LOW);
  
  if(!digitalRead(LineSensor9)){
    digitalWrite(converter9,HIGH);
  }
  else digitalWrite(converter9,LOW);
  
  if(!digitalRead(LineSensor10)){
    digitalWrite(converter10,HIGH);
  }
  else digitalWrite(converter10,LOW);
  
  if(!digitalRead(LineSensor11)){
    digitalWrite(converter11,HIGH);
  }
  else digitalWrite(converter11,LOW);
  
  if(!digitalRead(LineSensor12)){
    digitalWrite(converter12,HIGH);
  }
  else digitalWrite(converter12,LOW);
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  pinMode(LineSensor1, INPUT);
  pinMode(LineSensor2, INPUT);
  pinMode(LineSensor3, INPUT);
  pinMode(LineSensor4, INPUT);
  pinMode(LineSensor5, INPUT);
  pinMode(LineSensor6, INPUT);
  pinMode(LineSensor7, INPUT);
  pinMode(LineSensor8, INPUT);
  pinMode(LineSensor9, INPUT);
  pinMode(LineSensor10, INPUT);
  pinMode(LineSensor11, INPUT);
  pinMode(LineSensor12, INPUT);

  pinMode(FrontSensor1, INPUT);
  pinMode(FrontSensor2, INPUT);

  pinMode(Stop,OUTPUT);

  pinMode(button_in,INPUT_PULLUP);
  pinMode(button_out,OUTPUT);
 
  pinMode(converter1, OUTPUT);
  pinMode(converter2, OUTPUT);
  pinMode(converter3, OUTPUT);
  pinMode(converter4, OUTPUT);
  pinMode(converter5, OUTPUT);
  pinMode(converter6, OUTPUT);
  pinMode(converter7, OUTPUT);
  pinMode(converter8, OUTPUT);
  pinMode(converter9, OUTPUT);
  pinMode(converter10, OUTPUT);
  pinMode(converter11, OUTPUT);
  pinMode(converter12, OUTPUT);

}

void loop() {   
  // Sensing()함수를 호출해서 적외선센서값을 읽습니다
  digitalWrite(button_out,HIGH);
  Sensing();
  

  //digitalWrite(22,HIGH);
  //digitalWrite(24,HIGH);
 


  if((!digitalRead(FrontSensor1))&& (!digitalRead(FrontSensor2))&& (!digitalRead(FrontSensor3)) )
  {    
    digitalWrite(Stop,HIGH);
    //Serial.println("222");
  }
  else{
    digitalWrite(Stop,LOW);
    //Serial.println("33333");
  }

  
  if(digitalRead(button_in) == LOW)
  {
    
    digitalWrite(button_out, LOW);
    Serial.println("ONONONONONONO");
  }
  else 
  {
    digitalWrite(button_out, HIGH);
    Serial.println("OFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
  }
  
}

void stopbutton()
{

  cur_time = millis();
  if(cur_time - pre_time >= duration)
  {
    digitalWrite(button_out, HIGH);
    pre_time =cur_time;
  }

 
}
