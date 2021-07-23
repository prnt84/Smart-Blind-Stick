#include <WiFi.h>
#include <WiFiClient.h>
#include "TinyGPS++.h"
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information
#include <Wire.h>
 

TinyGPSPlus gps;

WiFiClient client;
volatile float minutes, seconds;
volatile int degree, secs, mins;

String latitude,longitude;
int gpsbutton=8;
const int trigger = 5;
const int echo = 6;
const int buzzer = 18;
const int button = 19;
int onOff = HIGH;
int buttonState;
int gpsbuttonstate;
int previousButtonState = HIGH;
long previousDebounceTime = 0;
long debounceDelay = 50;
int calls = 1;   // Execution count, so this doesn't run forever
int maxCalls = 1;   // Maximum number of times the Choreo should be executed



void setup() {
  // put your setup code here, to run once:
pinMode(echo, INPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(trigger, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin (9600);
  pinMode(gpsbutton,INPUT_PULLUP);
  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID,WIFI_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  Serial.println("Setup complete.\n");
  ultrasonic();

}

void gpslocation()
{
  smartDelay(1000);  /* Generate precise delay of 1ms */
if (!gps.location.isValid())
{          
Serial.print("Latitude : ");
Serial.println("*****");
Serial.print("Longitude : ");
Serial.println("*****");
}
else
{

Serial.print("Latitude in Decimal Degrees : ");
Serial.println(gps.location.lat(), 6);
latitude=gps.location.lat();
Serial.print("Longitude in Decimal Degrees : ");
Serial.println(gps.location.lng(), 6);
longitude=gps.location.lng();
 
 if (calls <= maxCalls) {
     String link = String("www.google.com/maps/search/?api=1&query=" + latitude + "," + longitude);
  String msg = String("Help me! Click on this link to see my location: " + link);
      
    Serial.println("Running SendSMS - Run #" + String(calls++));

     
     
     
    TembooChoreo SendSMSChoreo(client);

    // Invoke the Temboo client
    SendSMSChoreo.begin();

    // Set Temboo account credentials
    SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String AuthTokenValue = "236668f4b7611ec5723176c7a5dcf858";
    SendSMSChoreo.addInput("AuthToken", AuthTokenValue);
    String ToValue = "+919805059541";
    SendSMSChoreo.addInput("To", ToValue);
    
    String FromValue = "+12024106678";
    SendSMSChoreo.addInput("From", FromValue);
    String BodyValue = msg;
    SendSMSChoreo.addInput("Body", BodyValue);
    String AccountSIDValue = "ACe162e3811eb82bddbc3a89a18cd275f4";
    SendSMSChoreo.addInput("AccountSID", AccountSIDValue);

    // Identify the Choreo to run
    SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

    // Run the Choreo; when results are available, print them to serial
    // 901 time to wait for a Choreo response. Can be edited as needed
    // USE_SSL input to tell library to use HTTPS
    SendSMSChoreo.run(901, USE_SSL);
    
    while(SendSMSChoreo.available()) {
      char c = SendSMSChoreo.read();
      Serial.print(c);
    }
    SendSMSChoreo.close();
  
  Serial.println("\nWaiting...\n");
  delay(30000); // wait 30 seconds between SendSMS calls
  }
}
}

void ultrasonic()
{
  // put your main code here, to run repeatedly: 
  int reading = digitalRead(button);
  Serial.println(reading);
  if (reading != previousButtonState)
  {
    previousDebounceTime = millis();
    
  }
  if ((millis() - previousDebounceTime) > debounceDelay)
  {
    if(reading!=buttonState)
    {
    buttonState = reading;
    if(buttonState==HIGH)
    {
      onOff=!onOff;
    }
  }
  }
  if (onOff == buttonState)
  {
    long duration, distance;
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    duration = pulseIn(echo, HIGH);
    distance = (duration / 2) / 29.1;
    if (distance < 10)
    {
      digitalWrite(buzzer, HIGH);
      delay(200);
     digitalWrite(buzzer, LOW);
      delay(100);
    }
    else {
      if (distance < 20)
      {
         digitalWrite(buzzer, HIGH);
        delay(300);
    digitalWrite(buzzer, LOW);
        delay(100);
      }
      else {
        if (distance < 30)
        {
          digitalWrite(buzzer, HIGH);
          delay(400);
          digitalWrite(buzzer, LOW);
          delay(100);
        }
        else {
          if (distance < 40)
          {
            digitalWrite(buzzer, HIGH);
            delay(500);
           digitalWrite(buzzer, LOW);
            delay(100);
          }
          else {
            if (distance < 50)
            {
           digitalWrite(buzzer, HIGH);
              delay(600);
             digitalWrite(buzzer, LOW);
              delay(100);
            }
            
                  }
                }
              }
            }
          
        
      
    
    if (distance >= 50 || distance <= 0) {
      Serial.println("Not in range.");
    }
    else {
      Serial.print(distance);
      Serial.println(" cm");
    }
  }
  previousButtonState = reading;
  delay(500);
}
static void smartDelay(unsigned long ms)
{
unsigned long start = millis();
do 
{
while (Serial.available())  /* Encode data read from GPS while data is available on serial port */
gps.encode(Serial.read());
    /* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
} while (millis() - start < ms);
  }


void loop() {
  // put your main code here, to run repeatedly: 
  gpsbuttonstate=digitalRead(gpsbutton);
  
  if(gpsbuttonstate==LOW)
  {
    gpslocation();
  }
  else
  {
    ultrasonic();
  }
}
