int introducedValue;
int minSamplingInterval = 1;
int maxSamplingInterval = 10;
int shouldStop = false;
int samplingInterval = 10;
int ultrasonicThreshold;
int ldrThreshold;
enum MenuState {
  MAIN_MENU,
  SENSOR_SETTINGS,
  RESET_LOGGER,
  SYSTEM_STATUS,
  LED_CONTROL,
};

MenuState currentMenu = MAIN_MENU;
bool shouldPrintMenu = true;
bool waitingUltrasonicThreshold = false;
bool waitingLdrThreshold = false;
bool waitingSamplingInterval = false;
bool waitingResetData = false;
char choice;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Arduino Menu System");
}

void loop() {
  if (shouldPrintMenu) {
    switch (currentMenu) {
      case MAIN_MENU:
        displayMainMenu();
        break;
      case SENSOR_SETTINGS:
        displaySensorSettingsMenu();
        break;
      case RESET_LOGGER:
        displayResetLoggerMenu();
        break;
      case SYSTEM_STATUS:
        displaySystemStatusMenu();
        break;
      case LED_CONTROL:
        displayLEDControlMenu();
        break;
    }
  }

  if (Serial.available()) {
    if (waitingSamplingInterval) {
      changeSamplingInterval();
    }
    else if (waitingUltrasonicThreshold) {
      changeUltrasonicThreshold();
    }
    else if (waitingLdrThreshold) {
      changeLDRThreshold();
    }
    else if (waitingResetData) {
      resetLoggerData(choice);
    }
    else {
      choice = Serial.read();
      processInput(choice);
    }
  }
}

void displayMainMenu() {
  Serial.println("\nMain Menu");
  Serial.println("1. Sensor Settings");
  Serial.println("2. Reset Logger Data");
  Serial.println("3. System Status");
  Serial.println("4. RGB LED Control");
  Serial.println("5. Exit");
  Serial.println("Enter choice: ");
  shouldPrintMenu = false;
}

void displaySensorSettingsMenu() {
  Serial.println("\nSensor Settings");
  Serial.println("1.1. Sampling Interval");
  Serial.println("1.2. Ultrasonic Alert Threshold");
  Serial.println("1.3. LDR Alert Threshold");
  Serial.println("1.4. Back");
  Serial.println("Enter choice: ");
  shouldPrintMenu = false;
}

void displayResetLoggerMenu() {
  Serial.println("\nReset Logger Data");
  Serial.println("Are you sure?");
  Serial.println("2.1. Yes");
  Serial.println("2.2. No");
  Serial.println("Enter choice: ");
  shouldPrintMenu = false;
}

void displaySystemStatusMenu() {
  Serial.println("\nSystem Status");
  Serial.println("3.1. Current Sensor Readings");
  Serial.println("3.2. Current Sensor Settings");
  Serial.println("3.3. Display Logged Data");
  Serial.println("3.4. Back");
  Serial.println("Enter choice: ");
  shouldPrintMenu = false;
}

void displayLEDControlMenu() {
  Serial.println("\nRGB LED Control");
  Serial.println("4.1. Manual Color Control");
  Serial.println("4.2. Toggle Automatic ON/OFF");
  Serial.println("4.3. Back");
  Serial.println("Enter choice: ");
  shouldPrintMenu = false;
}

void processInput(char choice) {
  switch (currentMenu) {
    case MAIN_MENU:
      handleMainMenuInput(choice);
      break;
    case SENSOR_SETTINGS:
      handleSensorSettingsInput(choice);
      break;
    case RESET_LOGGER:
      handleResetLoggerInput(choice);
      break;
    case SYSTEM_STATUS:
      handleSystemStatusInput(choice);
      break;
    case LED_CONTROL:
      handleLEDControlInput(choice);
      break;
  }
  if (waitingSamplingInterval) {
    shouldPrintMenu = false;
  }
  else if (waitingUltrasonicThreshold) {
    shouldPrintMenu = false;
  }
  else if (waitingLdrThreshold) {
    shouldPrintMenu = false;
  }
  else {
    shouldPrintMenu = true;
  }
}

void handleMainMenuInput(char choice) {
  switch (choice) {
    case '1':
      currentMenu = SENSOR_SETTINGS;
      break;
    case '2':
      currentMenu = RESET_LOGGER;
      break;
    case '3':
      currentMenu = SYSTEM_STATUS;
      break;
    case '4':
      currentMenu = LED_CONTROL;
      break;
    default:
      Serial.println("Invalid choice");
      break;
  }
}

void handleSensorSettingsInput(char choice) {
  switch (choice) {
    case '1':
      Serial.println("Enter new sampling interval (1-10 seconds): ");
      waitingSamplingInterval = true;
      break;
    case '2':
      Serial.println("Enter new minum value for Ultrasonic sensor: ");
      waitingUltrasonicThreshold = true;
      break;
    case '3':
      Serial.println("Enter new minum value for LDR sensor: ");
      waitingLdrThreshold = true;
      break;
    case '4':
      currentMenu = MAIN_MENU;
      break;
    default:
      Serial.println("Invalid choice");
      break;
  }
}

void handleResetLoggerInput(char choice) {
  //resetLoggerData();
  waitingResetData = true;
}

void handleSystemStatusInput(char choice) {
  switch (choice) {
    case '1':
      displaySystemStatus();
      break;
    case '2':
      // Code to display current sensor settings
      break;
    case '3':
      displayLoggedData();
      break;
    case '4':
      currentMenu = MAIN_MENU;
      break;
    default:
      Serial.println("Invalid choice");
      break;
  }
}

void handleLEDControlInput(char choice) {
  switch (choice) {
    case '1':
      // Code for manual color control
      break;
    case '2':
      toggleLEDAutoMode();
      break;
    case '3':
      currentMenu = MAIN_MENU;
      break;
    default:
      Serial.println("Invalid choice");
      break;
  }
}


// Placeholder functions for actions
void changeSamplingInterval() {
  if (Serial.available()) {
    introducedValue = Serial.parseInt();
    Serial.println(introducedValue);
    if (introducedValue >= minSamplingInterval && introducedValue <= maxSamplingInterval) {
      //samplingInterval = Serial.parseInt();
      currentMenu = MAIN_MENU;
      Serial.println("Sampling interval updated.");
      samplingInterval = introducedValue;
    }
    else {
      Serial.println("Invalid choice");
    }
    waitingSamplingInterval = false;
    currentMenu = SENSOR_SETTINGS;
    shouldPrintMenu = true;
  }
}
void changeUltrasonicThreshold() {
  if (Serial.available()) {
    introducedValue = Serial.parseInt();
    ultrasonicThreshold = introducedValue;
    Serial.println(introducedValue);
    currentMenu = MAIN_MENU;
    Serial.println("Minimum threshold has been set.");
    waitingUltrasonicThreshold = false;
    currentMenu = SENSOR_SETTINGS;
    shouldPrintMenu = true;
  }
}
void changeLDRThreshold() {
  if (Serial.available()) {
    introducedValue = Serial.parseInt();
    ldrThreshold = introducedValue;
    Serial.println(introducedValue);
    currentMenu = MAIN_MENU;
    Serial.println("Minimum threshold has been set.");
    waitingLdrThreshold = false;
    currentMenu = SENSOR_SETTINGS;
    shouldPrintMenu = true;
  }
}
void resetLoggerData(char choice) {
  if (introducedValue == '1') {
    samplingInterval = 10;
    ultrasonicThreshold = 0;  
    ldrThreshold = 0;
    Serial.println("All done!");
    currentMenu = MAIN_MENU;
  }
  if (introducedValue == '2') {
    currentMenu = MAIN_MENU;
  }
  if (introducedValue > '2') {
    Serial.println("Invalid number");
  }
  shouldPrintMenu = true;
  waitingResetData = false;

}
void displaySystemStatus() {
  /* Code to display system status */
}
void controlLED(int r, int g, int b) {
  /* Code to control LED */
}
void toggleLEDAutoMode() {
  /* Code to toggle LED auto mode */
}
void displayLoggedData() {
  /* Code to display logged data */
}
