/*******************************************************************
This code implement the Amazon Amazon Dash Replenishment API to 
automatically buy something from amazon when the sensor reading is 
below the threshold value. This code utilize the WiFi101 library,
AmazonDRS library, Ultrasonic Library and ArduinoJson library. It is the modified version of the amazonDashButton found in the AmazonDRS library 
example.

Credit to Brian Carbonette, J.Rodrigo and Benoit Blanchon for writing the 
AmazonDRS library, Ultrasonic Library and Arduino json library respectively.

AmazonDRS library
https://github.com/andium/AmazonDRS

Arduino json library
https://github.com/bblanchon/ArduinoJson

Arduino ultrasonic library
https://github.com/JRodrigoTech/Ultrasonic-HC-SR04

WiFi101 library
https://github.com/arduino-libraries/WiFi101

 *******************************************************************/

#include "AmazonDRS.h"
#include "Ultrasonic.h"

AmazonDRS DRS = AmazonDRS();

//WiFi creds ----------------------------------------------------------------------------------
char ssid[] = "****"; //  your network SSID (name)
char pass[] = "****";    // your network password (use for WPA, or use as key for WEP)
//------------------------------------------------------------------------------------------------------

#define slotNumber 1 //This will vary for multi slot devices 

const int TrigPin = 4;     //DIO number of the trigger pin
const int EchoPin = 5;     //DIO number of the echo pin
const in Bottle_height = 25; //Insert containers height (based on sensor position)
static long sensorTime = 0;    //millis of last sensor reading
static long waitingTime = 0;    //millis of last purchase
static String slotStatus = ""; //boolean which depicts if slot is available for replenishment
static String slotId = "";     //unique slot id ex: 0a5038b7-7609-4b81-b87e-3e291f386324 

Ultrasonic ultrasonic(TrigPin, EchoPin);

void setup() {
  Serial.begin(115200);

  //pinMode(EchoPin, INPUT);
  //pinMode(TrigPin, OUTPUT);
  
  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
   }

  //Start up DRS
  DRS.begin(ssid,pass);
  
  //initialize slots
  DRS.retrieveSubscriptionInfo();  //check slot statuses

  slotStatus = DRS.getSlotStatus(slotNumber);
  slotId = DRS.getSlotId(slotNumber);
}

void loop() {

  //Check for level reading
  //if the slot status is true proceed to request replenishment for the associated slot
  
  if (levelCheck() && ((millis() - waitingTime) > 10000))    //wait 10 secs
    {
      
       //Check if slot is available, if so replenish
                
        if(slotStatus == "true")   //if the product in slot are available 
        {
            waitingTime = millis();
            //we have a match! replenish the products associated with that slot!            
            DRS.requestReplenishmentForSlot(slotId);
            Serial.println("Buy new");
        }
        else
        {
          Serial.print("Sorry, slot ");
          Serial.print(slotId);
          Serial.println(" is not available at this time");
        }
        
    }

}

bool levelCheck(void)
{
  delay(1000);
  int sensor = ultrasonic.Ranging(CM);
  int level = Bottle_height-sensor;
  Serial.print("Level(cm): ");
  Serial.println(level);
  
  if(level < 5 && level > 0 && ((millis() - sensorTime) > 30000))  //wait 30 sec
  {
    sensorTime = millis();
    Serial.println(" Level Low!");
    return true;
  }
  else 
  {
    return false;
  }
}
