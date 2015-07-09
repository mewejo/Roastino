#include <PID_v1.h>
#include <max6675.h>

const int analogInPin = A0;

// Pins
const int PIN_Element = 0;
const int PIN_Switch_1 = 1;
const int PIN_Switch_2 = 2;
const int PIN_Switch_3 = 3;
const int PIN_Switch_4 = 4;
const int PIN_LED_Status_Blue = 5;
const int PIN_LED_Status_Red = 6;

const int PIN_Thermocouple_Chamber_DO = 9;
const int PIN_Thermocouple_Chamber_CS = 10;
const int PIN_Thermocouple_Chamber_CLK = 11;
const int PIN_Thermocouple_Chamber_VCC = 12;
const int PIN_Thermocouple_Chamber_GND = 13;

// Define the thermocouple
MAX6675 Thermocouple_Chamber(PIN_Thermocouple_Chamber_CLK, PIN_Thermocouple_Chamber_CS, PIN_Thermocouple_Chamber_DO);

// Define the PID variables
double ElementPID_Setpoint, Thermocouple_Chamber_Value, ElementPID_Output;

// Specify the links and initial tuning parameters
PID ElementPID(&Thermocouple_Chamber_Value, &ElementPID_Output, &ElementPID_Setpoint,2,5,1, DIRECT);
int ElementPID_WindowSize = 2000;
unsigned long ElementPID_WindowStartTime;

void setup()
{  
  // Setup the pins
  pinMode(PIN_Element, OUTPUT);
  pinMode(PIN_Switch_1, INPUT);
  pinMode(PIN_Switch_2, INPUT);
  pinMode(PIN_Switch_3, INPUT);
  pinMode(PIN_Switch_4, INPUT);
  pinMode(PIN_LED_Status_Blue, OUTPUT);
  pinMode(PIN_LED_Status_Red, OUTPUT);
  
  // Turn on internal resistors
  digitalWrite(PIN_Switch_1, HIGH);
  digitalWrite(PIN_Switch_2, HIGH);
  digitalWrite(PIN_Switch_3, HIGH);
  digitalWrite(PIN_Switch_4, HIGH);
  
  // Pins for Thermocouple
  pinMode(PIN_Thermocouple_Chamber_VCC, OUTPUT);
  digitalWrite(PIN_Thermocouple_Chamber_VCC, HIGH);
  pinMode(PIN_Thermocouple_Chamber_GND, OUTPUT);
  digitalWrite(PIN_Thermocouple_Chamber_GND, LOW);
  
  // Wait for MAX chip to stabalise
  delay(500); 
  
  ElementPID_WindowStartTime = millis();

  // Initialize the variables we're linked to
  CalculateDesiredTemperature();

  // Tell the PID to range between 0 and the full window size
  ElementPID.SetOutputLimits(0, ElementPID_WindowSize);

  // Turn the PID on
  ElementPID.SetMode(AUTOMATIC);
}

void loop()
{
  ControlTemperature();
}

boolean CalculateDesiredTemperature()
{  
  if(!digitalRead(PIN_Switch_4))
  {
    ElementPID_Setpoint = 200;
    return true;
  }
  
  if(!digitalRead(PIN_Switch_3))
  {
    ElementPID_Setpoint = 150;
    return true;
  }
  
  if(!digitalRead(PIN_Switch_2))
  {
    ElementPID_Setpoint = 100;
    return true;
  }
  
  if(!digitalRead(PIN_Switch_1))
  {
    ElementPID_Setpoint = 50;
    return true;
  }
  
  ElementPID_Setpoint = Thermocouple_Chamber.readCelsius();
  return false;
}

void Element(boolean State)
{
  if(State)
  {
    digitalWrite(PIN_Element, HIGH);
    digitalWrite(PIN_LED_Status_Red, HIGH);
  }
  else
  {
    digitalWrite(PIN_Element, LOW);
    digitalWrite(PIN_LED_Status_Red, LOW);
  }
}

void ControlTemperature()
{
  boolean TempResult = CalculateDesiredTemperature();
  Thermocouple_Chamber_Value = Thermocouple_Chamber.readCelsius();
  ElementPID.Compute();
  
  if(!TempResult);
  {
    Element(false);
    return;
  }
  
  /************************************************
   * Turn the output pin on/off based on PID output
   ************************************************/
  unsigned long Now = millis();
  if((Now - ElementPID_WindowStartTime) > ElementPID_WindowSize)
  {
    // Time to shift the Relay Window
    ElementPID_WindowStartTime += ElementPID_WindowSize;
  }
  
  if(ElementPID_Output > (Now - ElementPID_WindowStartTime))
  {
    Element(true);
  }
  else
  {
    Element(false);
  }
  
  delay(200);
}
