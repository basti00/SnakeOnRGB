// use the cRGB struct hsv method
#define USE_HSV

#include <WS2812.h>

#define LEDCount 64
#define ledDataPin 7
#define a_but 3
#define b_but 4
#define down_but 5
#define up_but 6
#define right_but 8
#define left_but 9

WS2812 LED(LEDCount);
cRGB value;

int h = 0;   //stores 0 to 614
byte steps = 55; //number of hues we skip in a 360 range per update

byte sat = 255;//100;
byte val = 10;

long sleep = 50; //delays between update

void setup() {
  pinMode(a_but, INPUT_PULLUP);
  pinMode(b_but, INPUT_PULLUP);
  pinMode(up_but, INPUT_PULLUP);
  pinMode(down_but, INPUT_PULLUP);
  pinMode(left_but, INPUT_PULLUP);
  pinMode(right_but, INPUT_PULLUP);

  LED.setOutput(ledDataPin);
  for(int i = 0; i < LEDCount; i++)
  {
    value.SetHSV(h, sat, 0);
    LED.set_crgb_at(i, value);
  }
}

void loop() {
  uint64_t time_now = millis();
  int static s = 0;
  s++;
  int static state;
  if(s == 64)
    state+=steps;
  state %= 360;
  s %= 64;

  for(int i = 0; i < LEDCount; i++)
  {
    value.SetHSV(state, sat, 0);

    if(i/8 == a_but-3 && digitalRead(a_but) == LOW)
      value.SetHSV(state, sat, val);
    else if(i/8 == b_but-3 && digitalRead(b_but) == LOW)
      value.SetHSV(state, sat, val);
    else if(i/8 == up_but-3 && digitalRead(up_but) == LOW)
      value.SetHSV(state, sat, val);
    else if(i/8 == down_but-3 && digitalRead(down_but) == LOW)
      value.SetHSV(state, sat, val);
    else if(i/8 == left_but-3 && digitalRead(left_but) == LOW)
      value.SetHSV(state, sat, val);
    else if(i/8 == right_but-3 && digitalRead(right_but) == LOW)
      value.SetHSV(state, sat, val);

    LED.set_crgb_at(i, value);

  }

  LED.sync();
  while(millis() < time_now + sleep)
    ;
}

unsigned int rng(){
  uint64_t static seed = 1000;
  seed = ((seed * 344534537681)+4534535809)%875644564849;
  //seed = ((seed * 23)+7)%1012;
  return (unsigned int) seed % 360;
}