#include <Arduino.h>
#include <TM1637Display.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <Adafruit_MAX31855.h>

// Module connection pins (Digital Pins)
#define CLK D1
#define DIO D2

#define PIN_MAXCS  D0

#define TFT_DC     D3
#define TFT_CS     D8
//#define TFT_RST    7 //3v3

#define TS_CS      D4
#define TS_IRQ    -1
// SCK D5
// MISO D6
// MOSI D7


#define NB_PTS    300
#define GRAPH_H   220

#define DELAY     2000

#define GRAD_VERT   9
#define GRAD_HORI   10


TM1637Display display(CLK, DIO);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);//, TFT_RST);
XPT2046_Touchscreen ts(TS_CS, TS_IRQ);
Adafruit_MAX31855 thermocouple(PIN_MAXCS);


unsigned long previousMillis = 0;

byte points[NB_PTS] = {0};
int indice = 0;

const uint8_t SEG_MOINS[1] = { SEG_G };
const uint8_t SEG_RIEN[1] = { 0 };

void setup()
{
  Serial.begin(115200);
  Serial.println("BOOT...");
  
  display.setBrightness(7, true); // Turn on
  Serial.println("1");
  
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  Serial.println("2");
  
  ts.begin();
  Serial.println("3");
  delay(1000);

  displayAxes();
  Serial.println("4");

  // Thermocouple
  thermocouple.begin();
  Serial.println("5");

  display.showNumberDec(8888, false, 4, 3);

  Serial.println("BOOT OK");
}

void loop()
{
  Serial.println("LOOP...");
  unsigned long currentMillis = millis();
  Serial.println("5");
  
  if (currentMillis - previousMillis >= DELAY)
  {
    Serial.println("6");
    float t = thermocouple.readCelsius();
    Serial.println(t);

    points[indice] = map(t, 0, 400, 0, GRAPH_H);
    indice = (indice + 1) % NB_PTS;
    Serial.println("7");

    if(t < 0)
    {
      display.setSegments(SEG_MOINS, 1, 0);
    }
    else
    {
      display.setSegments(SEG_RIEN, 1, 0);
    }
    Serial.println("8");
    display.showNumberDec(int(abs(t)), false, 3, 1);

    Serial.println("9");
    displayGraph();
    Serial.println("10");
  
    previousMillis = currentMillis;
    
  }
  Serial.println("11");
  delay(10);
  
  

}


void displayGraph()
{

  for(int i = 0; i < NB_PTS; i++)
  {
    int h = points[(i + indice) % NB_PTS];
    tft.drawFastVLine(320 - NB_PTS + i, 0, GRAPH_H - h, ILI9341_BLACK);
    tft.drawFastVLine(320 - NB_PTS + i, GRAPH_H - h, h, ILI9341_BLUE);
  }

}

void displayAxes()
{
  tft.drawFastVLine(320 - NB_PTS - 1, 0, GRAPH_H + 1, ILI9341_RED);
  tft.drawFastHLine(320 - NB_PTS - 1, GRAPH_H + 1, NB_PTS + 1, ILI9341_RED);
  
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(1);

  for(int y = 0; y < GRAD_VERT; y++)
  {
    int n = (GRAD_VERT - 1 - y) * 50;
    String str;
    if(n < 10)
      str = String("  ") + String(n);
    else if(n < 100)
      str = String(" ") + String(n);
    else
      str = String(n);

    tft.setCursor(0, y * (GRAPH_H - 4) / (GRAD_VERT - 1));
    tft.print(str);
  }

  for(int x = 0; x < GRAD_HORI; x++)
  {
    int offset = 0;
    int t = (GRAD_HORI - 1 - x) * NB_PTS / GRAD_HORI * DELAY / 1000;
    if(t < 10)
      offset = 3;
    else if(t < 100)
      offset = 6;
    else
      offset = 9;
      
    tft.setCursor(320 - NB_PTS + x * NB_PTS / (GRAD_HORI - 1) - offset - 3, GRAPH_H + 8);
    tft.print(String(t));
  }

  

}

