#include <Wire.h>
#define encoder0PinA 2
#define encoder0PinB 3
volatile  int encoder0Pos = 0;
unsigned int tmp = 0;
unsigned int Aold = 0;
unsigned int Bnew = 0;
double deg;
int degint=12345;


byte flag = 0;

//Blink LED
void blinkled()
{
  if(flag)
  {
    flag = 0;
    digitalWrite(13, LOW);
  }
  else
  {
    flag = 1;
    digitalWrite(13, HIGH);
  }
}


void setup() {
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);
  // set up the Serial Connection
  Serial.begin (115200);
  Wire.begin(0x38);             // i2c in slave mode address is 0x38
  Wire.onRequest(requestEvent);
}

void loop() {
  //Check each changes in position
  if (tmp != encoder0Pos) {
    deg=encoder0Pos*360.0/1000.0/4.0/20.0;
    degint=100*deg;
    Serial.println(deg);
    tmp = encoder0Pos;
  }
  delay(100);
}
// Interrupt on A changing state
void doEncoderA() {
  Bnew^Aold ? encoder0Pos++ : encoder0Pos--;
  Aold = digitalRead(encoder0PinA);
}

// Interrupt on B changing state
void doEncoderB() {
  Bnew = digitalRead(encoder0PinB);
  Bnew^Aold ? encoder0Pos++ : encoder0Pos--;
}

void requestEvent()
{
  byte sendhi = 0, sendli = 0;
  byte sendbyte[3];
  int16_t tempreading_deg;
  tempreading_deg=degint;
  sendhi=tempreading_deg>>8;
  sendli=tempreading_deg&0xff;
  sendbyte[0]=sendhi;
  sendbyte[1]=sendli;
  sendbyte[2]=(sendhi+sendli)&0xff;
  Wire.write(sendbyte,3); 
  blinkled();
}
