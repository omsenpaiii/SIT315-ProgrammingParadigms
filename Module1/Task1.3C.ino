#include <LiquidCrystal.h>
#include <IRremote.h>

const int TMP36 = A0;
const int motor = 13;
const int LedRed = 12;
const int LedGreen = 11;
const int RECV_PIN = 2;

volatile bool motorEnabled = true;
int red_state = LOW;
int green_state = LOW;

LiquidCrystal lcd(8, 3, 4, 5, 6, 7);
IRrecv irrecv(RECV_PIN);
decode_results results;

void stopMotor() {
  motorEnabled = false;
  digitalWrite(motor, LOW);
  digitalWrite(LedRed, LOW);
  digitalWrite(LedGreen, HIGH);
  lcd.setCursor(11, 1);
  lcd.print("OFF");
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

  irrecv.enableIRIn();
  attachInterrupt(digitalPinToInterrupt(RECV_PIN), stopMotor, FALLING);

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

    if (Temperature > 50 || green_state == HIGH) {
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
    }
  }

  if (irrecv.decode(&results)) {
    Serial.println("Program interrupted by IR Interrupt.");
    Serial.print("IR Code: ");
    Serial.println(results.value, HEX);

    if (results.value == 0x19DA44BB) { // Replace with the correct IR code from your remote
      green_state = !green_state;
      Serial.println("IR Interrupt.");
    }
    irrecv.resume();
  }

  delay(1000);
}
