#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "enums.h"
#include "pages.h"

extern GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2;

void displayText(const char *str, uint16_t y, uint8_t align, int16_t xOffset = 0)
{
    int16_t tw = u8g2.getUTF8Width(str); // text box width
    int16_t ta = u8g2.getFontAscent();   // positive
    int16_t td = u8g2.getFontDescent();  // negative; in mathematicians view
    int16_t th = ta - td;                // text box height
    y = y - th / 2 + ta;

    u8g2.setCursor(0, y);
    switch (align)
    {
    case GxEPD_ALIGN_RIGHT:
        u8g2.setCursor(display.width() - tw - xOffset, y);
        break;
    case GxEPD_ALIGN_LEFT:
        u8g2.setCursor(xOffset, y);
        break;
    case GxEPD_ALIGN_CENTER:
        u8g2.setCursor(display.width() / 2 - (tw / 2) - xOffset, y);
        break;
    default:
        break;
    }
    u8g2.print(str);
}

void displayGlyph(const int glyph, uint16_t y, uint8_t align, int16_t xOffset = 0)
{
    int16_t ta = u8g2.getFontAscent();  // positive
    int16_t td = u8g2.getFontDescent(); // negative; in mathematicians view
    int16_t th = ta - td;               // text box height
    y = y - th / 2 + ta;

    switch (align)
    {
    case GxEPD_ALIGN_RIGHT:
        u8g2.drawGlyph(display.width() - xOffset, y, glyph);
        break;
    case GxEPD_ALIGN_LEFT:
        u8g2.drawGlyph(xOffset, y, glyph);
        break;
    case GxEPD_ALIGN_CENTER:
        u8g2.drawGlyph(display.width() / 2 - xOffset, y, glyph);
        break;
    default:
        break;
    }
}

void displayPage(tm timeinfo)
{
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);
        displayInfo(timeinfo);
    } while (display.nextPage());
}

#endif /* DISPLAY_H */
