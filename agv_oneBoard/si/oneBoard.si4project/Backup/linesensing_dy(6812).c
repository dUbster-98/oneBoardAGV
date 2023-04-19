const int LineSensor1=22;
const int LineSensor2=24;
const int LineSensor3=26;
const int LineSensor4=28;
const int LineSensor5=30;
const int LineSensor6=32;
const int LineSensor7=34;
const int LineSensor8=36;
const int LineSensor9=38;
const int LineSensor10=40;
const int LineSensor11=42;
const int LineSensor12=44;


const int LMOTOR_SPEED = 2;      // 모터 핀 번호    전압을 높일수록 속도 up
const int LmotorSetDRIVE=4;
const int LmotorSetBREAKE=5;  //  브레이크  입력,  low 주면 정지
const int LmotorSetDIR=3; // 방향 선택 low 주면 역방향, high 주면 정방향
const int RmotorSetDRIVE=9;
const int RmotorSetBREAKE=8;  //  브레이크  입력,  low 주면 정지
const int RmotorSetDIR=7; // 방향 선택 low 주면 역방향, high 주면 정방향
const int RMOTOR_SPEED = 6;



int a,b,c,d,e,f,g,h,i,j,k,l;
int rightFlag = 0;
int leftFlag = 0;

int leftLinsensor=0;
int rightLinsensor=0;

struct SENSOR{
  union{
    int R; // union으로 int R과 struct B가 메모리를 공유합니다. R은 B에 접근하기 위한 이름으로만 사용하는 걸로 보입니다.
    struct{
      unsigned sensor1:1; // 참고한 사이트에서는 bit field 사용 시 unsigned인지 signed인지 명시하는 걸 권장한다고 합니다.
      unsigned sensor2:1; // 따라서 unsigned 1bit로 변경해 주었습니다.
      unsigned sensor3:1;
      unsigned sensor4:1;
      unsigned sensor5:1;
      unsigned sensor6:1;
      unsigned sensor7:1;
      unsigned sensor8:1;
      unsigned sensor9:1;
      unsigned sensor10:1;
      unsigned sensor11:1;
      unsigned sensor12:1;
    } B;
  } sen;
};
struct SENSOR tracesensor;

// 적외선 센서값을 읽어서 tracesensor 구조체 안에 저장하는 함수. 값을 읽을 경우 해당 위치에 1이 저장됩니다
void Sensing(){
  a=0;b=0;c=0;d=0;e=0;f=0;g=0;h=0;i=0;j=0;k=0;l=0;
 
  if(!digitalRead(LineSensor1)){
//    Serial.print("1 ");
    a = 1;
  }
  if(!digitalRead(LineSensor2)){
 //   Serial.print("2 ");
    b = 1;
  }
  if(!digitalRead(LineSensor3)){
//    Serial.print("3 ");
    c = 1;
  }
  if(!digitalRead(LineSensor4)){
//    Serial.print("4 ");
    d = 1;
  }
 
  if(!digitalRead(LineSensor5)){
//    Serial.println("5");
    e = 1;
  }
  if(!digitalRead(LineSensor6)){
//    Serial.print("6 ");
    f = 1;
  }
  if(!digitalRead(LineSensor7)){
//    Serial.print("7 ");
    g = 1;
  }
 
  if(!digitalRead(LineSensor8)){
//    Serial.println("8");
    h = 1;
  }
  if(!digitalRead(LineSensor9)){
//    Serial.println("8");
    i = 1;
  }
  if(!digitalRead(LineSensor10)){
//    Serial.println("8");
    j = 1;
  }
  if(!digitalRead(LineSensor11)){
//    Serial.println("8");
    k = 1;
  }
  if(!digitalRead(LineSensor12)){
//    Serial.println("8");
    l = 1;
  }


   
  tracesensor.sen.B.sensor1 = a;
  tracesensor.sen.B.sensor2 = b;
  tracesensor.sen.B.sensor3 = c;
  tracesensor.sen.B.sensor4 = d;
  tracesensor.sen.B.sensor5 = e;
  tracesensor.sen.B.sensor6 = f;
  tracesensor.sen.B.sensor7 = g;
  tracesensor.sen.B.sensor8 = h;
  tracesensor.sen.B.sensor9 = i;
  tracesensor.sen.B.sensor10 = j;
  tracesensor.sen.B.sensor11 = k;
  tracesensor.sen.B.sensor12 = l;

  leftLinsensor =a+b+c+d+e+f;
  rightLinsensor = g+h+i+j+k+l;
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

  pinMode(LMOTOR_SPEED, OUTPUT);
  pinMode(LmotorSetDRIVE, OUTPUT);
  pinMode(LmotorSetBREAKE, OUTPUT);
  pinMode(LmotorSetDIR, OUTPUT); 

  digitalWrite(LmotorSetDRIVE,LOW);
  digitalWrite(LmotorSetBREAKE,LOW);
  digitalWrite(LmotorSetDIR,LOW);

  pinMode(RMOTOR_SPEED, OUTPUT);
  pinMode(RmotorSetDRIVE, OUTPUT);
  pinMode(RmotorSetBREAKE, OUTPUT);
  pinMode(RmotorSetDIR, OUTPUT); 

  digitalWrite(RmotorSetDRIVE,LOW);
  digitalWrite(RmotorSetBREAKE,LOW);
  digitalWrite(RmotorSetDIR,HIGH);


}

void loop() { 
  // Sensing()함수를 호출해서 적외선센서값을 읽습니다
  Sensing();
 
  Serial.println(tracesensor.sen.R, BIN);

  
  rightDrive();
  
 
}

void leftyDrive()
{
  switch(tracesensor.sen.R){
    // 전부 흰색인 경우
    case 0b000000000000:
      Serial.print("        gogo             ");
       goMotor();
 
    break;
 
    // 전부 검정색인 경우
    case 0b111111111111:
      if(leftFlag > rightFlag)
      {
    
         Serial.print("           LEFT 2      ");
         leftFastMotor();
         
      }
      else if(leftFlag < rightFlag)
      {
         Serial.print("           right 2      ");
          rightFastMotor();
      }

      break;
    case 0b111111111110:
    Serial.print("           LEFTY 2      ");
          leftFlag++;
          leftFastMotor();
      break;
            
    case 0b111111111100:
    case 0b111111111001:
      Serial.println("        LEFTY 1       ");
      leftFlag++;
      leftMotor();
 
      break;
 


 
    // lefty 직선
    case 0b111111110011:    
    case 0b111111100111:
    case 0b111111001111:
    case 0b111110011111:
      Serial.println("gogo");
      leftFlag=0;
      rightFlag=0;
       goMotor();
      break;
      
    case 0b111100111111:
    case 0b111001111111:
      Serial.println("        righty 1       ");
      rightFlag++;
      rightMotor();
     break;
    case 0b110011111111:
    case 0b100111111111:
      Serial.println("        righty 2       ");
      rightFlag++;
       rightFastMotor();
      break;      

    case 0b001111111111:
    case 0b011111111111:
      Serial.println("        righty 2       ");
      rightFlag++;
       rightFastMotor();
      break;

    default:
      Serial.println("??");

      if(leftLinsensor<rightLinsensor) //(leftFlag > rightFlag)
      {
      Serial.println("        lefty 1       ");
      leftMotor();
      }
      else if(leftLinsensor>rightLinsensor) //(leftFlag < rightFlag)
      {
              Serial.println("        righty 1       ");
              rightMotor();
      }
      else 
      {
              Serial.println("gogo");
               goMotor();
      }

      break;
  }
    


  
}





void rightDrive()
{
  switch(tracesensor.sen.R){
    // 전부 흰색인 경우
    case 0b000000000000:
      Serial.print("        gogo             ");
      goMotor();
 
    break;
 
    // 전부 검정색인 경우
    case 0b111111111111:
      if(leftFlag > rightFlag)
      {
    
         Serial.print("           LEFT 2      ");
         leftFastMotor();
         
      }
      else if(leftFlag < rightFlag)
      {
         Serial.print("           right 2      ");
         rightFastMotor();
      }

      break;
    case 0b111111111110:

    Serial.print("           LEFTY 2      ");
        leftFlag++;
        leftFastMotor();
      break;
      
    case 0b111111111100:        
   
      Serial.println("        LEFTY 1       ");
      leftFlag++;
      leftMotor();
      
 
      break;
 


 
   
    case 0b111111111001:
    case 0b111111110011:  
    case 0b111111100111:
      Serial.println("        gogo      ");
       leftFlag=0;
      rightFlag=0;
       goMotor();
 
      break;

    
    case 0b111111001111:
    rightFlag++;
      rightMotor();
      break;   


    
    case 0b111110011111:
     rightFlag++;
      rightFastMotor();
      break;

    
    case 0b111100111111:
    case 0b111001111111:  
    case 0b110011111111:
    case 0b100111111111:
    case 0b001111111111:
    case 0b011111111111:


      Serial.println("        righty 2       ");
      rightFlag++;
      rightFastMotor();
      break;      

    default:
      Serial.println("??");

      if(leftLinsensor =4 && rightLinsensor <4)
      {
         rightMotor();
      }
      else if(leftLinsensor<rightLinsensor) //(leftFlag > rightFlag)
      {
      Serial.println("        lefty 1       ");
      leftMotor();
      }
      else if(leftLinsensor>rightLinsensor) //(leftFlag < rightFlag)
      {
              Serial.println("        righty 1       ");
              rightMotor();
      }
      else 
      {
              Serial.println("gogo");
               goMotor();
      }

      break;
  }

  
    
  


}



void goMotor()
{


 analogWrite(LMOTOR_SPEED,100);
 analogWrite(RMOTOR_SPEED,100);
 //delay(2000);

  
}


void leftMotor()
{


 analogWrite(LMOTOR_SPEED,50);
 analogWrite(RMOTOR_SPEED,100);
// delay(2000);

  
}



void leftFastMotor()
{


 analogWrite(LMOTOR_SPEED,30);
 analogWrite(RMOTOR_SPEED,100);
 

  
}


void rightMotor()
{


 analogWrite(LMOTOR_SPEED,100);
 analogWrite(RMOTOR_SPEED,50);


  
}




void rightFastMotor()
{


 analogWrite(LMOTOR_SPEED,100);
 analogWrite(RMOTOR_SPEED,30);


  
}