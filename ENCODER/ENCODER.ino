#include <Wire.h>

#define encoder0PinA 2
#define encoder0PinB 3


char t[7]; //empty array where to put the numbers going to the master
volatile int Val; // variable used by the master to sent data to the slave

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
 Wire.begin(8);                // Slave id #8
  Wire.onRequest(requestEvent); // fucntion to run when asking for data
  Wire.onReceive(receiveEvent); // what to do when receiving data
}






void loop() {
  //Check each changes in position
  if (tmp != encoder0Pos) {
    deg=encoder0Pos*360.0/1000.0/4.0/20.0;
    degint=100*deg;
    Serial.print(encoder0Pos);
    tmp = encoder0Pos;
    Serial.print(" : ");
    Serial.println(t);
  }
  delay(100);
   uint16_t x = encoder0Pos;//(millis()%1000000)/100; //generate a uint16_t number
  dtostrf(x, 6, 0, t); //converts the float or integer to a string. (floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, empty array);

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




// function: what to do when asked for data
void requestEvent() {
Wire.write(t); 
}





// what to do when receiving data from master
void receiveEvent(int howMany)
{Val = Wire.read();
if (Val==2) {
  encoder0Pos=0;
  Serial.println("Reset Command Recieved");
  }
  Val=0;
}


