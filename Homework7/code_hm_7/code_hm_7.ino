#include "LedControl.h" // Library for controlling the LED matrix

// Pin configuration
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int xPin = A0; // Joystick X-axis
const int yPin = A1; // Joystick Y-axis
const int buttonPin = 2; // Button pin

// LED matrix configuration
const byte matrixSize = 8; // Size of the LED matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); // Initialize LedControl object
byte matrixBrightness = 2; // Brightness level of the matrix

// Player variables
byte xPosPlayer = 0;
byte yPosPlayer = 0;
byte xLastPosPlayer = 0;
byte yLastPosPlayer = 0;
int delayLedPlayer = 300; // Delay for LED player update
byte xPositionRandom;
byte yPositionRandom;
int lastDebounceDelayPlayer;

// Joystick thresholds
const int minThreshold = 200;
const int maxThreshold = 600;
const byte moveInterval = 200; // Interval for LED movement
unsigned long long lastMoved = 0;

// Button variables
const byte pressInterval = 50;
int lastPressed = 0;
byte buttonState = LOW;
int lastReading;
int currentMillis;
int reading;

// Game status variables
bool matrixChanged = true; // Flag for matrix update
bool gameStatus = true; // Game running status
int nrWallsMax; // Maximum number of walls
int nrWalls = 0; // Current number of walls

// Bomb variables
byte bombActive = false;
int startBomb;
int bombExplodingTime = 2000;
int lastChangedBomb;
int bombLightInterval = 200;
int xPosBomb;
int yPosBomb;

// Animation variables
int lastAnimation = 0;
int delayAnimation = 100;
int currentAnimation = 0;

// Matrix state array
byte matrix[matrixSize][matrixSize] = {0};

// Predefined images array
const uint8_t images[][8] = {
  {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }, {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000
  }, {
    0b00000000,
    0b00000000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00000000,
    0b00000000
  }, {
    0b00000000,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00000000
  }, {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111
  }, {
    0b00000000,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00000000
  }, {
    0b00000000,
    0b00000000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00000000,
    0b00000000
  }, {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000
  }, {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }
};
const int imagesLen = sizeof(images) / 8;

void clearMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      matrix[row][col] = 0;
    }
  }
  updateMatrix();
}

void handleGameOver(int currentMillis) { //display a certain animantion when the player loses teh game and restart the game
  if (currentMillis - lastAnimation > delayAnimation) {
    lastAnimation = currentMillis;
    displayImage(images[currentAnimation]);
    if (++currentAnimation >= imagesLen) {
      gameStatus = !gameStatus;
      currentAnimation = 0;
      clearMatrix();
      nrWalls = 0;
      xPosPlayer = 0;
      yPosPlayer = 0;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  lc.shutdown(0, false); // Enable display
  lc.setIntensity(0, matrixBrightness); // Set brightness
  lc.clearDisplay(0); // Clear display
  nrWallsMax = random(32, 48); // Initialize maximum walls
}

void loop() {
  currentMillis = millis();
  reading = digitalRead(buttonPin);

  if (!gameStatus) {              //if the player lost
    handleGameOver(currentMillis);
  } else {                        //the game can begin again
    handleGameRunning(currentMillis, reading);
  }

  lastPressed = reading;
}

void displayImage(const byte * image) {       //display the animation when the player loses
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, bitRead(image[i], matrixSize - 1 - j));
    }
  }
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void generateMap() {                        // generate random position for the walls on the matrix but avoid the leds near the starting point
  // Map generation logic
  xPositionRandom = random(matrixSize + 1);
  yPositionRandom = random(matrixSize + 1);
  if (xPositionRandom > xPosPlayer + 1 || yPositionRandom > yPosPlayer + 1) {
    if (!matrix[xPositionRandom][yPositionRandom]) { //in case this position was already set as a wall or is the same position as the player
      nrWalls++;
      matrix[xPositionRandom][yPositionRandom] = !matrix[xPositionRandom][yPositionRandom];
    }
  }
}

void updatePositions() {                // when we use the joystick
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  // Store the last positions of the LED
  xLastPosPlayer = xPosPlayer;
  yLastPosPlayer = yPosPlayer;
  // Update xPos based on joystick movement (X-axis)
  if (xValue < minThreshold) {
    if (xPosPlayer < matrixSize - 1) {
      xPosPlayer++;
      if (matrix[xPosPlayer][yPosPlayer]) {
        xPosPlayer--;
      }
    }
  }
  if (xValue > maxThreshold) {
    if (xPosPlayer > 0) {
      xPosPlayer--;
      if (matrix[xPosPlayer][yPosPlayer]) {
        xPosPlayer++;
      }
    }
  }
  if (yValue > maxThreshold) {
    if (yPosPlayer < matrixSize - 1) {
      yPosPlayer++;
      if (matrix[xPosPlayer][yPosPlayer]) {
        yPosPlayer--;
      }
    }
  }
  // Update xPos based on joystick movement (Y-axis)
  if (yValue < minThreshold) {
    if (yPosPlayer > 0) {
      yPosPlayer--;
      if (matrix[xPosPlayer][yPosPlayer]) {
        yPosPlayer++;
      }
    }
  }
  // Check if the position has changed and update the matrix if necessary
  if (xPosPlayer != xLastPosPlayer || yPosPlayer != yLastPosPlayer) {
    matrixChanged = true;
    matrix[xLastPosPlayer][yLastPosPlayer] = 0;
    //    matrix[xPos][yPos] = 1;
  }
}

void placeBomb() {        //bomb placed and we save the coordinates for that bomb
  // Place bomb logic
  xPosBomb = xPosPlayer;
  yPosBomb = yPosPlayer;

  bombActive = true;
  startBomb = currentMillis;
}


void explode() {          //when the time goes off for the bomb we need to verifi which walls we have to distroy
  // Directions in which the explosion will affect: [x, y]
  int directions[][2] = {{0, 0}, { -1, 0}, {1, 0}, {0, -1}, {0, 1}};

  for (int i = 0; i < 5; i++) {
    int x = xPosBomb + directions[i][0];
    int y = yPosBomb + directions[i][1];

    // Check bounds and update matrix
    if (x >= 0 && x < matrixSize && y >= 0 && y < matrixSize) {  //check if the walls we want to take down are not outside the matrix
      matrix[x][y] = 0;

      // Check for player's position
      if (x == xPosPlayer && y == yPosPlayer) {
        gameStatus = false;
      }
    }
  }
}


void handleGameRunning(int currentMillis, int reading) {
  // Game running logic here
  if (nrWalls <= nrWallsMax) { //generate the walls of the game
    generateMap();
    matrixChanged = true;
    xPosPlayer = 0;         //keep the players position at 0
    yPosPlayer = 0;
  }
  else {
    if (currentMillis - lastDebounceDelayPlayer > delayLedPlayer) {    //Hilight the current position of the player by turning the led on and off
      lastDebounceDelayPlayer = currentMillis;
      matrix[xPosPlayer][yPosPlayer] = !matrix[xPosPlayer][yPosPlayer];
      matrixChanged = true;
    }
    if (currentMillis - lastMoved > moveInterval ) { // Check if it's time to move the LED
      // game logic
      updatePositions(); // Update the position of the LED based on joystick input
      lastMoved = currentMillis; // Update the time of the last move
    }
    if (reading != lastReading) {  //check if the reading from the button is different than the last time
      lastPressed = currentMillis;
    }

    if ((currentMillis - lastPressed) > pressInterval) {  // when the button in pressed then we place a bomb at that location
      if (reading != buttonState) {
        buttonState = reading;
        if (buttonState == LOW) {
          placeBomb();
        }
      }
    }
    if (bombActive && (currentMillis - startBomb) < bombExplodingTime) { //if the a bomb was placed at a certain location, in order to see it, we will make the led at that position blick faster than the player
      if (currentMillis - lastChangedBomb > bombLightInterval) {
        matrix[xPosBomb][yPosBomb] = !matrix[xPosBomb][yPosBomb];
        lastChangedBomb = currentMillis;
        matrixChanged = true;
      }
    }
    else if (bombActive) { //when the bomb is ready to explode
      bombActive = false;
      explode();
      matrixChanged = true;
    }
    if (matrixChanged == true) { // Check if the matrix display needs updating
      updateMatrix(); // Update the LED matrix display
      matrixChanged = false; // Reset the update flag
    }
  }
  lastReading = reading;
}
