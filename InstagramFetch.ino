/*******************************************************************
 *  An example of usisng the InstagramStats library to get
 *  info on a given user.
 *
 *  Written by Brian Lough and updated by Adrian Pecker
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
 *******************************************************************/

#include "InstagramStats.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Arduino.h"
#include <WiFiClientSecure.h>
#include "JsonStreamingParser.h"
#include <ArduinoJson.h>
#include "theSwedishMaker.h"

// ----------------------------
// Standard Libraries - Already Installed if you have ESP8266 set up
// ----------------------------

#include <ESP8266WiFi.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

// Define the typ of hardware and the pins used. 

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN   12
#define DATA_PIN  15
#define CS_PIN    13

// Hardware SPI connection
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
char ssid[] = "***************";         // your network SSID (name)
char password[] = "*************"; // your network key

String INSTAGRAM_ACCESS_TOKEN = "*******************";
String IG_USER_ID = "****************";

WiFiClientSecure client;
InstagramStats instaStats(client, INSTAGRAM_ACCESS_TOKEN, IG_USER_ID);

unsigned long delayBetweenChecks = 600000; //mean time between api requests
unsigned long whenDueToCheck = 0;

void setup()
{
  P.begin();
  P.setFont(fontSubs);

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  //Set client insecure for https connections
  client.setInsecure();
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // If using ESP8266 Core 2.5 RC, uncomment the following
  // client.setInsecure();
}

void getInstagramFollowersForUser()
{
  Serial.println("Getting instagram user stats for " + IG_USER_ID);
  P.print("fetching");
  int instaFollowersCount = instaStats.getFollowersCount(IG_USER_ID);

  

  String instaFollowersCount2 = num_format(instaFollowersCount);
  Serial.println("Response:");
  Serial.print("Number of followers: ");
  Serial.println(instaFollowersCount);
  P.print("&" + instaFollowersCount2 ); 
}


void loop()
{
  unsigned long timeNow = millis();
  if ((timeNow > whenDueToCheck))
  {
    getInstagramFollowersForUser();
    whenDueToCheck = timeNow + delayBetweenChecks;
  }
}
String num_format(long num){
     String num_s;
     long num_original = num;
     if (num>99999 && num<=999999){
        num = num / 1000;
        long fraction = num_original%1000;
        String num_fraction = String(fraction);
        String decimal = num_fraction.substring(0,1);
        num_s = String(num) + "." + decimal + "K";          
    }
    else if(num>999999){
        num = num / 1000000;
        long fraction = num_original%1000000;
        String num_fraction = String(fraction);
        String decimal = num_fraction.substring(0,1);
        if (num_original<100000000){
          num_s = " " + String(num) + "." + decimal + "M";      
        }
        else{
        num_s = String(num) + "." + decimal + "M";
        }
    }
    else{
        int num_l = String(num).length();
        char num_f[15];
        int blankDigits = 6 - num_l;
        for(int i = 0; i < blankDigits; i++){
          num_f[i] = ' '; 
        }
        num_f[blankDigits] = '\0';
        num_s = num_f + String(num);
    }
    return num_s;
}   
