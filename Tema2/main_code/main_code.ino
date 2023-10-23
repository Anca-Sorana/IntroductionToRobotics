const int RED_LED_PIN = 3;
const int GREEN_LED_PIN = 5;
const int BLUE_LED_PIN = 6;

const int potPinRed = A0;
const int potPinGreen = A1;
const int potPinBlue = A2; 

const float maxValueLED = 255.;
const float maxValuePot = 1023.;

float potValueRed = 0;
float potValueGreen = 0;
float potValueBlue = 0;

float intensityRed = 0;
float intensityGreen = 0;
float intensityBlue = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  potValueRed = analogRead(potPinRed);
  potValueBlue = analogRead(potPinBlue);
  potValueGreen = analogRead(potPinGreen);

  intensityRed = map(potValueRed, 0, maxValuePot, 0, maxValueLED);
  intensityGreen = map(potValueGreen, 0, maxValuePot, 0, maxValueLED);
  intensityBlue = map(potValueBlue, 0, maxValuePot, 0, maxValueLED);

  
  //if you want to check the pots' values you can uncomment this:
   
//  Serial.print("Potentiometer Value for Red LED: ");
//  Serial.print(potValueRed);
//  Serial.print(" Potentiometer Value for Green LED: ");
//  Serial.print(potValueGreen);
//  Serial.print(" Potentiometer Value for Blue LED: ");
//  Serial.print(potValueBlue);
//  Serial.println(' ');

  analogWrite(RED_LED_PIN, intensityRed);
  analogWrite(GREEN_LED_PIN, intensityGreen);
  analogWrite(BLUE_LED_PIN, intensityBlue);
  
}
