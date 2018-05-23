const int motorPin = 13;

void motorPumpON(void)
{
  digitalWrite(motorPin, HIGH);
}

void motorPumpOff(void)
{
  digitalWrite(motorPin, LOW);
  analogWrite(motorPin, 0);
}

void motorONSpeed(int speedVar)
{
  analogWrite(motorPin, speedVar);
}

void setup() 
{
 // put your setup code here, to run once:
 pinMode(motorPin, OUTPUT); 
 motorPumpOff(); 
 Serial.begin(9600);
 Serial.println("");
 Serial.println("Water Pump Speed Control");
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available()==0) {             //Wait for user input
  
  }
  String data = Serial.readStringUntil('\n');

  Serial.println("");
  Serial.print("Serial Data Received: ");
  Serial.println(data);
  
  int dataVar = data.toInt();

  Serial.print("Water Pump Speed:- ");
  Serial.println(dataVar);
  
  motorONSpeed(dataVar);
  //motorPumpON();
  delay(5000);
  
  Serial.println("Water Pump OFF");
  motorPumpOff(); 
  delay(1000);
}
