//NodeMCU  Wemos D1
//Arduino as ISP
#include <SPI.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>


#define LED_PIN D6
#define LED_COUNT 6

static uint16_t hue = 0; // 0-359
uint8_t saturation = 100; // 0-100
uint8_t lightness = 50; // 0-100
unsigned int PColor=0;
unsigned int Hour1 = 0;
unsigned int Hour2 = 0;
unsigned int Minute1 = 0;
unsigned int Minute2 = 0;
unsigned int Second1 = 0;
unsigned int Second2 = 0;


const char *ssid     = "SSID";
const char *password = "PASSWORD";

const long utcOffsetInSeconds = 3600*-5;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//arduino
//const int dataPin = 13;   //Outputs the byte to transfer
//const int loadPin = 11;   //3 //Controls the internal transference of data in SN74HC595 internal registers
//const int clockPin = 12;  // 4//Generates the clock signal to control the transference of data

//nodeMCU
//const int dataPin = 14;   //Outputs the byte to transfer
//const int loadPin = 0;   //3 //Controls the internal transference of data in SN74HC595 internal registers
//const int clockPin = 4;  // 4//Generates the clock signal to control the transference of data

//wemos
const int dataPin = 14;  //D5 //Outputs the byte to transfer
const int loadPin = 0;   //D3 //3 //Controls the internal transference of data in SN74HC595 internal registers
const int clockPin = 4;  //D2 // 4//Generates the clock signal to control the transference of data

byte digits[10] {
  0b11111100, //0
  0b01100000, //1
  0b11011010, //2
  0b11110010, //3
  0b01100110, //4
  0b10110110, //5
  0b10111110, //6
  0b11100000, //7
  0b11111110, //8
  0b11100110  //9
};

void setup() {
  Serial.begin(115200);
  
  pinMode(dataPin, OUTPUT);
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);  
  digitalWrite(loadPin, 0);
  shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
  shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
  shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
  shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
  shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
  shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
  digitalWrite(loadPin, 1);

  WiFi.begin(ssid, password);
  int i=0;
  int b=0;
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    digitalWrite(loadPin, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, digits[i]); //right
    shiftOut(dataPin, clockPin, LSBFIRST, digits[b]); //left
    shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
    shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
    digitalWrite(loadPin, 1);
    i++;
    if (i==10) {
      i=0;
      b++;
    }
    if (b==9 && i==9) {
      digitalWrite(loadPin, 0);
      shiftOut(dataPin, clockPin, LSBFIRST, 0b00000001); //right
      shiftOut(dataPin, clockPin, LSBFIRST, 0b00001011); //right
      shiftOut(dataPin, clockPin, LSBFIRST, 0b00111010); //00111111
      shiftOut(dataPin, clockPin, LSBFIRST, 0b00001010); //right
      shiftOut(dataPin, clockPin, LSBFIRST, 0b00001010); //right
      shiftOut(dataPin, clockPin, LSBFIRST, 0b10011110); //left
      digitalWrite(loadPin, 1);
      delay(10000);
      i=0;
      b=0;
    }
  }
  timeClient.begin();
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(25); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void chase() {
  byte disp = 0;
  for (unsigned int j=0; j<5; j++) {
    for (unsigned int i=0; i<8; i++) {
      digitalWrite(loadPin, 0);
      bitSet(disp, i);
      shiftOut(dataPin, clockPin, LSBFIRST, disp); //right
      shiftOut(dataPin, clockPin, LSBFIRST, disp); //left
      digitalWrite(loadPin, 1);
      delay(25);
      bitClear(disp, i);
    }  
  }
}

void casino() {
  for (unsigned int i=0; i<20; i++) {
    digitalWrite(loadPin, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, digits[random(0,9)]); //right
    shiftOut(dataPin, clockPin, LSBFIRST, digits[random(0,9)]); //left
    shiftOut(dataPin, clockPin, LSBFIRST, digits[random(0,9)]); //left
    shiftOut(dataPin, clockPin, LSBFIRST, digits[random(0,9)]); //left
    shiftOut(dataPin, clockPin, LSBFIRST, digits[random(0,9)]); //left
    shiftOut(dataPin, clockPin, LSBFIRST, digits[random(0,9)]); //left
    digitalWrite(loadPin, 1);
    delay(25);
  }  
}

/**
 * Map HSL color space to RGB
 * 
 * Totally borrowed from:
 * http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/ 
 * 
 * Probably not the most efficient solution, but 
 * it get's the job done.
 */
uint32_t hsl(uint16_t ih, uint8_t is, uint8_t il) {

  float h, s, l, t1, t2, tr, tg, tb;
  uint8_t r, g, b;

  h = (ih % 360) / 360.0;
  s = constrain(is, 0, 100) / 100.0;
  l = constrain(il, 0, 100) / 100.0;

  if ( s == 0 ) { 
    r = g = b = 255 * l;
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
  } 
  
  if ( l < 0.5 ) t1 = l * (1.0 + s);
  else t1 = l + s - l * s;
  
  t2 = 2 * l - t1;
  tr = h + 1/3.0;
  tg = h;
  tb = h - 1/3.0;

  r = hsl_convert(tr, t1, t2);
  g = hsl_convert(tg, t1, t2);
  b = hsl_convert(tb, t1, t2);

  // NeoPixel packed RGB color
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}
/**
 * HSL Convert
 * Helper function
 */
uint8_t hsl_convert(float c, float t1, float t2) {

  if ( c < 0 ) c+=1; 
  else if ( c > 1 ) c-=1;

  if ( 6 * c < 1 ) c = t2 + ( t1 - t2 ) * 6 * c;
  else if ( 2 * c < 1 ) c = t1;
  else if ( 3 * c < 2 ) c = t2 + ( t1 - t2 ) * ( 2/3.0 - c ) * 6;
  else c = t2;
  
  return (uint8_t)(c*255); 
}

void loop() {
  timeClient.update();

  unsigned int b=0;
  unsigned int Hour = timeClient.getHours();
  unsigned int Minute = timeClient.getMinutes();
  unsigned int Seconds = timeClient.getSeconds();
  digitalWrite(loadPin, 0);
  shiftOut(dataPin, clockPin, LSBFIRST, 0b000000000000000000000000000000000000000000000000);
  digitalWrite(loadPin, 1);
  chase();
  while (Seconds<60) {
    Hour1 = (Hour/10U) % 10;
    Hour2 = (Hour/1U) % 10;
    Minute1 = (Minute/10U) % 10;
    Minute2 = (Minute/1U) % 10;
    Second1 = (Seconds/10U) % 10;
    Second2 = (Seconds/1U) % 10;
    unsigned int pause = 1000;
    for (b=0; b<6; b++) {
      strip.setPixelColor(b, hsl(PColor, saturation, lightness));
      strip.show();
    }

    PColor++;
    if (PColor>360) {
      PColor=0;
    }

    if (Second2==0) {
      casino();
      pause=500;
    } else {
      pause = 1000;
    }
    digitalWrite(loadPin, 0);
    shiftOut(dataPin, clockPin, LSBFIRST, digits[Second2]); //right
    shiftOut(dataPin, clockPin, LSBFIRST, digits[Second1]); //left
    shiftOut(dataPin, clockPin, LSBFIRST, digits[Minute2]); //right
    shiftOut(dataPin, clockPin, LSBFIRST, digits[Minute1]); //left
    shiftOut(dataPin, clockPin, LSBFIRST, digits[Hour2]); //right
    shiftOut(dataPin, clockPin, LSBFIRST, digits[Hour1]); //left
    digitalWrite(loadPin, 1);
    delay(pause);
    Seconds = Seconds+1;
  }
}
