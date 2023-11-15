// Define connections to the shift register
const int latchPin = 11; // Connects to STCP (latch pin) on the shift register
const int clockPin = 10; // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12; // Connects to DS (data pin) on the shift register

// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;
const int startStopButtonPin = 3;
const int resetButtonPin = 8;
const int lapButtonPin = 9;

// Store the digits in an array for easy access
int displayDigits[] = {segD1, segD2, segD3, segD4};
const int displayCount = 4; // Number of digits in the display

// Define the number of unique encodings (0-9, A-F for hexadecimal)
const int encodingsNumber = 16;
// Define byte encodings for the hexadecimal characters 0-F
byte byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110  // F
};
// Variables for controlling the display update timing
unsigned long lastIncrement = 0;
unsigned long delayCount = 100; // Delay between updates (milliseconds)
unsigned long number = 0; // The number being displayed
unsigned int readingStartButton;
unsigned int lastReadingStartButton;
unsigned int lastDebounceStartButton;

unsigned int readingResetButton;
unsigned int lastReadingResetButton;
unsigned int lastDebounceResetButton;

unsigned int readingLapButton;
unsigned int lastReadingLapButton;
unsigned int lastDebounceLapButton;

unsigned int debounceDelayButton = 50;
unsigned int startState;
unsigned int resetState;

byte startButtonState = false;
byte resetButtonState = false;
byte lapButtonState = false;
byte lapState = false;

int lapsSave[5];
int lapsMax = 5;
int count;

// Function prototypes
void initializePins();
void readButtons();
void handleStartButton();
void handleResetButton();
void handleLapButton();
void updateDisplay();
void writeReg(int digit);
void activateDisplay(int displayNumber);
void writeNumber(int number);

void setup() {
  initializePins();
}

void loop() {
  readButtons();
  handleStartButton();
  handleResetButton();
  handleLapButton();
  updateDisplay();
  writeNumber(number);
}

void initializePins() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
}

void readButtons() {
  // Read the current state for each button
  readingStartButton = digitalRead(startStopButtonPin);
  readingResetButton = digitalRead(resetButtonPin);
  readingLapButton = digitalRead(lapButtonPin);
}

void handleStartButton() {
  if (readingStartButton != lastReadingStartButton) {
    lastDebounceStartButton = millis();
  }
  if ((millis() - lastDebounceStartButton) > debounceDelayButton) {
    if (readingStartButton != startButtonState) {
      startButtonState = readingStartButton;
      if (readingStartButton != LOW) {  // the startState is true when the counter is on and
        if (!startState && lapState) {  // 0 when the counter is paused. In case we are currenly
          lapState = false;             // in the lap mode and press the start button, the lap
          number = 0;                   // state becomes false, the timer goes back to 0
          resetState = false;           // and the reset state becomes false
        }
        startState = !startState;
      }
    }
  }
  lastReadingStartButton = readingStartButton;
}

void handleResetButton() {
  if (!startState) {                    // we can use the reset button only when the timer is off
    if (readingResetButton != lastReadingResetButton) {
      lastDebounceResetButton = millis();
    }
    if ((millis() - lastDebounceResetButton) > debounceDelayButton) {
      if (readingResetButton != resetButtonState) {
        resetButtonState = readingResetButton;
        if (readingResetButton != LOW) {
          resetState = true;                // resetState becomes true only when the timer is off
          number = 0;                       // the number shown on the display will be 0
          if (lapState) {                   // if we press the reset buton when the lapState is on
            for (int i = lapsMax - 1; i >= 0; i--) {  // we erase every lap saved
              lapsSave[i] = 0;
            }
          }
        }
      }
    }
    lastReadingResetButton = readingResetButton;
  }
}

void handleLapButton() {
  if (readingLapButton != lastReadingLapButton) {
    lastDebounceLapButton = millis();
  }
  if ((millis() - lastDebounceLapButton) > debounceDelayButton) {
    if (readingLapButton != lapButtonState) {
      lapButtonState = readingLapButton;
      if (readingLapButton != LOW) {
        if (startState) {                           // if we press the lap button when the 
          for (int i = lapsMax - 1; i > 0; i--) {             // timer is on, we just save the laps 
            lapsSave[i] = lapsSave[i - 1];
          }
          lapsSave[0] = number;
          count = 0;
        }
        else if (!startState && resetState) {       // if we press the lap button after
          for (int i = 0; i < displayCount; i++) {  // we press the reset button we can navigate
            digitalWrite(displayDigits[i], LOW);    // through the saved laps, but first we clean the display 
          }
          number = lapsSave[count];
          count++;
          count = count % lapsMax;
          lapState = true;
        }

      }
    }
  }
  lastReadingLapButton = readingLapButton;
}

void updateDisplay() {
  if (startState && (millis() - lastIncrement > delayCount)) {  //if the timer is on,
    number++;                                                   // just display the timer
    number %= 10000; // Wrap around after 9999                  // and increment the number every 10 milliseconds
    lastIncrement = millis();
  }
  if (resetState) {                                             // if we press reset,
    for (int i = 0; i < displayCount; i++) {                    // clear the display
      digitalWrite(displayDigits[i], LOW);
    }
  }
}

void writeReg(int digit) {
  // Prepare to shift data by setting the latch pin low
  digitalWrite(latchPin, LOW);
  // Shift out the byte representing the current digit to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  // Latch the data onto the output pins by setting the latch pin high
  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber) {
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeNumber(int number) {
  // TODO: Initialize necessary variables for tracking the current number and digit position
  // TODO: Loop through each digit of the current number
  // TODO: Extract the last digit of the current number
  // TODO: Activate the current digit on the display
  // TODO: Output the byte encoding for the last digit to the display
  // TODO: Implement a delay for multiplexing visibility
  // TODO: Move to the next digit
  // TODO: Update 'currentNumber' by removing the last digit
  // TODO: Clear the display to prevent ghosting between digit activations

  int currentNumber = number;
  int displayDigit = displayCount - 1;

  while (displayDigit >= 0) {
    activateDisplay(displayDigit);
    // In order to be able to show the decimal point for the second digit we will change
    // the value from the byteEncodings every time we show something on the display
    // We will apply the OR opperation between the number that should be displayed on
    // second digit and B00000001 because the first 7 segments will stay the same and we will
    // only change the last one (cahnge it to 1)
    // after that we apply the AND opperation between the same number and B11111110 because
    // we want to make the last byte 0 and the first ones stay the same
    if (displayDigit == 2) {      
      byteEncodings[currentNumber % 10] = byteEncodings[currentNumber % 10] | B00000001;
    }
    writeReg(byteEncodings[currentNumber % 10]);
    byteEncodings[currentNumber % 10] = byteEncodings[currentNumber % 10] & B11111110;
    displayDigit--;
    currentNumber = currentNumber / 10;
    writeReg(B00000000);
  }
}
