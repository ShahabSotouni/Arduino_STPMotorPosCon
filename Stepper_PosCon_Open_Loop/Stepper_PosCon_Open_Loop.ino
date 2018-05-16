#include <Wire.h>

volatile int slvmsg; // varaible used by the master to sent data to the slave

const int DIR=2;
int DIRVAL=LOW;
bool isGoodCmd=true;
const int PUL=3;
const int ENA=4;
const int REV=1600; //Steps Per Rev (Set DM2282 Switches Accordingly)
int dly=10000;
int runupdst=0;
int stepcnt=0;
int pos=0,oldpos=0;
long long int lststp=0;


float anglsetp=0.0; //Angle Set Point
float posdeg=0.0;
float anglediff=0;


String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

  double freq=0;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 
  pinMode(DIR, OUTPUT);
  digitalWrite(DIR, DIRVAL);  
  pinMode(PUL, OUTPUT);
  digitalWrite(PUL, LOW);  
  pinMode(ENA, OUTPUT);
  digitalWrite(ENA, LOW);  


  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

    Wire.begin();        // join i2c bus (address optional for master)

  delay(2000);

  Wire.beginTransmission (8);
        Wire.write (slvmsg);
        slvmsg=0;
        Wire.endTransmission ();   
        delay(500); 
        Serial.println("Zero Set CMD Issued");
  
}



void GetData();

// the loop function runs over and over again forever
void loop() {

  GetData();

    runupdst=abs(anglediff*20/0.9);
    if (abs(anglediff)-runupdst*0.9/20>0.5) runupdst++;

dly=8000;
   
  if (anglediff>0.0){
    DIRVAL=HIGH;
    digitalWrite(DIR,DIRVAL); 
  }
  else if (anglediff<0.0){
    DIRVAL=LOW;
    digitalWrite(DIR,DIRVAL);      
  }
    
   
    int runuptrg=runupdst;
    while (runupdst>0){
      
       digitalWrite(PUL, LOW);
       delayMicroseconds(dly);  
       digitalWrite(PUL, HIGH);
       delayMicroseconds(3);
       digitalWrite(PUL, LOW);
       if(DIRVAL==HIGH) pos+=10;
       if(DIRVAL==LOW)  pos-=10;
       if (dly>100)
          if(runuptrg>2*runupdst){
            dly++;
          }
          else {dly--;}
       runupdst--;
    }

 delay(1500);
 GetData();
  
while (abs(anglediff)<0.25 && !(abs(anglediff)<0.05)){

      if (anglediff>0.05){
             digitalWrite(DIR, HIGH);
             digitalWrite(PUL, LOW);
             delayMicroseconds(dly);  
             digitalWrite(PUL, HIGH);
             delayMicroseconds(3);
             digitalWrite(PUL, LOW);
        }
        else if (anglediff<-0.05){
             digitalWrite(DIR, LOW);
             digitalWrite(PUL, LOW);
             delayMicroseconds(dly);  
             digitalWrite(PUL, HIGH);
             delayMicroseconds(3);
             digitalWrite(PUL, LOW);
      
             
        }
        delay(1500);
GetData();
}


  if (stringComplete) {
    
    char DIRIN=inputString.charAt(0);
    Serial.print(DIRIN);
    Serial.print(" , ");
    isGoodCmd=true;
    if( DIRIN=='+'){  Serial.print("Positive CMD Recognized , ");DIRVAL=HIGH;}
    else if ( DIRIN=='-') {Serial.print("Negative CMD Recognized , "); DIRVAL=LOW;}
    else isGoodCmd=false;
    if( DIRIN=='Z')
      {  
        // send data to slave.
        slvmsg=2;
        Wire.beginTransmission(8);
        Wire.write(slvmsg);
        slvmsg=0;
        Wire.endTransmission ();   
        delay(500); 
        Serial.println("Zero Set CMD Issued");
        anglsetp=0;
        pos=0;
      }
    if (isGoodCmd){
    
    anglsetp=inputString.substring(1).toFloat();
    if (!DIRVAL) anglsetp*=-1; 
    Serial.print("New Target Set To: ");
    Serial.print(anglsetp);
    isGoodCmd=false;
    
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
}



/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
 
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      Serial.print("\n");
    }
  }
}



void GetData(){
  
  posdeg=pos*360.0/1000.0/4.0/20.0;
  anglediff=anglsetp-posdeg;
 if (pos!=oldpos){
    Serial.print("\nCurrent Position: ");
    Serial.println(posdeg);   //shows the current position
    oldpos=pos;
 }
   
}

