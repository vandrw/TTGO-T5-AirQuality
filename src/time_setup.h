#ifndef TIME_SETUP_H
#define TIME_SETUP_H

#include <WiFi.h>

const char *ssid = "YOUR_SSD";
const char *password = "YOUR_PASSWORD";

const char *ntpServer1 = "europe.pool.ntp.org";
const char *ntpServer2 = "nl.pool.ntp.org";
const char *ntpServer3 = "pool.ntp.org";
const char *location = "CET-1CEST,M3.5.0,M10.5.0/3";

void synchronizeClock(void)
{
    configTzTime(location, ntpServer1, ntpServer2, ntpServer3);

    // Serial.println("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        // Serial.print(".");
    }
    // Serial.println("WiFi connected");
    // Serial.println("IP address: ");
    // Serial.println(WiFi.localIP());
}

#endif // TIME_SETUP_H