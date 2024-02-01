#include <LiquidCrystal.h>

const int TMP36 = A0;
const int motor = 13;
const int LedRed = 12;
const int LedGreen = 11;
const int buttonPin = 2;

volatile bool motorEnabled = true;

LiquidCrystal lcd(8, 3, 4, 5, 6, 7);

void stopMotor() {
  motorEnabled = false;
  digitalWrite(motor, LOW);
  digitalWrite(LedRed, LOW);
  digitalWrite(LedGreen, HIGH);
  lcd.setCursor(11, 1);
  lcd.print("OFF");
  Serial.println("OFF");
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Automated Plant");
  lcd.setCursor(0, 1);
  lcd.print("Watering System!");
  pinMode(motor, OUTPUT);
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Internal pull-up resistor

  attachInterrupt(digitalPinToInterrupt(buttonPin), stopMotor, FALLING);

  delay(2000);
  lcd.clear();
  lcd.print("Temp= ");
  lcd.setCursor(0, 1);
  lcd.print("WaterPump= ");
  Serial.println("System Initialized");
}

void loop() {
  if (motorEnabled) {
    int value = analogRead(TMP36);
    float Temperature = value * 500.0 / 1023.0;
    lcd.setCursor(6, 0);
    lcd.print(Temperature);
    lcd.setCursor(11, 1);

    Serial.print("Temperature: ");
    Serial.print(Temperature);
    Serial.print(" C | Water Pump: ");

    if (Temperature > 50) {
      digitalWrite(motor, HIGH);
      digitalWrite(LedRed, HIGH);
      digitalWrite(LedGreen, LOW);
      lcd.print("ON ");
      Serial.println("ON");
    } else {
      digitalWrite(motor, LOW);
      digitalWrite(LedRed, LOW);
      digitalWrite(LedGreen, HIGH);
      lcd.print("OFF");
      Serial.println("OFF");
    }
  }

  delay(1000);
}