/**
 * @file SmartPlantWateringSystem.ino
 * @brief Automated plant watering system with sensor inputs and interrupts
 * 
 * This Arduino sketch implements an automated plant watering system using various sensors
 * and interrupts. It controls a water pump based on temperature, IR remote control input,
 * and soil moisture level.
 * 
 * Pin Assignments:
 * - TMP36: Analog input pin for temperature sensor
 * - motor: Pin controlling the water pump
 * - LedRed: Pin controlling the red LED
 * - LedGreen: Pin controlling the green LED
 * - RECV_PIN: Pin for IR receiver
 * - soilMoisturePin: Analog input pin for soil moisture sensor
 * - blueLedPin: Pin for the blue LED
 * 
 * Hardware Requirements:
 * - Temperature sensor (TMP36)
 * - Soil moisture sensor
 * - IR receiver module
 * - LEDs for status indication
 * - Water pump
 * - Arduino board
 * - LiquidCrystal display for visualization
 * 
 * The system initializes by displaying a welcome message on the LCD. It continuously reads
 * temperature from the TMP36 sensor and displays it on the LCD. Depending on the temperature
 * and user input from the IR remote control, it controls the water pump and the LEDs.
 * Additionally, it monitors soil moisture level using an interrupt-based approach.
 * 
 * @author Om Tomar
 * @date 08/02/2024
 */

#include <LiquidCrystal.h>
#include <IRremote.h>

/// Pin assignments
const int TMP36 = A0;                   ///< Analog input pin for temperature sensor
const int motor = 13;                   ///< Pin controlling the water pump
const int LedRed = 12;                  ///< Pin controlling the red LED
const int LedGreen = 11;                ///< Pin controlling the green LED
const int RECV_PIN = 2;                 ///< Pin for IR receiver
const int soilMoisturePin = A1;         ///< Analog input pin for soil moisture sensor
const int blueLedPin = 9;               ///< Pin for the blue LED

/// Flag indicating whether the motor is enabled
volatile bool motorEnabled = true;

/// State variables for LEDs
int red_state = LOW;
int green_state = LOW;
int blue_state = LOW;

/// LiquidCrystal object for LCD display
LiquidCrystal lcd(8, 3, 4, 5, 6, 7);

/// IRrecv object for IR remote control
IRrecv irrecv(RECV_PIN);
decode_results results;

/// Function to stop the motor
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

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Automated Plant");
  lcd.setCursor(0, 1);
  lcd.print("Watering System!");

  // Set pin modes
  pinMode(motor, OUTPUT);
  pinMode(LedRed, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(blueLedPin, OUTPUT); // Set blue LED pin as output
  pinMode(soilMoisturePin, INPUT);

  // Enable IR receiver
  irrecv.enableIRIn();
  
  // Attach interrupt for stopping the motor
  attachInterrupt(digitalPinToInterrupt(RECV_PIN), stopMotor, FALLING);

  // Setup timer interrupt
  cli(); // Disable interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 15624; // Set compare match register for 1Hz
  TCCR1B |= (1 << WGM12); // Turn on CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set CS12 and CS10 bits for 1024 prescaler
  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt
  sei(); // Enable interrupts

  // Initialization delay
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

    if (results.value == 0x19DA44BB) { 
      green_state = !green_state;
      Serial.println("IR Interrupt.");
    }
    irrecv.resume();
  }

  delay(1000);
}

/// Timer interrupt service routine
ISR(TIMER1_COMPA_vect) {
  // Toggle blue LED state
  blue_state = !blue_state;
  digitalWrite(blueLedPin, blue_state);
}

/// Soil moisture interrupt service routine
ISR(PCINT0_vect) {
  // Read soil moisture value
  int moisture = analogRead(soilMoisturePin);
  Serial.print("Soil Moisture: ");
  Serial.println(moisture);
}
