
const int DIR=2;
int DIRVAL=LOW;
bool isGoodCmd=true;
const int PUL=3;
const int ENA=4;
const int REV=1600; //Steps Per Rev (Set DM2282 Switches Accordingly)
int dly=10000;
int runupdst=0;
int stepcnt=0;
long long int lststp=0;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

  double freq=0;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
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
  
}

// the loop function runs over and over again forever
void loop() {


 /* if (dly>dly_des) dly--;
  if (dly<dly_des) dly++;
 */
 
 /* Serial.print(dly);
 

  
  

  stepcnt++;

     freq=1000000.0/((micros()-lststp)*400.0);//
     stepcnt=0;
     lststp=micros();
   Serial.print(" , ");
   Serial.print(stepcnt);
   Serial.print(" , ");
   Serial.print(freq);
   Serial.print("\n");
  */

  /*
  digitalWrite(PUL, LOW);
  delayMicroseconds(dly);  
  digitalWrite(PUL, HIGH);
  delayMicroseconds(3);
  digitalWrite(PUL, LOW);
  */
  
/*delay(500);
delayMicroseconds(2);
  DIRVAL=!DIRVAL;
  digitalWrite(DIR, DIRVAL);
  */
  if (stringComplete) {
    
    char DIRIN=inputString.charAt(0);
    Serial.print(DIRIN);
    Serial.print(" , ");
    isGoodCmd=true;
    if( DIRIN=='P'){  Serial.print("Positive CMD Recognized , ");DIRVAL=HIGH;}
    else if ( DIRIN=='N') {Serial.print("Negative CMD Recognized , "); DIRVAL=LOW;}
    else isGoodCmd=false;

    if (isGoodCmd){
      Serial.print("\"Is Good CMD\"  ");
    
    float anglediff=0;
    anglediff=inputString.substring(1).toFloat();
    runupdst=anglediff*20/0.9;
    if (anglediff-runupdst*0.9/20>0.5) runupdst++;
    Serial.print(runupdst);
    Serial.print(" Steps, Possible Move: ");
    Serial.print(runupdst*0.9/20);
    Serial.print(" DEG");
    Serial.print("\n");
    isGoodCmd=false;
    digitalWrite(DIR, DIRVAL);
    dly=6000;
    int runuptrg=runupdst;
    while (runupdst>0){
       digitalWrite(PUL, LOW);
       delayMicroseconds(dly);  
       digitalWrite(PUL, HIGH);
       delayMicroseconds(3);
       digitalWrite(PUL, LOW);
       if (dly>100)
          if(runuptrg>2*runupdst){
            dly++;
          }
          else {dly--;}

           Serial.print(dly);
            Serial.print("\n");
       runupdst--;
    }
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
    }
  }
}


