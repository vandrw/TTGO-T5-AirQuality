#define LILYGO_T5_V213

#include <boards.h>
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <AceButton.h>
#include "display.h"

const int BUTTON_PIN = 39;

using namespace ace_button;

GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display(GxEPD2_213_B73(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEH0213B73
U8G2_FOR_ADAFRUIT_GFX u8g2;
AceButton button(BUTTON_PIN);

uint8_t lastSelect = 0;

void setup()
{
    // Incorporated button
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    button.setEventHandler(handleEvent);

    // E-paper display
    display.init(115200);
    u8g2.begin(display);                  // connect u8g2 procedures to Adafruit GFX
    u8g2.setFontMode(1);                  // use u8g2 transparent mode (this is default)
    u8g2.setFontDirection(0);             // left to right (this is default)
    u8g2.setForegroundColor(GxEPD_BLACK); // apply Adafruit GFX color
    u8g2.setBackgroundColor(GxEPD_WHITE); // apply Adafruit GFX color
    u8g2.setFont(u8g2_font_10x20_tf);     // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

    display.setRotation(1);
    display.setFullWindow();

    displayInfoPage(lastSelect++);
}

void loop()
{
    button.check();
}
