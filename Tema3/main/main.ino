const int buzzerPin = 11;

const int transistionLedPin = 12;
const int firstFloorLedPin = 5;
const int secondFloorLedPin = 6;
const int thirdFloorLedPin = 7;

const int firstFloorButtonPin = 2;
const int secondFloorButtonPin = 3;
const int thirdFloorButtonPin = 4;

byte readingFirstFloorButton = LOW;
byte lastReadingFirstFloorButton = LOW;
byte firstFloorButtonState = LOW;
byte readingSecondFloorButton = LOW;
byte lastReadingSecondFloorButton = LOW;
byte secondFloorButtonState = LOW;
byte readingThirdFloorButton = LOW;
byte lastReadingThirdFloorButton = LOW;
byte thirdFloorButtonState = LOW;

unsigned int lastDebounceTimeFirstFloorButton = 0;
unsigned int lastDebounceTimeSecondFloorButton = 0;
unsigned int lastDebounceTimeThirdFloorButton = 0;
unsigned int lastDebounceTimeLed = 0;
unsigned int lastDebounceTimeBuzzer = 0;
unsigned int lastDebounceTimeFloorsLed = 0;
unsigned int lastDebounceTimeTransitionLed = 0;
unsigned int lastDebounceTimeElevator = 0;
unsigned int debounceDelayButton = 50;
unsigned int debounceDelayLed = 1000;
unsigned int debounceDelayBuzzer = 1000;
unsigned int debounceDelayTransitionLed = 500;
unsigned int debounceDelayFloorsLed = 1500;
unsigned int debounceDelayElevator = 1000;
unsigned int startQueue = 0;
unsigned int endQueue = 0;
unsigned int queue[100];
unsigned int currentFloor = 1;
unsigned int currentMillis;

int increment;                  //used to know if we go down or up with the elevator
int endFloor;                   //destination
int startFloor = 5;             //departure

byte ledStateTransition = LOW;            //led state corresponding to each button
byte ledStateElevator = HIGH;   //led state corresponding to the transition between each floor
int count = 0;

bool elevatorWorking = 0;
void setup() {

  pinMode(buzzerPin, OUTPUT);

  pinMode(transistionLedPin, OUTPUT);
  pinMode(firstFloorLedPin, OUTPUT);
  pinMode(secondFloorLedPin, OUTPUT);
  pinMode(thirdFloorLedPin, OUTPUT);

  pinMode(firstFloorButtonPin, INPUT);
  pinMode(secondFloorButtonPin, INPUT);
  pinMode(thirdFloorButtonPin, INPUT);

  Serial.begin(9600);
}

void loop() {
  readingFirstFloorButton = digitalRead(firstFloorButtonPin);                       //reading the state of the buttons
  readingSecondFloorButton = digitalRead(secondFloorButtonPin);
  readingThirdFloorButton = digitalRead(thirdFloorButtonPin);
  currentMillis = millis();
  if (readingFirstFloorButton != lastReadingFirstFloorButton) {                     //checking if the buttons are pressed and save their pin values in a queue
    lastDebounceTimeFirstFloorButton = currentMillis;
  }
  if ((currentMillis - lastDebounceTimeFirstFloorButton) > debounceDelayButton) {
    if (readingFirstFloorButton != firstFloorButtonState) {
      firstFloorButtonState = readingFirstFloorButton;
      if (readingFirstFloorButton != LOW) {
        queue[endQueue++] = firstFloorLedPin;
      }
    }
  }
  lastReadingFirstFloorButton = readingFirstFloorButton;

  if (readingSecondFloorButton != lastReadingSecondFloorButton) {
    lastDebounceTimeSecondFloorButton = currentMillis;
  }
  if ((currentMillis - lastDebounceTimeSecondFloorButton) > debounceDelayButton) {
    if (readingSecondFloorButton != secondFloorButtonState) {
      secondFloorButtonState = readingSecondFloorButton;
      if (readingSecondFloorButton != LOW) {
        queue[endQueue++] = secondFloorLedPin;
      }
    }
  }
  lastReadingSecondFloorButton = readingSecondFloorButton;

  if (readingThirdFloorButton != lastReadingThirdFloorButton) {
    lastDebounceTimeThirdFloorButton = currentMillis;
  }
  if ((currentMillis - lastDebounceTimeThirdFloorButton) > debounceDelayButton) {
    if (readingThirdFloorButton != thirdFloorButtonState) {
      thirdFloorButtonState = readingThirdFloorButton;
      if (readingThirdFloorButton != LOW) {
        queue[endQueue++] = thirdFloorLedPin;
      }
    }
  }
  lastReadingThirdFloorButton = readingThirdFloorButton;

  if (startQueue != endQueue && !elevatorWorking) {                                           //check if there are values saved in the queue and if the elevator is working
    if (currentMillis - lastDebounceTimeElevator >= debounceDelayElevator)
    {
      endFloor = queue[startQueue];                                                             //extract the first value from the queue (the destination)
      elevatorWorking = true;
      lastDebounceTimeElevator = currentMillis;
      tone(buzzerPin, 1000);                                                                    //turn on the buzzer
      startQueue++;
      lastDebounceTimeFloorsLed = currentMillis;
      if (endFloor > startFloor) {                                                              //verify if the elevator is going up or down
        increment = 1;
      } else {
        increment = -1;
      }
    }

  }

  if (elevatorWorking) {                                                                      //while the elevator is working
    if (currentMillis - lastDebounceTimeTransitionLed >= debounceDelayTransitionLed) {        //make the transition led blink
      lastDebounceTimeTransitionLed = currentMillis;
      ledStateTransition = !ledStateTransition;
      digitalWrite(transistionLedPin, ledStateTransition);
    }
    digitalWrite(startFloor, ledStateElevator);
    if (currentMillis - lastDebounceTimeFloorsLed >= debounceDelayFloorsLed) {              //make the leds that correspond to every floor blink twice
      lastDebounceTimeFloorsLed = currentMillis;
      if (ledStateElevator == LOW) {
        ledStateElevator = HIGH;
        startFloor += increment;                                                            //go to the next floor after the led had been turned on and off
      } else {
        ledStateElevator = LOW;
      }
    }
    if (abs(endFloor - startFloor) == 0) {                                                  //arrive at the destination and start the buzzer at a different frequency
      elevatorWorking = false;
      lastDebounceTimeElevator = currentMillis;
      tone(buzzerPin, 2000);
      lastDebounceTimeBuzzer = currentMillis;
    }
  } else if (!elevatorWorking) {                                                            //while the elevator is not working
    digitalWrite(startFloor, HIGH);                                                         //keep the led for the current floor on
    digitalWrite(transistionLedPin, HIGH);                                                  //and also the transition led
    if (currentMillis - lastDebounceTimeBuzzer > debounceDelayBuzzer) {                     //turn off the buzzer after 1 second after the elevator arrived at the destination
      lastDebounceTimeBuzzer = currentMillis;
      noTone(buzzerPin);
    }
  }

}
