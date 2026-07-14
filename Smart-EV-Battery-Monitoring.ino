#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Analog Inputs
const int voltagePin = A0;
const int currentPin = A1;
const int tempPin = A3;

// Digital Outputs
const int greenLED = 8;
const int yellowLED = 9;
const int redLED = 10;
const int buzzer = 11;
const int relay = 12;
const int button = 2;

float voltage = 0;
float current = 0;
float temperature = 0;
int soc = 0;

void setup() {

  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);

  pinMode(button, INPUT_PULLUP);

  digitalWrite(relay, HIGH);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print(" SMART EV BMS ");
  lcd.setCursor(0,1);
  lcd.print(" Initializing ");
  delay(2000);
  lcd.clear();
}

void loop() {

  int vRaw = analogRead(voltagePin);
  int cRaw = analogRead(currentPin);
  int tRaw = analogRead(tempPin);

  // Voltage 44V-54V
  voltage = map(vRaw,0,1023,440,540)/10.0;

  // Current 0-20A
  current = map(cRaw,0,1023,0,200)/10.0;

  // Temperature 20-80°C
  temperature = map(tRaw,0,1023,-20,100);

  // Battery Percentage
  soc = map(voltage*10,440,540,0,100);

  if(soc>100) soc=100;
  if(soc<0) soc=0;

  String status="NORMAL";

  // Normal Mode
  digitalWrite(greenLED,HIGH);
  digitalWrite(yellowLED,LOW);
  digitalWrite(redLED,LOW);
  digitalWrite(buzzer,LOW);
  digitalWrite(relay,HIGH);

  // Fault Detection
  if(voltage>54)
  {
    status="OVER VOLT";
    digitalWrite(redLED,HIGH);
    digitalWrite(greenLED,LOW);
    digitalWrite(buzzer,HIGH);
    digitalWrite(relay,LOW);
  }

  else if(voltage<44)
  {
    status="LOW VOLT";
    digitalWrite(redLED,HIGH);
    digitalWrite(greenLED,LOW);
    digitalWrite(buzzer,HIGH);
    digitalWrite(relay,LOW);
  }

  else if(current>18)
  {
    status="OVER CURR";
    digitalWrite(yellowLED,HIGH);
    digitalWrite(greenLED,LOW);
    digitalWrite(buzzer,HIGH);
    digitalWrite(relay,LOW);
  }

  else if(temperature>60)
  {
    status="OVER TEMP";
    digitalWrite(redLED,HIGH);
    digitalWrite(greenLED,LOW);
    digitalWrite(buzzer,HIGH);
    digitalWrite(relay,LOW);
  }

  // Reset Button
  if(digitalRead(button)==LOW)
  {
    digitalWrite(redLED,LOW);
    digitalWrite(yellowLED,LOW);
    digitalWrite(greenLED,HIGH);
    digitalWrite(buzzer,LOW);
    digitalWrite(relay,HIGH);
    status="RESET";
    delay(500);
  }

  // LCD Display
  lcd.setCursor(0,0);
  lcd.print("V:");
  lcd.print(voltage,1);
  lcd.print(" I:");
  lcd.print(current,1);
  lcd.print(" ");

  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(temperature,0);
  lcd.print((char)223);
  lcd.print(" ");
  lcd.print(soc);
  lcd.print("% ");

  // Serial Monitor
  Serial.println("----------------------------");
  Serial.print("Voltage : ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Current : ");
  Serial.print(current);
  Serial.println(" A");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("SOC : ");
  Serial.print(soc);
  Serial.println("%");

  Serial.print("Status : ");
  Serial.println(status);

  delay(1000);
}