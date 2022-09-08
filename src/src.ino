#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <bsec.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "time_setup.h"
#include "display.h"
#include "pages.h"
#include "sensor.h"

#define BME_SCL 22
#define BME_SDA 21

GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display(GxEPD2_213_B73(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEH0213B73
U8G2_FOR_ADAFRUIT_GFX u8g2;
SPIClass *cardSPI = NULL;

// Create an object of the class Bsec
Bsec iaqSensor;
RTC_DATA_ATTR uint8_t sensor_state[BSEC_MAX_STATE_BLOB_SIZE] = {0};

// Refresh the display every 60 minutes
uint32_t displayInterval = 3600000;

String output;

void setup()
{

  // E-paper display
  display.init(115200);
  u8g2.begin(display);                  // connect u8g2 procedures to Adafruit GFX
  u8g2.setFontMode(1);                  // use u8g2 transparent mode (this is default)
  u8g2.setFontDirection(0);             // left to right (this is default)
  u8g2.setForegroundColor(GxEPD_BLACK); // apply Adafruit GFX color
  u8g2.setBackgroundColor(GxEPD_WHITE); // apply Adafruit GFX color

  display.setRotation(1);
  display.setFullWindow();

  // I2C
  Wire.begin(BME_SDA, BME_SCL);

  // WiFi and Time
  synchronizeClock();

  // SD Card
  cardSPI = new SPIClass(HSPI);
  cardSPI->begin(14, 2, 15, 13);

  if (!SD.begin(13, *cardSPI))
  {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  // BME680 sensor
  iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);

  bsec_virtual_sensor_t sensorList[10] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,                     // deg C
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE, // deg C
      BSEC_OUTPUT_RAW_HUMIDITY,                        // %
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,    // %
      BSEC_OUTPUT_RAW_PRESSURE,                        // hPa
      BSEC_OUTPUT_RAW_GAS,                             // Ohms
      BSEC_OUTPUT_CO2_EQUIVALENT,                      // ppm
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,               // ppb
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_ULP);
  checkIaqSensorStatus();
  Serial.println(output);

  loadState(SD);
}

void loop()
{
  int64_t timestamp = GetTimestamp();
  struct tm tinf;

  if (!getLocalTime(&tinf))
  {
    Serial.println("No time available yet");
    return;
  }

  if (iaqSensor.run(timestamp))
  {
    // Time, Temperature [°C], Raw temperature [°C], Relative humidity [%], Raw humidity [%],
    // Pressure [Pa], IAQ, IAQ accuracy, Static IAQ, Static IAQ accuracy, CO2 Equivalent [ppm],
    // Breath VOC Equivalent [ppb], Gas resistance [Ohms]
    output = String(tinf.tm_mday) + "/" + String(tinf.tm_mon + 1) + "/" + String(tinf.tm_year + 1900);
    output += " " + String(tinf.tm_hour) + ":" + String(tinf.tm_min) + ":";
    if (tinf.tm_sec < 10)
    {
      output += "0";
    }
    output += String(tinf.tm_sec);
    output += "," + String(iaqSensor.temperature);
    output += "," + String(iaqSensor.rawTemperature);
    output += "," + String(iaqSensor.humidity);
    output += "," + String(iaqSensor.rawHumidity);
    output += "," + String(iaqSensor.pressure / 100);
    output += "," + String(iaqSensor.iaq);
    output += "," + String(iaqSensor.iaqAccuracy);
    output += "," + String(iaqSensor.staticIaq);
    output += "," + String(iaqSensor.staticIaqAccuracy);
    output += "," + String(iaqSensor.co2Equivalent);
    output += "," + String(iaqSensor.breathVocEquivalent);
    output += "," + String(iaqSensor.gasResistance);
    appendRow(SD, output.c_str());
    updateState(SD);

    // Serial.println(output);
  }

  if (timestamp % displayInterval < 60000)
  {
    // Serial.println("Refreshing display");
    displayPage(tinf);
  }

  uint64_t time_us = ((iaqSensor.nextCall - timestamp) * 1000) - esp_timer_get_time();
  // Serial.printf("Deep sleep for %llu ms. BSEC next call at %llu ms.", time_us / 1000, iaqSensor.nextCall);
  esp_sleep_enable_timer_wakeup(time_us);
  esp_deep_sleep_start();
}
