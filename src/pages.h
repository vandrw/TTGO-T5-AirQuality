#ifndef PAGES_H
#define PAGES_H

#include <U8g2_for_Adafruit_GFX.h>
#include <bsec.h>
#include "enums.h"
#include "display.h"

void displayGlyph(const int glyph, uint16_t y, uint8_t align, int16_t xOffset /*= 0*/);
void displayText(const char *str, uint16_t y, uint8_t align, int16_t xOffset /*= 0*/);

extern GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2;
extern Bsec iaqSensor;

void displayInfo(void)
{
    String temp = String(String(30.55) + "\xB0");     // iaqSensor.temperature
    String humidity = String(String(45.65) + "%");    // iaqSensor.humidity
    String pressure = String(String(10.65) + " hPa"); // iaqSensor.pressure
    String iaq = String(45.65);                       // iaqSensor.iaq
    String co2 = String(500);                         // iaqSensor.co2Equivalent
    String voc = String(0.50);                        // iaqSensor.breathVocEquivalent

    u8g2.setFont(u8g2_font_unifont_t_weather);
    displayGlyph(0x031, 34, GxEPD_ALIGN_RIGHT, 25); // Temperature
    displayGlyph(0x032, 49, GxEPD_ALIGN_RIGHT, 25); // Humidty
    u8g2.setFont(u8g2_font_unifont_t_77);
    displayGlyph(0x26b5, 57, GxEPD_ALIGN_RIGHT, 21); // Pressure
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tr);
    displayText("AirQ", 75, GxEPD_ALIGN_RIGHT, 5); // IAQ
    displayText("CO2", 91, GxEPD_ALIGN_RIGHT, 7);  // CO2 Equivalent
    displayText("VOC", 106, GxEPD_ALIGN_RIGHT, 7); // VOC Equivalent
    u8g2.setFont(u8g2_font_10x20_tf);
    displayText(temp.c_str(), 30, GxEPD_ALIGN_RIGHT, 30);     // Temperature
    displayText(humidity.c_str(), 46, GxEPD_ALIGN_RIGHT, 30); // Humidty
    displayText(pressure.c_str(), 60, GxEPD_ALIGN_RIGHT, 30); // Pressure
    displayText(iaq.c_str(), 75, GxEPD_ALIGN_RIGHT, 30);      // IAQ
    displayText(co2.c_str(), 90, GxEPD_ALIGN_RIGHT, 30);      // CO2 Equivalent
    displayText(voc.c_str(), 105, GxEPD_ALIGN_RIGHT, 30);     // VOC Equivalent
}

void displayGraph(void)
{
    const uint16_t tw = display.width();
    const uint16_t th = display.height();
    const uint16_t pad = 5;
    display.drawFastHLine(pad, th - pad, tw - pad, GxEPD_BLACK); // Horizontal
    display.drawFastVLine(pad, pad, th - pad, GxEPD_BLACK);      // Vertical
    displayText("TODO Graph", display.height() / 2, GxEPD_ALIGN_CENTER, 0);
}

#endif /* PAGES_H */