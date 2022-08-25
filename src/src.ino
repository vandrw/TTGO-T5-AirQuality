#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <AceButton.h>
#include <bsec.h>
#include <Wire.h>
#include "display.h"
#include "pages.h"

#define BUTTON_PIN 39
#define BOARD_LED 19
#define BME_SCL 22
#define BME_SDA 21

using namespace ace_button;

GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display(GxEPD2_213_B73(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEH0213B73
U8G2_FOR_ADAFRUIT_GFX u8g2;
AceButton button(BUTTON_PIN);
 
Bsec iaqSensor;

uint8_t lastSelect = 0;

// Save CO2 equivalent every 10s
float co2History[100] = {0};
uint16_t histIndex = 99;
bool buttonPressed = false;
unsigned long previousMillis = 0;
uint16_t saveInterval = 10000;

// Refresh the display every 30 minutes
unsigned  long prevRefresh = 0;
uint32_t displayInterval = 1800000;

String output;

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

    display.setRotation(1);
    display.setFullWindow();

    // SPI
    // bmeSpi.begin(BME_SCK, BME_SDO, BME_SDI, BME_CS);
    Wire.begin(BME_SDA, BME_SCL);

    // BME680 sensor
    // iaqSensor.begin(BME_CS, bmeSpi);
    iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
    bsec_virtual_sensor_t sensorList[7] = {
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE, // deg C
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,    // %
        BSEC_OUTPUT_RAW_PRESSURE,                        // hPa
        BSEC_OUTPUT_CO2_EQUIVALENT,                      // ppm
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,               // ppb
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STABILIZATION_STATUS,
    };

    iaqSensor.updateSubscription(sensorList, 7, BSEC_SAMPLE_RATE_LP);

    checkIaqSensorStatus();
    Serial.println(output);
    iaqSensor.run();

    displayPage(lastSelect);

    output = "Timestamp [ms], Temperature [°C], Relative humidity [%], pressure [Pa], IAQ, IAQ accuracy, CO2 Equivalent [ppm], Breath VOC Equivalent [ppb]";
    Serial.println(output);
}

void loop()
{
    unsigned long currentMillis = millis();

    if (iaqSensor.run()) {
      output = String(currentMillis);
      output += ", " + String(iaqSensor.temperature);
      output += ", " + String(iaqSensor.humidity);
      output += ", " + String(iaqSensor.pressure / 100);
      output += ", " + String(iaqSensor.iaq);
      output += ", " + String(iaqSensor.iaqAccuracy);
      output += ", " + String(iaqSensor.co2Equivalent);
      output += ", " + String(iaqSensor.breathVocEquivalent);
      Serial.println(output);

      if (currentMillis - previousMillis > saveInterval)
      {
          previousMillis = currentMillis;
          co2History[histIndex--] = iaqSensor.co2Equivalent;

          if (histIndex <= 0)
          {
              histIndex = 99;
          }
      }
    }

    if (currentMillis - prevRefresh > displayInterval)
    {
        prevRefresh = currentMillis;
        displayPage(lastSelect);
    }

    button.check();

    if (buttonPressed)
    {
        buttonPressed = false;
        displayPage(lastSelect);
    }
}

// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        triggerLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        triggerLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void triggerLeds(void)
{
  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, HIGH);
  delay(100);
  digitalWrite(BOARD_LED, LOW);
  delay(100);
}
