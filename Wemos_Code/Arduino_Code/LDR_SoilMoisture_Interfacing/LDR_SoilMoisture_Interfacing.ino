const int s0 = 16;   //A
const int s1 = 14;   //B

unsigned int analogVal = 0;

int analogread(int pin)
{
  unsigned int tempVal = 0;
  
  if (pin == 0)
  {
    digitalWrite(s1,LOW);
    digitalWrite(s0,LOW);
    delay(25);
    //Reading Ay0
    tempVal = analogRead(A0);
    return tempVal;
  }
  else if (pin == 1)
  {
    digitalWrite(s1,LOW); 
    digitalWrite(s0,HIGH);
    delay(25);
    //Reading Ay1
    tempVal = analogRead(A0); 
    return tempVal;
  }
  else if (pin == 2)
  {
    digitalWrite(s1,HIGH);  
    digitalWrite(s0,LOW);
    delay(25);
    //Reading Ay2
    tempVal = analogRead(A0);
    return tempVal;  
  }
  else if (pin == 3)
  {
    digitalWrite(s1,HIGH);  
    digitalWrite(s0,HIGH);  
    delay(25);
    //Reading Ay3
    tempVal = analogRead(A0);
    return tempVal;
  }
}

void initializeCD4051(void)
{
  pinMode(s0,OUTPUT);
  pinMode(s1,OUTPUT);
}
void setup() {
  // put your setup code here, to run once:
 initializeCD4051();
 Serial.begin(9600);
 Serial.println("");
 Serial.println("");
 Serial.println("Interfacing Soil Moisture and LDR sensor using CD4051 with the Wemos D1 Mini");
}

void loop() {
  // put your main code here, to run repeatedly:
  analogVal = analogread(0);
  Serial.print("Soil Moisture Sensor:- ");
  Serial.println(analogVal);
  
  analogVal = analogread(1);
  Serial.print("LDR Light Sensor:- ");
  Serial.println(analogVal);

  delay(250);
}
