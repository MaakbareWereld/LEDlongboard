/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor

 This example code is in the public domain.
 */

#define MAXSPEED 8.3
#define WHEELDIAMETER 0.060
#define NRMAGNETS 4.0

#define DEBUG true

// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 2;

bool state = LOW;

int outPin = 9;
unsigned long t0;
volatile int counter = 0;
const unsigned long waitTime = 250; //ms
int maxRevelations;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  if (DEBUG) Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
  pinMode(outPin, OUTPUT);

  maxRevelations = (int)((MAXSPEED * NRMAGNETS * (float)waitTime) / (3.141 * WHEELDIAMETER * 1000.0));
  if (DEBUG) Serial.println(maxRevelations);
  attachInterrupt(digitalPinToInterrupt(pushButton),addCounter,RISING);
  t0 = millis();
}

// the loop routine runs over and over again forever:
void loop() {
  /*
  bool newState = digitalRead(pushButton);
  if (newState != state){
    if (newState == HIGH) counter++;
    state = newState;
  }
  */

  
  if (((millis() - t0) % waitTime)==0){
    analogWrite(outPin,constrain(map(counter, 0, maxRevelations, 0, 255),0,255));
    if (DEBUG) Serial.println(counter);
    counter =0;
    t0=millis();
    delay(1);
  }
}

void addCounter(){
  counter++;
}


