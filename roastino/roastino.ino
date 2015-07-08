const int analogInPin = A0;

void setup()
{
  pinMode(10, OUTPUT);
  //Serial.begin(9600); 
}

void loop()
{
  int baseSpeed = 300;
  //int heatPercent = 3;
  
  float power;
  float sensorValue;
  float heatPercent;
  int OnTime;
  int OffTime;
  
  sensorValue = analogRead(analogInPin);            
  heatPercent = map(sensorValue, 0, 1017, 0, 100);  
  power = (heatPercent / 100);

  //Serial.println(sensorValue);
  //Serial.println(heatPercent);
  //Serial.println(power);
  
  OnTime = (baseSpeed * (power * -1)) * -1;
  OffTime = (baseSpeed - (baseSpeed * power));
  
  digitalWrite(10, LOW);
  if(power < 1)
  {
    delay(OnTime);
    digitalWrite(10, HIGH);
    delay(OffTime);
  }
}



// 100%
// On time 100
// Off time 0

// 50%
// On time 50
// Off time 50

// 25%
// On time 25
// Off time 75


