
#ifndef CONFIG_H
#define CONFIG_H

//================ NETWORK CONFIGURATION ================
const char *WIFI_SSID = "YOUR_SSID";
const char *WIFI_PASSWORD = "YOUR_PASSWORD";

const char *HTTP_USERNAME = "admin";
const char *HTTP_PASSWORD = "admin";
const char *NTP_SERVER = "time.google.com";

//================ CLOUD COVERAGE CONFIGURATION ================
const int CLOUD_COVERAGE_THRESHOLD = 78;
const int CLOUD_COVERAGE_HYSTERESIS = 5;

const char* API_HOST = "api.open-meteo.com";
const int HTTP_PORT = 80;
const int EEPROM_SIZE = 512;
const unsigned long WIFI_TIMEOUT = 10000;
const int BUFFER_SIZE = 6144;

//================ PIN DEFINITIONS ================
const int RELAY_PIN = 2;        // D4 - Main light relay
const int ERROR_LED_PIN = 14;   // D5 - Error indicator
const int STATUS_LED_PIN = 13;  // D7 - Status indicator

//================ TIME CONFIGURATION ================
const int TIME_RISE_OFFSET_MINUTES = 0;
const int TIME_SET_OFFSET_MINUTES = 0;

const int TIMEZONE_OFFSET = 0;

const int TIME_OFFSET_MONITORING = 30;
const int MAX_MONITORING_RETRIES = 3;
const int RETRY_DELAY_MS = 300000;
const int DAYLIGHT_OFFSET = 3600;

//================ LOCATION CONFIGURATION ================
const float LATITUDE = 0;
const float LONGITUDE = 0;

//================ TIMING INTERVALS ================
const unsigned long SYNC_INTERVAL = 24 * 60 * 60 * 1000; // 24 hours in milliseconds

#endif