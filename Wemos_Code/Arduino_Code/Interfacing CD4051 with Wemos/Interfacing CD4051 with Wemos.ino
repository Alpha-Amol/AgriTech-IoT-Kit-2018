/*
 * Three binary signals (A, B and C) select one of the eight 
 * channels to be turned on and connect one of the eight inputs 
 * to the common pin.
 * In our case we have grounded the C binary control signal, so 
 * using A and B binary control signals
 * we can select one of the four channels to be turned on and connect
 * one of them to the common pim (A0 of Wemos D1 mini).
 *  Wiring: 
 *  Wemos   ->  4051 
 *  --------------- 
 *  D0      ->   S0 / A
 *  D5      ->   S1 / B 
 *  GND     ->   S2 / C 
 *  A0      ->   Common 
 *  3.3v    ->   VCC 
 *  GND     ->   GND 
 *  GND     ->   Inhibit 
 *  GND     ->   VEE
 *  
 *  Since 
 */
const int A = 16;  //GPIO16 or D0
const int B = 14;  //GPIO14 or D5

/*
 * Variable to store the analog sensors value
 */
unsigned int analogVal = 0;

/*
 * Function to select one of the four channels to be turned on and connect
 * one of them to the common pim (A0 of Wemos D1 mini).
 * Just pass oon the analog channel you want to read (0 to 3) and it will
 * return you the analog reading in integer format
 */
int analogread(int pin)
{
  unsigned int tempVal = 0;
  
  if (pin == 0)
  {
    digitalWrite(A,LOW);
    digitalWrite(B,LOW);
    delay(25);
    //Reading Channel 0 or Ay0
    tempVal = analogRead(A0);
    return tempVal;
  }
  else if (pin == 1)
  {
    digitalWrite(A,HIGH);
    digitalWrite(B,LOW); 
    delay(25);
    //Reading Channel 1 or Ay1
    tempVal = analogRead(A0); 
    return tempVal;
  }
  else if (pin == 2)
  {
    digitalWrite(A,LOW);
    digitalWrite(B,HIGH);  
    delay(25);
    //Reading Channel 2 or Ay2
    tempVal = analogRead(A0);
    return tempVal;  
  }
  else if (pin == 3)
  {
    digitalWrite(A,HIGH);  
    digitalWrite(B,HIGH);  
    delay(25);
    //Reading Channel 3 or Ay3
    tempVal = analogRead(A0);
    return tempVal;
  }
}

/*
 * Initialize the pins connected to the binary signals of CD4051
 * as output
 */
void initializeCD4051(void)
{
  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
}

void setup() {
  // put your setup code here, to run once:
 initializeCD4051();

 //For debugging purpose open your serial monitor and set baid rate to 9600
 Serial.begin(9600);
 Serial.println("");
 Serial.println("CD4051 Interfacing");
}

void loop() {
  // put your main code here, to run repeatedly:

  //Reading Analog sensor 0 connected to channel 0 of CD4051
  analogVal = 0;
  analogVal = analogread(0);
  Serial.print("Analog Sensor 0:- ");
  Serial.println(analogVal);

  //Reading Analog sensor 1 connected to channel 1 of CD4051
  analogVal = 0;
  analogVal = analogread(1);
  Serial.print("Analog Sensor 1:- ");
  Serial.println(analogVal);
 
  //Reading Analog sensor 2 connected to channel 2 of CD4051 
  analogVal = 0;
  analogVal = analogread(2);
  Serial.print("Analog Sensor 2:- ");
  Serial.println(analogVal);

  //Reading Analog sensor 3 connected to channel 3 of CD4051
  analogVal = 0;
  analogVal = analogread(3);
  Serial.print("Analog Sensor 3:- ");
  Serial.println(analogVal);
  
  delay(250);
}
