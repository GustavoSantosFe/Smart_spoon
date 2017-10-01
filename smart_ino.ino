#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void draw_temp(int temp);
void draw_wait();
void turn_off();
void power_cycle();

#define THERMISTORPIN A0
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define NUMSAMPLES 5
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000

uint16_t samples[NUMSAMPLES];

float read_temp();

int heat_pin = 10;

int power_button = 7;
int powr_flag = 1;
int heating_flag = 0;

static const unsigned char PROGMEM logo[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00,
  0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00,
  0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xF9, 0xD9, 0x8F, 0x9D, 0x78, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x01, 0xFD, 0xFF, 0xDF, 0xDF, 0x78, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x01, 0xDD, 0xDD, 0xDD, 0xDF, 0x70, 0x00, 0x00,
  0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x01, 0xC1, 0xDD, 0xDD, 0xDC, 0x70, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0xF1, 0xDD, 0xC7, 0xDC, 0x70, 0x00, 0x00,
  0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x7D, 0xDD, 0xDF, 0xDC, 0x70, 0x00, 0x00,
  0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x1D, 0xDD, 0xDD, 0xDC, 0x70, 0x00, 0x00,
  0x03, 0xFF, 0xFF, 0xE0, 0x00, 0x7F, 0xFF, 0xF0, 0x01, 0xDD, 0xDD, 0xDD, 0xDC, 0x70, 0x00, 0x00,
  0x07, 0xFF, 0xFF, 0x00, 0x00, 0x3F, 0xFF, 0xF0, 0x01, 0xFD, 0xDD, 0xDF, 0xDC, 0x78, 0x00, 0x00,
  0x0F, 0xFF, 0xFE, 0x00, 0x03, 0x1F, 0xFF, 0xE0, 0x00, 0xF9, 0xDD, 0xCD, 0xDC, 0x78, 0x00, 0x00,
  0x0F, 0xFF, 0xFC, 0x00, 0x01, 0x87, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFC, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFC, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFC, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFC, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFE, 0x00, 0x01, 0x87, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFE, 0x00, 0x03, 0x0F, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFF, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00,
  0x1F, 0xFF, 0xFF, 0xF8, 0x01, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xC0,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xF9, 0xD8, 0xF8, 0xF9, 0xD8, 0xC0, 0x60,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFD, 0xFD, 0xFD, 0xFD, 0xFC, 0x9E, 0x20,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0x11, 0x10,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xC1, 0xDD, 0xDD, 0xDD, 0xDD, 0x11, 0x10,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xF1, 0xDD, 0xDD, 0xDD, 0xDD, 0x1E, 0x10,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x7D, 0xDD, 0xDD, 0xDD, 0xDD, 0x12, 0x10,
  0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x1D, 0xDD, 0xDD, 0xDD, 0xDC, 0x91, 0x20,
  0x1F, 0xFF, 0xFF, 0xF8, 0x00, 0x1F, 0xFF, 0xFF, 0xE1, 0xDD, 0xDD, 0xDD, 0xDD, 0xDC, 0xC0, 0x60,
  0x1F, 0xFF, 0xFF, 0xF0, 0x00, 0x07, 0xFF, 0xFF, 0xE1, 0xFD, 0xFD, 0xFD, 0xFD, 0xDC, 0x60, 0xC0,
  0x0F, 0xFF, 0xFF, 0xE0, 0x00, 0x03, 0xFF, 0xFF, 0xE0, 0xF9, 0xD8, 0xF8, 0xF9, 0xDC, 0x1F, 0x00,
  0x07, 0xFF, 0xFF, 0x86, 0x00, 0x01, 0xFF, 0xFF, 0xE0, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0xFF, 0xCC, 0x00, 0x01, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x07, 0xFF, 0xE6, 0x00, 0x03, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0xE0, 0x00, 0x0F, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0xF8, 0x00, 0x3F, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x3F, 0xFF, 0xFE, 0x01, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x6C, 0x00,
  0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0C, 0x00,
  0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x03, 0xED, 0x1B, 0x6F, 0x9C,
  0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x03, 0x6D, 0x1B, 0x6D, 0xB6,
  0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x03, 0x6D, 0x1B, 0x6D, 0xB6,
  0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x66, 0x1B, 0x6D, 0xBE,
  0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x66, 0x0A, 0x6D, 0xB0,
  0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x66, 0x0A, 0x6D, 0xB6,
  0x00, 0x00, 0x00, 0x1F, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE6, 0x0E, 0x6F, 0x9C,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM end_bkg[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xC0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xE1, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x38,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x18,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x18,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x18,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x38,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x38,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x70,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x7F, 0x90,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x20, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x90, 0x70,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x88, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x85, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC3, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC7, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xEC, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xC0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC9, 0xC0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6B, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7D, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x1E, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x1E, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x03, 0x80, 0x1E, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x80, 0x03, 0xC0, 0x0E, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xC1, 0xE0, 0x06, 0xC0, 0x0E, 0x00,
  0x00, 0x00, 0x00, 0x07, 0xE3, 0xF0, 0x00, 0x00, 0x1F, 0xFC, 0xC0, 0x70, 0xFC, 0x7E, 0x0C, 0x00,
  0x00, 0x00, 0x00, 0x06, 0x6B, 0x30, 0x00, 0x03, 0xFF, 0xE1, 0x80, 0x30, 0xCD, 0x66, 0x0C, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x36, 0x60, 0x00, 0x3F, 0xE3, 0x83, 0x00, 0x38, 0x66, 0xCC, 0x0C, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x63, 0x60, 0x07, 0xFE, 0x43, 0x05, 0x00, 0x18, 0x6C, 0x6C, 0x04, 0x00,
  0x00, 0x00, 0x00, 0x01, 0xC1, 0xC0, 0xFF, 0xDE, 0x27, 0x89, 0x00, 0x18, 0x38, 0x38, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0xC1, 0xC1, 0xFF, 0xB3, 0x18, 0xD1, 0x00, 0x18, 0x38, 0x38, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x63, 0x60, 0x0F, 0xFD, 0xB8, 0x61, 0x00, 0x18, 0x6C, 0x6C, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x36, 0x60, 0x00, 0x7F, 0xE4, 0x71, 0x00, 0x38, 0x66, 0xCC, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x06, 0x6B, 0x30, 0x00, 0x03, 0xFF, 0x91, 0x80, 0x30, 0xCD, 0x66, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x07, 0xE3, 0xF0, 0x00, 0x00, 0x1F, 0xFC, 0xC0, 0x70, 0xFC, 0x7E, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x01, 0xFE, 0xF1, 0xE0, 0x06, 0xC0, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0x80, 0x03, 0xC0, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x03, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};

void setup(void) {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.drawBitmap(0, 0,  logo, 128, 64, 1);
  display.display();
  delay(5000);
  pinMode(power_button, INPUT);
  analogReference(EXTERNAL);
  pinMode(heat_pin, OUTPUT);
}

void loop(void)
{
  if(powr_flag > 0){
    display.clearDisplay();
    int curr_temp = read_temp();
    if(powr_flag == 1)
      draw_wait(curr_temp);
    else
      draw_temp();
    if(curr_temp <= 50.0){
      heating_flag = 1;
      digitalWrite(heat_pin, HIGH);
    }
    else{
      heating_flag = 0;
      digitalWrite(heat_pin, LOW);
    }
    delay(50);
  }
  else{
    turn_off();
  }
  display.display();
  if(digitalRead(power_button)==HIGH)
    power_cycle();

}

void power_cycle(){
  int timer = 0;
  int state = digitalRead(power_button) == HIGH ? 1 : 0;
  while (state) {
    delay(100);
    timer++;
    for(int i = 0;i< NUMSAMPLES && state == 1; i++){
      state = digitalRead(power_button) == HIGH ? 1 : 0;
    }
    if(timer > 10){
      turn_off();
      display.display();
    }
  }
  if (timer < 10) {
    if(powr_flag == 2 || powr_flag == 0)
      powr_flag = 1;
    else
      powr_flag = 2;
  }
  else
    powr_flag = 0;
}

void draw_temp(int temp){
  display.clearDisplay();
  if(temp >= 50.0){
    display.drawBitmap(0, 0,  end_bkg, 128, 64, 1);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20,8);
    display.println("Pode se \n servir!");
  }
  else{
    char out_temp[6];
    sprintf(out_temp, "%d C", temp);
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(25,20);
    display.println(out_temp);
    display.drawCircle(65, 20, 4, WHITE);
    display.drawRect(20, 15, 80, 30, WHITE);
  }
  display.drawRect(0, 0, 5, 64, WHITE);
  if(temp >= 25){
    int progress = (((temp-25)*100)/25)*0.64;
    display.fillRect(1, 64 - progress, 4, progress , WHITE);
  }

  display.display();
}

void draw_wait(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(6,5);
  display.println("aperte para aquecer");
  display.drawCircle(60, 40, 11, WHITE);
  display.fillRect(55, 25, 11, 11, BLACK);
  display.fillRect(58, 25, 3, 15, WHITE);
  display.setCursor(0,56);
  display.println("*segure para desligar");
  display.display();
}

float read_temp(){
  uint8_t i;
  float average;

  for (i=0; i< NUMSAMPLES; i++) {
    samples[i] = analogRead(THERMISTORPIN);
    delay(10);
  }

  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;

  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  float steinhart;
  steinhart = average / THERMISTORNOMINAL;
  steinhart = log(steinhart);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  delay(10);

  return steinhart;
}

void turn_off(){
  display.clearDisplay();
  if(heating_flag){
    heating_flag = 0;
    digitalWrite(heat_pin, LOW);
  }
}

