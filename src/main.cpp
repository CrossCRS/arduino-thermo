#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "thermo.xbm"

#define EEPROM_SHIFT 55
#define EEPROM_ADDR_TEMP_MIN 0
#define EEPROM_ADDR_TEMP_MAX 1
#define DISPLAY_RESET_PIN U8X8_PIN_NONE
#define DISPLAY_CLOCK_PIN SCL
#define DISPLAY_DATA_PIN SDA
#define DISPLAY_FONT_SMALL u8g2_font_helvB12_tf
#define DISPLAY_FONT_BIG u8g2_font_inb21_mf
#define THERMO_PIN 9
#define BUTTON_PIN 3
#define LOOP_INTERVAL 1000 // ms
//#define DEBUG

volatile long debounce_time = 0;
volatile long current_time = 0;
volatile byte display_state = 0;

volatile int temp_min = 127;
volatile int temp_max = -127;

OneWire oneWire(THERMO_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress thermo;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, DISPLAY_RESET_PIN, DISPLAY_CLOCK_PIN, DISPLAY_DATA_PIN);

char thermo_buff[10] = "";
char buff[20] = "";

void button_event() {
  current_time = millis();
  if ((current_time - debounce_time) > 200) {
    display_state = !display_state;
    Serial.println("button_event");
  }
  debounce_time = current_time;
}

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_event, FALLING);

  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFontPosTop();

  sensors.begin();
  sensors.getAddress(thermo, 0);
}

int temp;
void loop() {
  // Sensor
  sensors.requestTemperaturesByAddress(thermo);
  temp = (int)(sensors.getTempC(thermo));
  //temp = (int)42;
  if (temp <= 99 && temp >= -99) { // Accept sane data
    sprintf(thermo_buff, "%d°C", temp);
    if (temp < temp_min) {
      temp_min = temp;
    }
    if (temp > temp_max) {
      temp_max = temp;
    }
  }
  #ifdef DEBUG
  Serial.print("thermo_buff = ");
  Serial.println(thermo_buff);
  #endif

  // Display
  u8g2.setFont(DISPLAY_FONT_BIG);
  u8g2.setFontDirection(0);

  u8g2.clearBuffer();

  if (display_state == 0) { // Current temperature
    u8g2.drawXBM(8, 0, thermo_width, thermo_height, thermo_bits);

    u8g2.setCursor(128 - u8g2.getUTF8Width(thermo_buff), 8);
    u8g2.print(thermo_buff);
  } else if (display_state == 1) { // Min/max temperatures
    u8g2.setFont(DISPLAY_FONT_SMALL);
    u8g2.drawXBM(8, 0, thermo_width, thermo_height, thermo_bits);

    u8g2.setCursor(35, 0);
    u8g2.print("min");
    u8g2.setCursor(35, 16);
    u8g2.print("max");

    sprintf(buff, "%d°C", temp_min);
    u8g2.setCursor(128 - u8g2.getUTF8Width(buff), 0);
    u8g2.print(buff);

    sprintf(buff, "%d°C", temp_max);
    u8g2.setCursor(128 - u8g2.getUTF8Width(buff), 16);
    u8g2.print(buff);
  }

  u8g2.sendBuffer();

  delay(LOOP_INTERVAL);
}