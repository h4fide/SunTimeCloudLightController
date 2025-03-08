#ifndef CONFIG_H
#define CONFIG_H

//================ NETWORK CONFIGURATION ================
const char *WIFI_SSID = "YOUR_SSID";
const char *WIFI_PASSWORD = "YOUR_PASSWORD";

const char *NTP_SERVER = "time.google.com"; // Using more reliable NTP server Than NTP_POOL

//================ CLOUD COVERAGE CONFIGURATION ================
const int CLOUD_COVERAGE_THRESHOLD = 78;
const int CLOUD_COVERAGE_HYSTERESIS = 5;

const char* API_HOST = "api.open-meteo.com";
const int HTTP_PORT = 80;
const int EEPROM_SIZE = 512;
const unsigned long WIFI_TIMEOUT = 10000;
const int BUFFER_SIZE = 6144;

//================ PIN DEFINITIONS ================
const int RELAY_PIN = 12;        // D6 - Main light relay
const int ERROR_LED_PIN = 14;   // D5 - Error indicator
const int STATUS_LED_PIN = 13;  // D7 - Status indicator

//================ TIME CONFIGURATION ================
const int TIME_RISE_OFFSET_MINUTES = 0;
const int TIME_SET_OFFSET_MINUTES = 0;

const int TIMEZONE_OFFSET = 0; // Update this with your timezone offset in seconds

const int TIME_OFFSET_MONITORING = 30; // Minutes before sunrise/sunset to check clouds
const int MAX_MONITORING_RETRIES = 3;
const int RETRY_DELAY_MS = 300000; // 5 minutes between retries
const int DAYLIGHT_OFFSET = 0; // Daylight saving time offset in seconds 3600 for 1 hour

//================ LOCATION CONFIGURATION ================
const float LATITUDE = 0;  // Update with your actual latitude
const float LONGITUDE = 0; // Update with your actual longitude

//================ TIMING INTERVALS ================
const unsigned long SYNC_INTERVAL = 1 * 60 * 60 * 1000; // 1 hour in milliseconds

#endif