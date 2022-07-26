#ifndef PAGES_H
#define PAGES_H

#include <U8g2_for_Adafruit_GFX.h>
#include <bsec.h>
#include "enums.h"
#include "display.h"
#include "bitmaps.h"

void displayGlyph(const int glyph, uint16_t y, uint8_t align, int16_t xOffset /*= 0*/);
void displayText(const char *str, uint16_t y, uint8_t align, int16_t xOffset /*= 0*/);

extern GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2;
extern Bsec iaqSensor;

void displayInfo(tm timeinfo)
{
    // Display bitmap based on Air Quality Index
    if (iaqSensor.iaq < 100)
    {
        display.drawBitmap(15, 15, epd_bitmap_good, 100, 100, GxEPD_BLACK);
    }
    else if (iaqSensor.co2Equivalent < 150)
    {
        display.drawBitmap(15, 15, epd_bitmap_moderate, 100, 100, GxEPD_BLACK);
    }
    else
    {
        display.drawBitmap(15, 15, epd_bitmap_bad, 100, 100, GxEPD_BLACK);
    }

    // Display status of sensor
    String dispText = String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min);
    u8g2.setFont(u8g2_font_chikita_tr);
    switch (iaqSensor.staticIaqAccuracy)
    {
    case 0:
        dispText += " Recently started...";
        break;
    case 1:
        dispText += " Gathering samples...";
        break;
    case 2:
        dispText += " Calibrating...";
        break;
    case 3:
        // displayText("Accurate", display.height() - 3, GxEPD_ALIGN_LEFT, 3);
        break;
    }
    displayText(dispText.c_str(), display.height() - 3, GxEPD_ALIGN_LEFT, 3);

    // Store sensor measurements and display them, alongside glyphs
    String temp = String(String(iaqSensor.temperature) + '\xB0' + 'C'); // Temperature
    String humidity = String(String(iaqSensor.humidity) + '%');         // Humidity
    String pressure = String(String(iaqSensor.pressure / 100) + "Pa");  // Pressure
    String iaq = String(iaqSensor.staticIaq);                           // Air Quality Index
    String co2 = String(String(iaqSensor.co2Equivalent) + "ppm");       // CO2 Equivalent
    String voc = String(String(iaqSensor.breathVocEquivalent) + "ppb"); // VOC Equivalent

    u8g2.setFont(u8g2_font_unifont_t_weather);
    displayGlyph(0x031, 34, GxEPD_ALIGN_CENTER, -6); // Temperature
    displayGlyph(0x032, 49, GxEPD_ALIGN_CENTER, -6); // Humidty
    u8g2.setFont(u8g2_font_unifont_t_77);
    displayGlyph(0x26b5, 57, GxEPD_ALIGN_CENTER, -8); // Pressure
    u8g2.setFont(u8g2_font_prospero_bold_nbp_tr);
    displayText("AirQ", 75, GxEPD_ALIGN_CENTER, -4); // IAQ
    displayText("CO2", 91, GxEPD_ALIGN_CENTER, -7);  // CO2 Equivalent
    displayText("VOC", 106, GxEPD_ALIGN_CENTER, -7); // VOC Equivalent
    u8g2.setFont(u8g2_font_10x20_tf);
    displayText(temp.c_str(), 30, GxEPD_ALIGN_LEFT, 150);     // Temperature
    displayText(humidity.c_str(), 46, GxEPD_ALIGN_LEFT, 150); // Humidty
    displayText(pressure.c_str(), 60, GxEPD_ALIGN_LEFT, 150); // Pressure
    displayText(iaq.c_str(), 75, GxEPD_ALIGN_LEFT, 150);      // IAQ
    displayText(co2.c_str(), 90, GxEPD_ALIGN_LEFT, 150);      // CO2 Equivalent
    displayText(voc.c_str(), 105, GxEPD_ALIGN_LEFT, 150);     // VOC Equivalent
}

void displayGraph(float *history)
{
    const uint16_t tw = display.width();
    const uint16_t th = display.height();
    const uint16_t linesPad = 10;
    const uint16_t axisPad = 3;

    float maxVal = 1.00;
    for (int i = 0; i < 100; i++)
    {
        if (history[i] > maxVal)
        {
            maxVal = history[i];
        }
    }

    // Draw axes and labels
    display.drawFastHLine(linesPad, th - linesPad, tw - (2 * linesPad), GxEPD_BLACK); // Horizontal
    display.drawFastVLine(linesPad, linesPad, th - (2 * linesPad), GxEPD_BLACK);      // Vertical

    display.setRotation(0);
    u8g2.setFont(u8g2_font_chikita_tr);
    displayText("CO2e [ppm]", axisPad, GxEPD_ALIGN_CENTER, 0);

    display.setRotation(1);
    displayText(String(maxVal, 2).c_str(), linesPad + axisPad, GxEPD_ALIGN_LEFT, linesPad + axisPad);
    displayText("Time [min]", th - axisPad, GxEPD_ALIGN_CENTER, 0);
    displayText(String(100 * 10 / 60).c_str(), th - axisPad, GxEPD_ALIGN_RIGHT, axisPad);

    // Draw the CO2 history as a scatter plot
    // Origin is at (linesPad, th - linesPad)
    for (int i = 0; i < 100; i++)
    {
        uint16_t x = (i * (tw - (2 * linesPad)) / 100) + linesPad + 1;
        uint16_t y = th - linesPad - 1 - ((history[i] / maxVal) * (th - (2 * linesPad)));
        display.drawPixel(x, y, GxEPD_BLACK);
    }
}

#endif /* PAGES_H */
