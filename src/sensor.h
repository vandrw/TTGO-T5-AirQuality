#ifndef SENSOR_H
#define SENSOR_H

#include <bsec.h>
#include "FS.h"
#include "SD.h"

#define BOARD_LED 19
#define STATE_SAVE_PERIOD UINT32_C(480 * 60 * 1000) // 480 minutes - 3 times a day

// Helper functions declarations
void checkIaqSensorStatus(void);
void triggerLeds(void);
void loadState(fs::FS &f);
void updateState(fs::FS &f);

extern Bsec iaqSensor;
extern RTC_DATA_ATTR uint8_t sensor_state[BSEC_MAX_STATE_BLOB_SIZE];
extern uint16_t stateUpdateCounter;
extern String output;

// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK)
  {
    if (iaqSensor.status < BSEC_OK)
    {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        triggerLeds(); /* Halt in case of failure */
    }
    else
    {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK)
  {
    if (iaqSensor.bme680Status < BME680_OK)
    {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        triggerLeds(); /* Halt in case of failure */
    }
    else
    {
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

int64_t GetTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

void loadState(fs::FS &f)
{
  File file = f.open("/state", FILE_READ);
  size_t byte_count = 0;

  if (file.read() == BSEC_MAX_STATE_BLOB_SIZE)
  {
    // Existing state in SD card
    Serial.println("Reading state from SD card");

    byte_count = file.read(sensor_state, BSEC_MAX_STATE_BLOB_SIZE);

    if (byte_count != BSEC_MAX_STATE_BLOB_SIZE)
    {
      Serial.println("Error reading state from SD card");
    }

    iaqSensor.setState(sensor_state);
    checkIaqSensorStatus();
    file.close();
  } else
  {
    Serial.println("No state found on the SD card.");
  }
}

void updateState(fs::FS &f)
{
  File file = f.open("/state", FILE_WRITE);
  size_t byte_count = 0;

  iaqSensor.getState(sensor_state);
  checkIaqSensorStatus();

  file.write(BSEC_MAX_STATE_BLOB_SIZE);
  byte_count = file.write(sensor_state, BSEC_MAX_STATE_BLOB_SIZE);

  if (byte_count != BSEC_MAX_STATE_BLOB_SIZE)
  {
    Serial.println("Error writing state to SD card");
  }

  file.close();
}

#endif // SENSOR_H