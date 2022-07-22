#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <AceButton.h>

using namespace ace_button;

enum
{
    INFO_PAGE,
    GRAPH_PAGE,
    PAGE_MAX,
};

enum
{
    GxEPD_ALIGN_RIGHT,
    GxEPD_ALIGN_LEFT,
    GxEPD_ALIGN_CENTER,
};

extern GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display;
extern U8G2_FOR_ADAFRUIT_GFX u8g2;
extern AceButton button;
extern uint8_t lastSelect;

static void displayText(const char *str, uint16_t y, uint8_t align, int16_t xOffset = 0)
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

static void displayGlyph(const int glyph, uint16_t y, uint8_t align, int16_t xOffset = 0)
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

static void displayInfoPage(uint8_t num)
{
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);
        switch (num)
        {
        case INFO_PAGE:
            u8g2.setFont(u8g2_font_unifont_t_weather);
            displayGlyph(0x031, 45, GxEPD_ALIGN_RIGHT, 25); // Temperature
            displayGlyph(0x032, 85, GxEPD_ALIGN_RIGHT, 25); // Humidty
            u8g2.setFont(u8g2_font_unifont_t_77);
            displayGlyph(0x26b5, 57, GxEPD_ALIGN_RIGHT, 21); // Pressure
            displayGlyph(0x26c6, 98, GxEPD_ALIGN_RIGHT, 25); // VOC gas
            u8g2.setFont(u8g2_font_10x20_tf);
            displayText("30\xB0", 40, GxEPD_ALIGN_RIGHT, 30);        // Temperature
            displayText("1023.46 hPa", 60, GxEPD_ALIGN_RIGHT, 30);   // Pressure
            displayText("34.56%", 80, GxEPD_ALIGN_RIGHT, 30);        // Humidty
            displayText("158.42 KOhms", 100, GxEPD_ALIGN_RIGHT, 30); // VOC gas
            break;
        case GRAPH_PAGE:
            // drawBitmap(display, DEFALUT_QR_CODE_BMP, 10, 10, true);
            displayText("TODO Graph", display.height() / 2, GxEPD_ALIGN_CENTER);
            break;
        default:
            break;
        }
    } while (display.nextPage());
}

void handleEvent(AceButton * /*button*/, uint8_t eventType, uint8_t /*buttonState*/)
{
    if (eventType == AceButton::kEventPressed)
        displayInfoPage(lastSelect++);
    lastSelect %= PAGE_MAX;
}

#endif /* DISPLAY_H */
