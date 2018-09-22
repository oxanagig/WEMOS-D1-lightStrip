#include <Homie.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

#define FW_NAME "ledstrip-control"
#define FW_VERSION "0.1.0"

#define PIN            D1
#define LED_COUNT      30

void ledHandler();
void loopHandler();
void colorWipe(uint32_t c, uint8_t wait);

int delayval = 50; 
int SoffitR;
int SoffitG;
int SoffitB;
int numPixels = LED_COUNT;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
Ticker LEDTimer;

HomieNode ledNode("led", "led");

void ledHandler(){
  static uint8_t alternate = 1;

  alternate^=1; 
  if(alternate == 1)
  {
    uint32_t newColor = random(0xFFFFFFFF);
    colorWipe(newColor,delayval);
  }
  else
  {
    colorWipe(0, delayval);
  }

}

void colorWipe(uint32_t c, uint8_t wait) {
  int i;
 
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

bool lightOnHandler(const HomieRange& range, const String& value) {

  Homie.getLogger()<< "the color is "<<value<<endl;

  LEDTimer.detach();

  if (value == "off") {
    colorWipe(strip.Color(0, 0, 0), delayval);    // Black/off
    ledNode.setProperty("color").send("0,0,0,0");
  } else if (value == "red") {
    colorWipe(strip.Color(255, 0, 0), delayval);  // Red
    ledNode.setProperty("color").send("255,0,0,0");
  } else if (value == "green") {
    colorWipe(strip.Color(0, 0, 255), delayval);  // Green
    ledNode.setProperty("color").send("0,255,0,0");
  } else if (value == "blue") {
    colorWipe(strip.Color(0, 255, 0), delayval);  // Blue
    ledNode.setProperty("color").send("0,0,255,0");
  } else if(value == "nightClub"){
    LEDTimer.attach_ms(100,ledHandler);
  } else {
      SoffitR = value.substring(0,value.indexOf(',')).toInt();
      SoffitG = value.substring(value.indexOf(',')+1,value.lastIndexOf(',')).toInt();
      SoffitB = value.substring(value.lastIndexOf(',')+1).toInt();
      colorWipe(strip.Color(SoffitR, SoffitB, SoffitG), delayval);
      String newColor = String(SoffitR) +','+String(SoffitG)+','+String(SoffitB)+",0";
      ledNode.setProperty("color").send(newColor);
  }
  return true;
}


void setup() {
  
  randomSeed(analogRead(0));

  Serial.begin(115200);
  Serial.println(FW_NAME FW_VERSION);
  
  strip.begin();

  Homie_setFirmware(FW_NAME, FW_VERSION);
  ledNode.setProperty("color").send("0,0,0,0");
  ledNode.advertise("color").settable(lightOnHandler);

  Homie.setup();


}

void loop() {
  //Homie.reset();
  Homie.loop();
}