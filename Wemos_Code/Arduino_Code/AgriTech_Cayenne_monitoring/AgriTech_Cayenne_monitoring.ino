/*
 * This code automatically provides watering and light to
 * your plant and you can also monitor the sensor parameters 
 * like soil temperature, soil moisture, air temperautre and humidity,
 * and light intensity via the Cayenne application from anywhere in the
 * world.
 * But you can just monitor and would not be able to control the water
 * pump or led light via the Cayenne application
 */

/*
 * Libraries required for sending data to Cayenne Cloud server
 */
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>
#include <ESP8266WiFi.h>

/*
 * Libraries required for interfacing the DS18B20 digital 
 * temperature sensor
 * DS18B20 sensor uses 1-wire protocol for sending the
 * temperature readings
 */
#include <OneWire.h>
#include <DallasTemperature.h>

/*
 * EEPROM is used to store the the light intensity threshold and
 * soil moisture threshold.
 * Based on the light intensity threshold the led light can be switched 
 * ON/OFF automatically
 * Based on the soil moisture threshold the water pump can be switched 
 * ON/OFF automatically
 * You can change these threshold values via the cayenne app
 */
#include <EEPROM.h>

/*
 * Libraries required for interfacing the DHT11 digital 
 * temperature and humidity sensor
 */
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN  12         // GPIO Pin which is connected to the DHT sensor.
#define DHTTYPE DHT11     // DHT11 or DHT22 (AM2302) or DHT21 (AM2301)
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// Data wire is plugged into GPIO 0 on the wemos d1 mini
#define ONE_WIRE_BUS 0
#define TEMPERATURE_PRECISION 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold DS18B20 device addresses
DeviceAddress insideThermometer = { 0x28, 0x7C, 0xC7, 0x97, 0x09, 0x00, 0x00, 0x61 };

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
 */
const int s0 = 16;   //A
const int s1 = 14;   //B

/*
 * Variable to store the analog sensors value
 */
unsigned int analogVal = 0;

/*
 * Built in LED pin (GPIO 2 or D4)of the Wemos D1 Mini
 */
const int ledPin = 2;

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

//WiFi signal strength Variable
long wifiRSSI = 0;

// WiFi network info i.e. SSID and Password
char ssid[] = "LitlHeat";            //Add your home wifi network ssid 
char wifiPassword[] = "Auoate@11";   //Add wifi password

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "9ccd05e0-658-11e7-8378-e1a615c9f526";       //Add your username
char password[] = "98b38f9c8518d6c03cc53fd9819be5ddc03c8e";   //Add your password
char clientID[] = "1ccd5020-ae9f-11e-b0e9-e9adcff3788e";       //Add your clientID

 //Publish data every 15 seconds (15000 milliseconds). Change this value to publish at a different interval.
int uploadInterval = 15000; //in milliseconds
unsigned long lastMillis = 0;

/*
 * Function to read an integer data from a particular address in EEPROM
 */
int eepromReadInt(int address){
   int value = 0x0000;
   value = value | (EEPROM.read(address) << 8);
   value = value | EEPROM.read(address+1);
   return value;
}

/*
 * Function to write an integer data to a particular address in EEPROM
 */ 
void eepromWriteInt(int address, int value){
   EEPROM.write(address, (value >> 8) & 0xFF );
   EEPROM.write(address+1, value & 0xFF);
   EEPROM.commit();
}

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

/*
 * Function to Initialize the pins connected to the binary signals of CD4051
 * as output
 */
void initializeCD4051(void)
{
  pinMode(s0,OUTPUT);
  pinMode(s1,OUTPUT);

  //Default Ay0 connected to A0
  digitalWrite(s1,LOW);
  digitalWrite(s0,LOW);
}

/*
 * Function for blinking an led
 */
void ledBlink(int ledpin, int times)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(ledpin, HIGH);
    delay(250);
    digitalWrite(ledpin, LOW);   
    delay(250); 
  }
}

/*
 * Function for reading the DS18B20 temperature sensor using its unique address
 */
float readSoilTemperature(DeviceAddress deviceAddress)
{
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors.requestTemperatures();
  
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temperature C: ");
  Serial.print(tempC);
  Serial.print(" Temperature F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC));

  return tempC;
}

/*
 * Function for reading the air temperature using DHT11 digital temperature and humidity sensor
 */
float readAirTemperature(void)
{
  float tempC = 0;
  sensors_event_t event; 
  dht.temperature().getEvent(&event);
  tempC = event.temperature;
  if (isnan(tempC)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    
    Serial.print(tempC);
    Serial.print(" *C");
  }
  return tempC;
}

/*
 * Function for reading the air humidity using DHT11 digital temperature and humidity sensor
 */
float readAirHumidity(void)
{
  float hum = 0;
  sensors_event_t event; 
  dht.humidity().getEvent(&event);
  hum = event.relative_humidity;
  if (isnan(hum)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    
    Serial.print(hum);
    Serial.print(" %");
  }
  return hum;
}

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
 initializeCD4051();
 pinMode(motorPin, OUTPUT); 
 pinMode(lightPin, OUTPUT);
 motorPumpOff(); 
 lightOff(); 
 
 pinMode(ledPin, OUTPUT);
 digitalWrite(ledPin, LOW); 
 
 Serial.begin(9600);
 Serial.println("");
 Serial.println("");
 Serial.println("Smart Mali Project!");

 EEPROM.begin(512); // Initlize EEPROM with 512 Bytes

 Serial.println("DHT Sensor Initializing");
 //Initialize DHT sensor device.
 dht.begin();
 // Print temperature sensor details.
 sensor_t sensor;
 dht.temperature().getSensor(&sensor);
 // Print humidity sensor details.
 dht.humidity().getSensor(&sensor);
 // Set delay between sensor readings based on sensor details.
 delayMS = sensor.min_delay / 1000;
 Serial.print("Delay between DHT Sensor reading: ");
 Serial.println(delayMS);
 delayMS = delayMS * 2;
 
 Serial.println("DS18B20 Sensor Initializing");   
 // Start up the DS18B20 library
 sensors.begin();
 // set the resolution
 sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);

 delay(500);                      // Wait for half a second
 
 Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  
 ledBlink(ledPin, 5);
 
 Serial.println("CD4051 Interfacing with the Wemos D1 Mini");
}

void loop() {
  Cayenne.loop(); 
  int lightLevel, soilLevel;

  if (millis() - lastMillis > uploadInterval ) 
  {
    // put your main code here, to run repeatedly:
    Serial.println("");
    
    digitalWrite(ledPin, HIGH); 
  
    analogVal = 0;
    analogVal = analogread(0);
    Serial.print("Soil Moisture Sensor Raw:- ");
    Serial.print(analogVal);
    Serial.print("  ");
    soilLevel = map(analogVal, 0, 1000, 0, 100);
    Serial.print("Soil Moisture Sensor (%):- ");
    Serial.println(soilLevel);
    if (soilLevel < 25)
    {
      Serial.println("Motor ON for 2.5 seconds and then OFF");
      motorPumpON();
      delay(2500);
      motorPumpOff();
    }
    else{
      Serial.println("Motor OFF");
      motorPumpOff();    
    }
    
    analogVal = 0;
    analogVal = analogread(1);
    Serial.print("LDR Light Sensor Raw:- ");
    Serial.print(analogVal);
    Serial.print("  ");
    lightLevel = map(analogVal, 0, 1000, 0, 100);
    Serial.print("LDR Light Sensor (%):- ");
    Serial.println(lightLevel);
    if (lightLevel < 25)
    {
      Serial.println("Light ON ");
      lightON();
    }
    else{
      Serial.println("Light OFF");
      lightOff();  
    }
    
    float soilTemperature = readSoilTemperature(insideThermometer);
  
    float airTemperature = readAirTemperature();
    Serial.print("  ");
    float airHumidity = readAirHumidity();
    Serial.println("");

    //the received wifi signal strength:
    wifiRSSI = WiFi.RSSI(); 
    Serial.print("RSSI:");
    Serial.println(wifiRSSI);
    
    //Write data to Cayenne here. The following code lines just sends all the important data to Cayenne
    Cayenne.virtualWrite(1, lastMillis);
    Cayenne.luxWrite(2, lightLevel);
    Cayenne.virtualWrite(3, soilLevel);
    Cayenne.celsiusWrite(5, airTemperature);
    Cayenne.virtualWrite(6, airHumidity);
    Cayenne.virtualWrite(7, wifiRSSI);
    Cayenne.celsiusWrite(12, soilTemperature);
    
    digitalWrite(ledPin, LOW); 
    lastMillis = millis();
  }
  
  delay(delayMS*2);  
}
