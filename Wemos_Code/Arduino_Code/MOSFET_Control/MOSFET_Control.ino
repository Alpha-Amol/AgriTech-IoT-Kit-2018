/*
 * LED Light is connected to GPIO 15 or D8 pin
 * of the Wemos D1 Mini
 */
const int lightPin = 15;

/*
 * LED Light is connected to GPIO 13 or D7 pin
 * of the Wemos D1 Mini
 */
const int motorPin = 13;


/*
 * Function to switch ON the LED Light using MOSFET
 */
void lightON(void)
{
  digitalWrite(lightPin, HIGH);
}

/*
 * Function to switch OFF the LED Light using MOSFET
 */
void lightOff(void)
{
  digitalWrite(lightPin, LOW);
}

/*
 * Function to switch ON the water pump using MOSFET
 */
void motorPumpON(void)
{
  digitalWrite(motorPin, HIGH);
}

/*
 * Function to switch OFF the water pump using MOSFET
 */
void motorPumpOff(void)
{
  digitalWrite(motorPin, LOW);
}

void setup() {
  // put your setup code here, to run once:
 pinMode(motorPin, OUTPUT); 
 pinMode(lightPin, OUTPUT);
 motorPumpOff(); 
 lightOff(); 
 
 Serial.begin(9600);
 Serial.println("");
 Serial.println("");
 
 Serial.println("Water Pump and LED Light Control!");
}

void loop() {
  // put your main code here, to run repeatedly:
 Serial.println("Switching ON the Water Pump");
 motorPumpON();
 delay(2500*2);
 Serial.println("Switching OFF the Water Pump");
 motorPumpOff(); 
 delay(2500);
 
 Serial.println("Switching ON the LED Light");
 lightON();
 delay(2500*2);
 Serial.println("Switching OFF the LED Light");
 lightOff(); 
 delay(2500);
}
