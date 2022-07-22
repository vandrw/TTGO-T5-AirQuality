#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <AceButton.h>
#include <bsec.h>
#include "display.h"
#include "pages.h"

#define BUTTON_PIN 39
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

using namespace ace_button;

GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display(GxEPD2_213_B73(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEH0213B73
U8G2_FOR_ADAFRUIT_GFX u8g2;
AceButton button(BUTTON_PIN);

Bsec iaqSensor; // (BME_CS, BME_MOSI, BME_MISO, BME_SCK);

uint8_t lastSelect = 0;

// Save CO2 equivalent every 10s
float co2History[100] = {0};
unsigned long previousMillis = 0;
uint16_t saveInterval = 10000;
uint16_t histIndex = 99;
bool buttonPressed = false;

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

    // BME680 sensor
    iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
    bsec_virtual_sensor_t sensorList[5] = {
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    };

    iaqSensor.updateSubscription(sensorList, 5, BSEC_SAMPLE_RATE_LP);

    displayPage(lastSelect);
}

void loop()
{
    if (!iaqSensor.run())
    { // If no data is available
        return;
    }

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis > saveInterval)
    {
        previousMillis = currentMillis;
        co2History[histIndex--] = iaqSensor.co2Equivalent;

        if (histIndex < 0)
        {
            histIndex = 99;
        }
    }

    button.check();

    if (buttonPressed) {
        buttonPressed = false;
        displayPage(lastSelect);
    }
}
