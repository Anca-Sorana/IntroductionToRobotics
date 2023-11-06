// declare all the joystick pins
const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output
// declare all the segments pins
const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;
const int segSize = 8;
const int shortPressTime = 1000;
// modify if you have common anode
bool commonAnode = false;
bool joyMoved = false;

byte state = HIGH;
byte dpState = LOW;
byte swState = LOW;
byte lastSwState = LOW;
byte ledState = LOW;

int xValue = 0;
int yValue = 0;
int currentSegment = 7;
int minThreshold = 400;
int maxThreshold = 600;
int interval = 100;

unsigned long currentMillis;        //used for blinky segment
unsigned long previousMillis;       //used for blinky segment
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP   //used to memorize the segments pins
};

byte segmentsState[segSize] = {
  0, 0, 0, 0, 0, 0, 0, 0                            //used to memorize the segments state
};

int directions[8][4] = {                            
  //UP DOWN LEFT  RIGHT
  {-1,  6,    5,    1}, //a                         //used to memorize what segment we can
  {0,   6,    5,   -1}, //b                         //access and the move we have to do
  {6,   3,    4,    7}, //c                         //a -> 0, b -> 1, c -> 2, d -> 3  
  {6,  -1,    4,    2}, //d                         //e -> 4, f -> 5, g -> 6, dp -> 7
  {6,   3,   -1,    2}, //e                         //-1 -> N/A
  {0,   6,   -1,    1}, //f
  {0,   3,   -1,   -1}, //g
  {-1, -1,    2,   -1}  //dp
};
void setup() {
  // initialize all the pins
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  if (commonAnode == true) {
    state = !state;
  }
}
void loop() {
  xValue = analogRead(pinY);                                    //read the value from the pins
  yValue = analogRead(pinX);
  currentMillis = millis();
  if (xValue < minThreshold && joyMoved == false) {             //move to the left
    if(directions[currentSegment][2] >= 0) {
      currentSegment = directions[currentSegment][2];
    }
    joyMoved = true;
  }
  if (xValue > maxThreshold && joyMoved == false) {             //move to the right
    if(directions[currentSegment][3] >= 0) {
      currentSegment = directions[currentSegment][3];
    }
    joyMoved = true;
  }
  if (yValue > maxThreshold && joyMoved == false) {             //move up
    if(directions[currentSegment][0] >= 0) {
      currentSegment = directions[currentSegment][0];
    }
    joyMoved = true;
  }

  if (yValue < minThreshold && joyMoved == false) {             //move down
    if(directions[currentSegment][1] >= 0) {
      currentSegment = directions[currentSegment][1];
    }
    joyMoved = true;
  }
  if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }
  swState = digitalRead(pinSW);                                 //button pressed
  if (swState == LOW && lastSwState == HIGH) {
    pressedTime = millis();
  }
    if (swState == HIGH && lastSwState == LOW) {                //check if it is a long press or short press
      releasedTime = millis();
      long pressDuration = releasedTime - pressedTime;          
      if( pressDuration < shortPressTime ) {
        segmentsState[currentSegment] = !segmentsState[currentSegment];
      }
      else {
        for(int i = 0; i < segSize; i++) {
          segmentsState[i] = 0;
        }
        currentSegment = 7;
      }
    }
  lastSwState = swState;
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(segments[currentSegment], ledState);
  }
  displayNumber(currentSegment, dpState);
  delay(1);
}
void displayNumber(byte currentSegment, byte decimalPoint) {             
  for (int i = 0; i < segSize; i++) {
    if(i != currentSegment)    //let the current segment blink
      digitalWrite(segments[i], segmentsState[i]);
  }
  
}
