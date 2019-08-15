#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "thermo.xbm"


#define DISPLAY_RESET_PIN U8X8_PIN_NONE
#define DISPLAY_CLOCK_PIN SCL
#define DISPLAY_DATA_PIN SDA
#define DISPLAY_FONT u8g2_font_inb21_mf
#define THERMO_PIN 2
#define LOOP_INTERVAL 1000 // ms
//#define DEBUG


OneWire oneWire(THERMO_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress thermo;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, DISPLAY_RESET_PIN, DISPLAY_CLOCK_PIN, DISPLAY_DATA_PIN);

char thermo_buff[10] = "";

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFontPosTop();

  sensors.begin();
  sensors.getAddress(thermo, 0);
}

void loop() {
  // Sensor
  sensors.requestTemperaturesByAddress(thermo);
  sprintf(thermo_buff, "%d°C", (int)(sensors.getTempC(thermo)));
  #ifdef DEBUG
  Serial.print("thermo_buff = ");
  Serial.println(thermo_buff);
  #endif

  // Display
  u8g2.setFont(DISPLAY_FONT);
  u8g2.setFontDirection(0);

  u8g2.clearBuffer();

  u8g2.drawXBM(8, 0, thermo_width, thermo_height, thermo_bits);

  u8g2.setCursor(128 - u8g2.getUTF8Width(thermo_buff), 8);
  u8g2.print(thermo_buff);

  u8g2.sendBuffer();

  delay(LOOP_INTERVAL);
}