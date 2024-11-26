// Program to show time and temperature on led matrix display
//
// for D1 mini ESP8266
// uses DHT20 and DS3231
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//
// 05-04-2024 V1.0

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <RTClib.h>
#include <DFRobot_DHT20.h>
#include "Font_Data.h"

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   11
#define DATA_PIN  13
#define CS_PIN    15

struct animations                                               
{
  textEffect_t   effecIn;       // Animation type In
  textEffect_t   effecOut;      // Animation type Out - In this example there are the same, but you can mix them.
  const char *   pText;         // Text to display
  uint16_t       speed;         // Animation speed (multiplier for library default) larger number = SLOWER
  uint16_t       pause;         // pause (multiplier for library default)  larger number = SLOWER
  textPosition_t align;         // PA_LEFT, PA_CENTER, PA_RIGHT
};

char time_str [10];
char tmp_str [10];
float temp = 0.0;
char szMesg [10];

animations animList[] =
{
  { PA_SCROLL_DOWN,  PA_FADE, time_str, 2, 3000, PA_CENTER },
  { PA_SCROLL_DOWN,  PA_FADE, tmp_str, 2, 3000, PA_CENTER },
};

int i;

uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 };

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

RTC_DS3231 rtc;
DFRobot_DHT20 dht20;

void setup(void)
{
  P.begin();
  P.setFont(dig5x8sq);  
  Serial.begin(115200);

  delay(3000); // wait for console opening


  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    P.print("no RTC");
    delay(1000);
    while (1);
  }

 //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
   //Initialize sensor
  while(dht20.begin()){
    Serial.println("Initialize sensor failed");
    delay(1000);
  }

  P.addChar('$', degC);  // where ever we put $ the ⁰C will appear with the superscript like o
}

void loop(void)
{
  DateTime now = rtc.now();  
  temp = dht20.getTemperature();
  dtostrf(temp, 3, 1, szMesg); //convert float to string
  //Time
  sprintf (time_str, "%02d:%02d", now.hour(), now.minute());
  sprintf (tmp_str, "%s$", szMesg);

  Serial.print("Current time = ");
  Serial.println(time_str);
  //Get ambient temperature
  Serial.print("temperature:"); Serial.print(dht20.getTemperature());Serial.print("°C");
  //Get relative humidity
  Serial.print("  humidity:"); Serial.print(dht20.getHumidity()*100);Serial.println("%RH");

  static uint8_t i = 0;                                                   // text effect index

  if (P.displayAnimate())                                                 // animates and returns true when an animation is completed
  {
    if (i == ARRAY_SIZE(animList))i = 0;                                  // reset loop index

        P.displayText(animList[i].pText, animList[i].align, P.getSpeed(), animList[i].pause, animList[i].effecIn, animList[i].effecOut);
    //delay(3000);
    i++;                                                                  // then set up for next text effect
  }
  }

 

