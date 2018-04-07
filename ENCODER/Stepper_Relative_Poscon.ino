#define encoder0PinA 2
#define encoder0PinB 3
volatile  int encoder0Pos = 0;
unsigned int tmp = 0;
unsigned int Aold = 0;
unsigned int Bnew = 0;
void setup() {
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);
  // set up the Serial Connection
  Serial.begin (115200);
}
void loop() {
  //Check each changes in position
  if (tmp != encoder0Pos) {
    double deg=encoder0Pos*360.0/1000.0/4.0/20.0;
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
