#include <Wire.h>


#define DEBUG

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

bool switchToPitchFlag=false;
bool switchToAlphaFlag=false;
bool pitchMode=false;
bool alphaMode=true;
bool pitchStop=true;


bool printFrequency=false;

float anglsetp=0.0; //Angle Set Point
float posdeg=0.0;
float anglediff=0;
unsigned long int freqDelay=25000;
unsigned long int freqDelaySmooth=25000;
unsigned long int lastStepTime=0;



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

void pitchFunction();

// the loop function runs over and over again forever
void loop() {

  
  if (alphaMode){
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
  }

  
  if (pitchMode){
    
   pitchFunction();
      
  }





  if (stringComplete) { 
    char DIRIN=inputString.charAt(0);
    Serial.print(DIRIN);
    Serial.print(" , ");
    isGoodCmd=false;
    if( DIRIN=='+'){
      Serial.print("Positive CMD Recognized , ");
      DIRVAL=HIGH;
      isGoodCmd=true;
      }
    else if ( DIRIN=='-') {
      Serial.print("Negative CMD Recognized , ");
      DIRVAL=LOW;
      isGoodCmd=true;
      }
    else if( DIRIN=='Z')
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
      }else if(DIRIN=='P'){
        Serial.println("Switching to pitch mode.Are you SURE?(Y/N)");
        switchToPitchFlag=true;
      }else if(DIRIN=='A'){
        Serial.println("Switching to alpha mode.Are you SURE?(Y/N)");
        switchToAlphaFlag=true;
      }else if( switchToPitchFlag==true){
        if(DIRIN=='Y'){
          alphaMode=false;
          pitchMode=true;  
          anglsetp=0;
          pitchStop=true;
           Serial.println("Pitch Mode Selected.");
        }else{
           Serial.println("Pitch Mode Switch Rejected.Nothing Changed!");
        }
        switchToPitchFlag=false;
      }

      if( switchToAlphaFlag==true){
        if(DIRIN=='Y'){
           pitchMode=false;
           alphaMode=true;  
           anglsetp=0;
           pitchStop=true;
           Serial.println("Alpha Mode Selected.");
        }else{
           Serial.println("Alpha Mode Switch Rejected.Nothing Changed!");
        }
        switchToAlphaFlag=false;
      }
    if (isGoodCmd){    
      anglsetp=inputString.substring(1).toFloat();
      if (!DIRVAL) anglsetp*=-1; 
      Serial.print("New Target Set To: ");
      Serial.print(anglsetp);
      freqDelay=1000000/anglsetp/400;
      if(pitchMode) pitchStop=false;
      
#ifdef DEBUG
        Serial.print("Frequency Delay=");
        Serial.print(freqDelay);
#endif
      lastStepTime=0;
      isGoodCmd=false;
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}




/*--- Pitch Mode Function---*/
void pitchFunction(){ 
    if(pitchStop){
      delay(100);
      return;
    }
    
      digitalWrite(DIR,DIRVAL); 
      unsigned long int timeNow=micros();
      if (lastStepTime==0 || timeNow-lastStepTime<0){
        lastStepTime=timeNow;
#ifdef DEBUG
        Serial.println("\nOut Of Loop");
#endif
      delay(100);
        return;
      }
      if(freqDelaySmooth==freqDelay){
        if(printFrequency){
//          Serial.println("Target Frequency Achived: ");
//          Serial.print(1000000.0/freqDelay/400.0);
//          Serial.println(" Hz");
          printFrequency=false; 
        }
      }else { /*if(freqDelaySmooth!=freqDelay)*/
       freqDelaySmooth+=((int)(freqDelay-freqDelaySmooth))*0.01;
        printFrequency=true;
//        Serial.print("Current Frequency: ");
//        Serial.print(1000000.0/freqDelaySmooth/400.0);
//        Serial.print(" Hz");
//        Serial.print("\t DelayInside Else!: ");
//        Serial.print(freqDelaySmooth);
//        Serial.println(" msec");
        
      }/*else if(freqDelaySmooth>freqDelay){
        freqDelaySmooth--;
        printFrequency=true;
      }*/


      unsigned long int loopDelay=freqDelaySmooth-timeNow+lastStepTime;
      lastStepTime=timeNow;
//      if (/*loopDelay<100 ||*/ loopDelay>500000){
//        Serial.print("Loop Delay Conditions Failed!");
//        Serial.println(loopDelay);
//        }
      if((int)loopDelay>0)
        delayMicroseconds(loopDelay);
      digitalWrite(PUL, HIGH);
      delayMicroseconds(3);
      digitalWrite(PUL, LOW);
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
