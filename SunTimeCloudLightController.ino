//================ LIBRARY IMPORTS ================
#include "config.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <ArduinoOTA.h> 
#include <sunset.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include "index_html.h"

//================ GLOBAL VARIABLES ================
float currentCloudCoverage = -1;
bool isMonitoring = false;
bool cloudTriggeredActivation = false;
String cloudStatus = "Not monitoring";

bool manualOverride = false;
unsigned long manualOverrideStartTime = 0;
int manualOverrideDuration = 60; // Duration in minutes
bool manualLightState = false;

char buffer[BUFFER_SIZE]; 

unsigned long lastTimeSync = 0;
bool wifiEnabled = true;

int TIME_RISE_OFFSET_ADDITIONAL = 0;  
int TIME_SET_OFFSET_ADDITIONAL = 0;   

//================ MONITORING FLAGS ================
bool monitoring_sunrise = true;
bool monitoring_sunset = true;
int monitoring_retry_count = 0;

int relayOn = LOW;
int relayOff = HIGH; 

char http_username[32] = "admin";
char http_password[32] = "admin";

//================ CONFIGURATION VARIABLES ================
int cloudThreshold = CLOUD_COVERAGE_THRESHOLD;
int cloudHysteresis = CLOUD_COVERAGE_HYSTERESIS;
int monitoringWindow = TIME_OFFSET_MONITORING;
int sunriseOffset = TIME_RISE_OFFSET_MINUTES;
int sunsetOffset = TIME_SET_OFFSET_MINUTES;
int timezoneOffsetSec = TIMEZONE_OFFSET;
int daylightOffsetSec = DAYLIGHT_OFFSET;
float locationLatitude = LATITUDE;
float locationLongitude = LONGITUDE;
char adminUsername[32] = "admin";
char adminPassword[32] = "admin";

//================ GLOBAL VARIABLES ================
int sunriseHour, sunsetHour;
int sunriseMinute, sunsetMinute;
int localtime_h, localtime_m;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, TIMEZONE_OFFSET + DAYLIGHT_OFFSET);
ESP8266WebServer server(80);
SunSet sun;

char deviceName[32] = "LightController"; // Device name for OTA and display
int cloudHysteresis = CLOUD_COVERAGE_HYSTERESIS; // Hysteresis for cloud coverage
int maxRetries = MAX_MONITORING_RETRIES;
int timezoneOffsetSec = TIMEZONE_OFFSET; 
int daylightOffsetSec = DAYLIGHT_OFFSET;

//================ SYSTEM STATE ENUM ================
enum SystemState {
    NORMAL,
    MONITORING,
    ACTIVE,
    SCHEDULED,
    MANUAL     // New state for manual override
};

SystemState currentState = NORMAL;

//================ EEPROM FUNCTIONS ================
void saveSettings() {
  EEPROM.begin(512);
  
  // Save all time settings
  EEPROM.put(0, sunriseHour);
  EEPROM.put(sizeof(int), sunriseMinute);
  EEPROM.put(2 * sizeof(int), sunsetHour);
  EEPROM.put(3 * sizeof(int), sunsetMinute);
  
  // Save basic configuration
  int addr = 4 * sizeof(int);
  EEPROM.put(addr, cloudThreshold); addr += sizeof(int);
  EEPROM.put(addr, monitoringWindow); addr += sizeof(int);
  EEPROM.put(addr, manualOverrideDuration); addr += sizeof(int);
  
  // Save advanced configuration
  EEPROM.put(addr, cloudHysteresis); addr += sizeof(int);
  EEPROM.put(addr, maxRetries); addr += sizeof(int);
  EEPROM.put(addr, timezoneOffsetSec); addr += sizeof(int);
  EEPROM.put(addr, daylightOffsetSec); addr += sizeof(int);
  EEPROM.put(addr, relayOn == HIGH ? 1 : 0); addr += sizeof(int);
  EEPROM.put(addr, sunriseOffset); addr += sizeof(int);
  EEPROM.put(addr, sunsetOffset); addr += sizeof(int);
  
  // Save location data
  EEPROM.put(addr, locationLatitude); addr += sizeof(float);
  EEPROM.put(addr, locationLongitude); addr += sizeof(float);
  
  // Save username and password - limit length for safety
  for (int i = 0; i < 32; i++) {
    EEPROM.write(addr++, adminUsername[i]);
    if (adminUsername[i] == 0) break;
  }
  
  for (int i = 0; i < 32; i++) {
    EEPROM.write(addr++, adminPassword[i]);
    if (adminPassword[i] == 0) break;
  }
  
  // Save device name
  for (int i = 0; i < 32; i++) {
    EEPROM.write(addr++, deviceName[i]);
    if (deviceName[i] == 0) break;
  }
  
  bool success = EEPROM.commit();
  EEPROM.end();
  
  Serial.printf("EEPROM save %s\n", success ? "successful" : "failed");
}


void loadSettings() {
  EEPROM.begin(512);
  
  // Read settings with validation
  EEPROM.get(0, sunriseHour);
  EEPROM.get(sizeof(int), sunriseMinute);
  EEPROM.get(2 * sizeof(int), sunsetHour);
  EEPROM.get(3 * sizeof(int), sunsetMinute);
  
  // Read basic configuration
  int addr = 4 * sizeof(int);
  EEPROM.get(addr, cloudThreshold); addr += sizeof(int);
  EEPROM.get(addr, monitoringWindow); addr += sizeof(int);
  EEPROM.get(addr, manualOverrideDuration); addr += sizeof(int);
  
  // Read advanced configuration
  EEPROM.get(addr, cloudHysteresis); addr += sizeof(int);
  EEPROM.get(addr, maxRetries); addr += sizeof(int);
  EEPROM.get(addr, timezoneOffsetSec); addr += sizeof(int);
  EEPROM.get(addr, daylightOffsetSec); addr += sizeof(int);
  
  int relayLogic;
  EEPROM.get(addr, relayLogic); addr += sizeof(int);
  relayOn = relayLogic == 1 ? HIGH : LOW;
  relayOff = relayLogic == 1 ? LOW : HIGH;
  
  EEPROM.get(addr, sunriseOffset); addr += sizeof(int);
  EEPROM.get(addr, sunsetOffset); addr += sizeof(int);
  
  // Read location data
  EEPROM.get(addr, locationLatitude); addr += sizeof(float);
  EEPROM.get(addr, locationLongitude); addr += sizeof(float);
  
  // Read username and password - limit length for safety
  int i;
  for (i = 0; i < 31; i++) {
    char c = EEPROM.read(addr++);
    adminUsername[i] = c;
    if (c == 0) break;
  }
  adminUsername[i] = 0; // Ensure null termination
  
  for (i = 0; i < 31; i++) {
    char c = EEPROM.read(addr++);
    adminPassword[i] = c;
    if (c == 0) break;
  }
  adminPassword[i] = 0; // Ensure null termination
  
  // Read device name
  for (i = 0; i < 31; i++) {
    char c = EEPROM.read(addr++);
    deviceName[i] = c;
    if (c == 0) break;
  }
  deviceName[i] = 0; // Ensure null termination
  
  // Validate values
  sunriseHour = constrain(sunriseHour, 0, 23);
  sunriseMinute = constrain(sunriseMinute, 0, 59);
  sunsetHour = constrain(sunsetHour, 0, 23);
  sunsetMinute = constrain(sunsetMinute, 0, 59);
  
  cloudThreshold = constrain(cloudThreshold, 0, 100);
  cloudHysteresis = constrain(cloudHysteresis, 0, 20);
  monitoringWindow = constrain(monitoringWindow, 5, 120);
  manualOverrideDuration = constrain(manualOverrideDuration, 5, 1440);
  
  maxRetries = constrain(maxRetries, 1, 10);
  sunriseOffset = constrain(sunriseOffset, -120, 120);  
  sunsetOffset = constrain(sunsetOffset, -120, 120);
  
  // If location values seem wrong, use defaults
  if (isnan(locationLatitude) || locationLatitude < -90 || locationLatitude > 90) {
    locationLatitude = LATITUDE;
  }
  
  if (isnan(locationLongitude) || locationLongitude < -180 || locationLongitude > 180) {
    locationLongitude = LONGITUDE;
  }
  
  // If device name is empty, set default
  if (strlen(deviceName) == 0) {
    strcpy(deviceName, "LightController");
  }
  
  // Copy values to http variables (fixing the pointer issue)
  strcpy(http_username, adminUsername);
  strcpy(http_password, adminPassword);
  
  EEPROM.end();
  
  Serial.println("Loaded settings:");
  Serial.printf("Sunrise: %d:%d (offset %d min)\n", sunriseHour, sunriseMinute, sunriseOffset);
  Serial.printf("Sunset: %d:%d (offset %d min)\n", sunsetHour, sunsetMinute, sunsetOffset);
  Serial.printf("Cloud Threshold: %d%% with %d%% hysteresis\n", cloudThreshold, cloudHysteresis);
  Serial.printf("Monitoring Window: %d mins\n", monitoringWindow);
  Serial.printf("Location: %f, %f\n", locationLatitude, locationLongitude);
  Serial.printf("Time settings: TZ=%d, DST=%d\n", timezoneOffsetSec, daylightOffsetSec);
  Serial.printf("Relay Logic: %s\n", relayOn == HIGH ? "Active HIGH" : "Active LOW");
  Serial.printf("Auth credentials: %s / %s\n", http_username, http_password);
  Serial.printf("Device Name: %s\n", deviceName);
}


//================ WIFI FUNCTIONS ================
void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    digitalWrite(ERROR_LED_PIN, HIGH);
    delay(100);
    digitalWrite(ERROR_LED_PIN, LOW);
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(ERROR_LED_PIN, LOW);
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void enableWiFi() {
  if (!wifiEnabled) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    wifiEnabled = true;
    lastTimeSync = 0; // Force time sync
    Serial.println("WiFi enabled");
  }
}


//================ TIME FUNCTIONS ================
void updateLocalTime() {
    unsigned long startTime = millis();
    while (!timeClient.update()) {
        ESP.wdtFeed();
        if (millis() - startTime > 10000) break;
        delay(100);
    }
  
  Serial.print("Current time (GMT): ");
  Serial.println(timeClient.getFormattedTime());
  setTime(timeClient.getEpochTime());
  digitalWrite(ERROR_LED_PIN, LOW);
}

void setTime_lf(int hours, int minutes) {
  time_t t = now();
  tmElements_t tm;
  breakTime(t, tm);
  
  tm.Hour = hours;
  tm.Minute = minutes;
  tm.Second = 0;
  
  time_t newTime = makeTime(tm);
  setTime(newTime);
}

//================ SYSTEM STATE STRING HELPER ================
String getSystemStateString() {
    String stateStr = "";
    // Calculate time values before switch statement
    int currentTime = hour() * 60 + minute();
    int sunriseTime = sunriseHour * 60 + sunriseMinute;
    int sunsetTime = sunsetHour * 60 + sunsetMinute;
    
    switch (currentState) {
        case MONITORING: {
            stateStr = "Monitoring clouds (" + 
                      String(TIME_OFFSET_MONITORING) + " min before ";
            // Check if monitoring before sunrise or sunset
            if (currentTime < sunriseTime) {
                stateStr += "sunrise)";
            } else {
                stateStr += "sunset)";
            }
            break;
        }
        case ACTIVE: {
            stateStr = "Triggered";
            break;
        }
        case SCHEDULED: {
            stateStr = "Scheduled ";
            break;
        }
        case MANUAL: {
            // Calculate remaining override time
            int remainingMinutes = manualOverrideDuration - ((millis() - manualOverrideStartTime) / 60000);
            stateStr = "Manual override (" + String(remainingMinutes) + " min remaining)";
            break;
        }
        case NORMAL: {
            stateStr = "Normal";
            break;
        }
        default: {
            stateStr = "Normal Operation";
        }
    }
    return stateStr;
}

//================ MONITORING WINDOW CHECK ================
bool isWithinMonitoringWindow(int currentHour, int currentMinute) {
    int currentTime = currentHour * 60 + currentMinute;
    int sunriseTime = sunriseHour * 60 + sunriseMinute;
    int sunsetTime = sunsetHour * 60 + sunsetMinute;
    
    bool beforeSunrise = (currentTime >= (sunriseTime - monitoringWindow) && 
                         currentTime < sunriseTime);
    bool beforeSunset = (currentTime >= (sunsetTime - monitoringWindow) && 
                        currentTime < sunsetTime);
    
    isMonitoring = (beforeSunrise || beforeSunset);
    
    return isMonitoring;
}

String getFormattedTimeForAPI() {
    time_t now = timeClient.getEpochTime();
    char buffer[25];
    
    // Format: YYYY-MM-DDThh:00
    sprintf(buffer, "%04d-%02d-%02dT%02d:00", 
            year(now),
            month(now),
            day(now),
            hour(now));
            
    return String(buffer);
}

float getCloudCoverage() {

  if (WiFi.status() != WL_CONNECTED) {
    cloudStatus = "WiFi disconnected";
    return -1;
  }

  WiFiClient client;
  HTTPClient http;

  String timeStr = getFormattedTimeForAPI();
  String url = String("http://") + API_HOST + "/v1/forecast?latitude=" +
               String(LATITUDE, 4) + "&longitude=" + String(LONGITUDE, 4) +
               "&hourly=cloud_cover";

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    const size_t capacity = 500;
    DynamicJsonDocument doc(capacity);

    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      JsonArray timeArray = doc["hourly"]["time"];
      JsonArray cloudCoverArray = doc["hourly"]["cloud_cover"];

      int indexMatch = -1;
      for (int i = 0; i < timeArray.size(); i++) {
        if (timeArray[i].as<String>() == timeStr) {
          indexMatch = i;
          break;
        }
      }

      if (indexMatch != -1) {
        float cloudCover = cloudCoverArray[indexMatch];
        http.end();
        return cloudCover;
      }
    } else {
      Serial.println("JSON deserialization error");
    }
  } else {
    Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  cloudStatus = "Error fetching data";
  return -1;
}

//================ MODIFIED CLOUD MONITORING FUNCTION ================
bool monitorCloudConditions(bool isSunrise) {
    if (monitoring_retry_count >= maxRetries) {
        monitoring_retry_count = 0;
        cloudStatus = "Max retries reached";
        
        if (isSunrise) {
            monitoring_sunrise = false;
            TIME_RISE_OFFSET_ADDITIONAL = 0;
        } else {
            monitoring_sunset = false;
            TIME_SET_OFFSET_ADDITIONAL = 0;
        }
        
        return false; // Don't activate lights on retry failure
    }
    
    float cloudCoverage = getCloudCoverage();
    currentCloudCoverage = cloudCoverage; // Update global variable
    
    if (cloudCoverage < 0) {
        cloudStatus = "Error fetching data";
        monitoring_retry_count++;
        delay(1000); // Short delay before returning to main loop
        return false; // Don't activate yet, will retry later
    }
    
    monitoring_retry_count = 0;
    cloudStatus = String(cloudCoverage) + "% cloud coverage";
    
    // Use hysteresis for more stable operation
    static bool wasAboveThreshold = false;
    bool isAboveThreshold = cloudCoverage > cloudThreshold;
    bool shouldActivate;
    
    if (isAboveThreshold && !wasAboveThreshold) {
        shouldActivate = true;
        wasAboveThreshold = true;
    } else if (!isAboveThreshold && wasAboveThreshold) {
        // Only deactivate if we drop below threshold minus hysteresis
        shouldActivate = !(cloudCoverage < (cloudThreshold - cloudHysteresis));
        if (!shouldActivate) wasAboveThreshold = false;
    } else {
        shouldActivate = wasAboveThreshold;
    }
    
    if (shouldActivate) {
        cloudStatus = "Activating (Cloud coverage: " + String(cloudCoverage) + "%)";
        cloudTriggeredActivation = true;
        currentState = ACTIVE;
    } else {
        cloudStatus = "Normal (Cloud coverage: " + String(cloudCoverage) + "%)";
    }
    
    if (isSunrise) {
        monitoring_sunrise = !shouldActivate;
        if (shouldActivate) TIME_RISE_OFFSET_ADDITIONAL += 30;
        else TIME_RISE_OFFSET_ADDITIONAL = 0;
    } else {
        monitoring_sunset = !shouldActivate;
        if (shouldActivate) TIME_SET_OFFSET_ADDITIONAL = 0;
        else TIME_SET_OFFSET_ADDITIONAL = 0;
    }
    
    return shouldActivate;
}

//================ LIGHT CONTROL FUNCTION ================
bool shouldActivateLights(int currentHour, int currentMinute) {
    int currentTime = currentHour * 60 + currentMinute;
    int sunriseTime = sunriseHour * 60 + sunriseMinute;
    int sunsetTime = sunsetHour * 60 + sunsetMinute;
    
    if (monitoring_sunrise && currentTime >= (sunriseTime - monitoringWindow)) {
        return monitorCloudConditions(true);
    }
    
    if (monitoring_sunset && currentTime >= (sunsetTime - monitoringWindow)) {
        return monitorCloudConditions(false);
    }
    
    bool isDuringNight = (currentTime < sunriseTime || currentTime >= sunsetTime);
    if (isDuringNight) {
        currentState = SCHEDULED;
        cloudStatus = "Scheduled";
    }
    
    return isDuringNight;
}




//================ MODIFIED TIME FORMATTING FUNCTION ================
String formatTime(double minutesFromMidnight, bool isSunrise) {
    int hours = int(minutesFromMidnight / 60);
    int minutes = int(minutesFromMidnight) % 60;
    
    // Use the configurable offsets
    int timeOffset = isSunrise ? sunriseOffset : sunsetOffset;
    int additionalOffset = isSunrise ? TIME_RISE_OFFSET_ADDITIONAL : TIME_SET_OFFSET_ADDITIONAL;
    
    // Apply total offset at once for more accuracy
    int totalOffset = timeOffset + additionalOffset;
    int totalMinutes = (hours * 60 + minutes) - totalOffset;
    
    if (totalMinutes < 0) {
        totalMinutes += 24 * 60; // Add a full day
    }
    
    hours = (totalMinutes / 60) % 24;
    minutes = totalMinutes % 60;
    
    char timeStr[6];
    sprintf(timeStr, "%d:%02d", hours, minutes);
    return String(timeStr);
}

void updateSunriseSunsetTime() {
    sun.setCurrentDate(year(), month(), day());
    sun.setTZOffset((timezoneOffsetSec + daylightOffsetSec) / 3600.0); // Convert seconds to hours
    sun.setPosition(locationLatitude, locationLongitude, (timezoneOffsetSec + daylightOffsetSec) / 3600.0);
    
    double sunrise = sun.calcSunrise();
    double sunset = sun.calcSunset();
    
    String sunriseTime = formatTime(sunrise, true);
    String sunsetTime = formatTime(sunset, false);
    
    int colonIndex = sunriseTime.indexOf(':');
    if (colonIndex > 0) {
        sunriseHour = sunriseTime.substring(0, colonIndex).toInt();
        sunriseMinute = sunriseTime.substring(colonIndex + 1).toInt();
    }
    
    colonIndex = sunsetTime.indexOf(':');
    if (colonIndex > 0) {
        sunsetHour = sunsetTime.substring(0, colonIndex).toInt();
        sunsetMinute = sunsetTime.substring(colonIndex + 1).toInt();
    }
    
    Serial.printf("Updated sun times - Sunrise: %02d:%02d, Sunset: %02d:%02d\n", 
                 sunriseHour, sunriseMinute, sunsetHour, sunsetMinute);
    
    saveSettings();
}


//================ LIGHT CONTROL FUNCTIONS ================
bool isDaylight(int currentHour, int currentMinute, int sunriseHour, int sunriseMinute, 
                int sunsetHour, int sunsetMinute) {
    int currentTime = currentHour * 60 + currentMinute;
    int sunriseTime = sunriseHour * 60 + sunriseMinute;
    int sunsetTime = sunsetHour * 60 + sunsetMinute;
    return (currentTime >= sunriseTime && currentTime < sunsetTime);
}


//================ WEB SERVER HANDLERS ================
void handleRoot() {
    // Print the raw authorization header to see what's being sent
    Serial.println("Auth header: " + server.header("Authorization"));
    
    if (!server.authenticate(http_username, http_password)) {
        Serial.printf("Authentication failed. Expected: %s/%s\n", http_username, http_password);
        // Print all headers to debug
        for (int i = 0; i < server.headers(); i++) {
            Serial.printf("Header[%s]: %s\n", 
                        server.headerName(i).c_str(), 
                        server.header(i).c_str());
        }
        return server.requestAuthentication(BASIC_AUTH, "Light Controller", "Authentication Failed");
    }
    
    Serial.println("Authentication successful, sending root page");
    String page = String(INDEX_HTML);
    bool isLightOn = (digitalRead(RELAY_PIN) == relayOn);
    String stateInfo = getSystemStateString();
    
    page.replace("[COLOR_STATUS]", isLightOn ? "#4CAF50" : "#ff4444");
    page.replace("[LIGHT_STATE]", isLightOn ? "ON" : "OFF");
    page.replace("[CURRENT_TIME]", timeClient.getFormattedTime());
    page.replace("[BUTTON_TEXT]", isLightOn ? "Turn Off" : "Turn On");
    page.replace("[BUTTON_ICON]", isLightOn ? "🌙" : "🔆");
    page.replace("[CLOUD_COVERAGE]", currentCloudCoverage < 0 ? "Unknown" : String(currentCloudCoverage, 1));
    page.replace("[CLOUD_STATUS]", isMonitoring ? stateInfo : cloudStatus);
    page.replace("[CLOUD_THRESHOLD]", String(cloudThreshold));
    page.replace("[CLOUD_HYSTERESIS]", String(cloudHysteresis));
    page.replace("[MONITORING_WINDOW]", String(monitoringWindow));
    page.replace("[OVERRIDE_DURATION]", String(manualOverrideDuration)); 
    page.replace("[SUNRISE_HOUR]", String(sunriseHour));
    page.replace("[SUNRISE_MINUTE]", String(sunriseMinute < 10 ? "0" : "") + String(sunriseMinute));
    page.replace("[SUNSET_HOUR]", String(sunsetHour));
    page.replace("[SUNSET_MINUTE]", String(sunsetMinute < 10 ? "0" : "") + String(sunsetMinute));
    page.replace("[ADMIN_USERNAME]", String(adminUsername));
    page.replace("[MAX_RETRIES]", String(maxRetries));
    page.replace("[TIMEZONE_OFFSET]", String(timezoneOffsetSec));
    page.replace("[DAYLIGHT_OFFSET]", String(daylightOffsetSec));
    page.replace("[SUNRISE_OFFSET]", String(sunriseOffset));
    page.replace("[SUNSET_OFFSET]", String(sunsetOffset));
    page.replace("[LATITUDE]", String(locationLatitude, 6));
    page.replace("[LONGITUDE]", String(locationLongitude, 6));
    page.replace("[DEVICE_NAME]", String(deviceName));
    page.replace("[RELAY_LOW_SELECTED]", relayOn == LOW ? "selected" : "");
    page.replace("[RELAY_HIGH_SELECTED]", relayOn == HIGH ? "selected" : "");
    
    server.send(200, "text/html", page);
    ESP.wdtFeed();
}

void handleSetTime() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }

    bool changed = false;

    if (server.hasArg("sunriseHour")) {
        int newHour = constrain(server.arg("sunriseHour").toInt(), 0, 23);
        if (sunriseHour != newHour) {
            sunriseHour = newHour;
            changed = true;
        }
    }
    
    if (server.hasArg("sunriseMinute")) {
        int newMinute = constrain(server.arg("sunriseMinute").toInt(), 0, 59);
        if (sunriseMinute != newMinute) {
            sunriseMinute = newMinute;
            changed = true;
        }
    }
    
    if (server.hasArg("sunsetHour")) {
        int newHour = constrain(server.arg("sunsetHour").toInt(), 0, 23);
        if (sunsetHour != newHour) {
            sunsetHour = newHour;
            changed = true;
        }
    }
    
    if (server.hasArg("sunsetMinute")) {
        int newMinute = constrain(server.arg("sunsetMinute").toInt(), 0, 59);
        if (sunsetMinute != newMinute) {
            sunsetMinute = newMinute;
            changed = true;
        }
    }

    if (changed) {
        Serial.printf("Saving new times - Sunrise: %02d:%02d, Sunset: %02d:%02d\n", 
                    sunriseHour, sunriseMinute, sunsetHour, sunsetMinute);
        saveSettings();
        monitoring_sunrise = true;
        monitoring_sunset = true;
    }
    
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleToggle() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }
    
    manualOverride = true;
    manualOverrideStartTime = millis();
    manualLightState = !(digitalRead(RELAY_PIN) == relayOn);
    
    digitalWrite(RELAY_PIN, manualLightState ? relayOn : relayOff);
    digitalWrite(STATUS_LED_PIN, manualLightState);
    
    currentState = MANUAL;
    
    Serial.print("Manual light toggled to: ");
    Serial.println(manualLightState ? "ON" : "OFF");
    
    if (server.hasArg("api")) {
        String jsonResponse = "{\"success\":true,\"state\":\"" + String(manualLightState ? "on" : "off") + "\"}";
        server.send(200, "application/json", jsonResponse);
    } else {
        server.sendHeader("Location", "/");
        server.send(303);
    }
}

void handleCloudMonitoring() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }

    float newCloudCoverage = getCloudCoverage();
    bool success = (newCloudCoverage >= 0);
    
    if (success) { 
        currentCloudCoverage = newCloudCoverage;
        
        if (isWithinMonitoringWindow(hour(), minute())) {
            if (currentCloudCoverage > CLOUD_COVERAGE_THRESHOLD) {
                cloudTriggeredActivation = true;
                currentState = ACTIVE;
                digitalWrite(RELAY_PIN, relayOn);
            }
        }
    }
    if (server.hasArg("api")) {
        String jsonResponse = "{\"success\":" + String(success ? "true" : "false") + 
                             ",\"cloudCoverage\":" + String(currentCloudCoverage) +
                             ",\"isMonitoring\":" + String(isMonitoring ? "true" : "false") +
                             ",\"status\":\"" + cloudStatus + "\"}";
        server.send(200, "application/json", jsonResponse);
    } else {
        server.sendHeader("Location", "/");
        server.send(303);
    }
}

void handleSystemStatus() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }
    
    bool isLightOn = (digitalRead(RELAY_PIN) == relayOn);
    String stateStr = getSystemStateString();
    
    DynamicJsonDocument doc(512);
    doc["success"] = true;
    doc["lightOn"] = isLightOn;
    doc["systemState"] = stateStr;
    doc["cloudCoverage"] = currentCloudCoverage;
    doc["cloudStatus"] = cloudStatus;
    doc["isMonitoring"] = isMonitoring;
    doc["sunriseTime"] = String(sunriseHour) + ":" + (sunriseMinute < 10 ? "0" : "") + String(sunriseMinute);
    doc["sunsetTime"] = String(sunsetHour) + ":" + (sunsetMinute < 10 ? "0" : "") + String(sunsetMinute);
    doc["time"] = timeClient.getFormattedTime();
    
    String jsonResponse;
    serializeJson(doc, jsonResponse);
    server.send(200, "application/json", jsonResponse);
}

void handleSaveConfig() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }
    
    bool changed = false;
    
    if (server.hasArg("cloudThreshold")) {
        int newThreshold = constrain(server.arg("cloudThreshold").toInt(), 0, 100);
        if (cloudThreshold != newThreshold) {
            cloudThreshold = newThreshold;
            changed = true;
        }
    }
    
    if (server.hasArg("cloudHysteresis")) {
        int newHysteresis = constrain(server.arg("cloudHysteresis").toInt(), 0, 20);
        if (cloudHysteresis != newHysteresis) {
            cloudHysteresis = newHysteresis;
            changed = true;
        }
    }
    
    if (server.hasArg("monitoringWindow")) {
        int newWindow = constrain(server.arg("monitoringWindow").toInt(), 5, 120);
        if (monitoringWindow != newWindow) {
            monitoringWindow = newWindow;
            changed = true;
        }
    }
    
    if (server.hasArg("overrideDuration")) {
        int newDuration = constrain(server.arg("overrideDuration").toInt(), 5, 1440);
        if (manualOverrideDuration != newDuration) {
            manualOverrideDuration = newDuration;
            changed = true;
        }
    }
    
    if (changed) {
        Serial.println("Saving configuration changes");
        saveSettings();
    }
    
    server.sendHeader("Location", "/");
    server.send(303);
}

// Add new handlers for the advanced and system configurations
void handleSaveAdvanced() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }
    
    bool changed = false;
    bool requiresReconnect = false;
    bool sunTimeChanged = false;
    
    if (server.hasArg("sunriseOffset")) {
        int newOffset = constrain(server.arg("sunriseOffset").toInt(), -120, 120);
        if (sunriseOffset != newOffset) {
            sunriseOffset = newOffset;
            changed = true;
            sunTimeChanged = true;
        }
    }
    
    if (server.hasArg("sunsetOffset")) {
        int newOffset = constrain(server.arg("sunsetOffset").toInt(), -120, 120);
        if (sunsetOffset != newOffset) {
            sunsetOffset = newOffset;
            changed = true;
            sunTimeChanged = true;
        }
    }
    
    if (server.hasArg("maxRetries")) {
        int newRetries = constrain(server.arg("maxRetries").toInt(), 1, 10);
        if (maxRetries != newRetries) {
            maxRetries = newRetries;
            changed = true;
        }
    }
    
    if (server.hasArg("timezoneOffset")) {
        int newOffset = server.arg("timezoneOffset").toInt();
        if (timezoneOffsetSec != newOffset) {
            timezoneOffsetSec = newOffset;
            changed = true;
            requiresReconnect = true;
            sunTimeChanged = true;
        }
    }
    
    if (server.hasArg("daylightOffset")) {
        int newOffset = server.arg("daylightOffset").toInt();
        if (daylightOffsetSec != newOffset) {
            daylightOffsetSec = newOffset;
            changed = true;
            requiresReconnect = true;
            sunTimeChanged = true;
        }
    }
    
    if (changed) {
        Serial.println("Saving advanced configuration changes");
        saveSettings();
        
        if (requiresReconnect) {
            // Update the NTP client with new offsets
            timeClient.setTimeOffset(timezoneOffsetSec + daylightOffsetSec);
            lastTimeSync = 0; // Force time sync
        }
        
        if (sunTimeChanged) {
            // Recalculate sunrise/sunset times
            updateSunriseSunsetTime();
        }
    }
    
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleSaveSystem() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }
    
    bool changed = false;
    bool credentialsChanged = false;
    
    if (server.hasArg("username") && server.arg("username").length() > 0 && 
        server.arg("username").length() < 32) {
        if (strcmp(adminUsername, server.arg("username").c_str()) != 0) {
            strncpy(adminUsername, server.arg("username").c_str(), 31);
            adminUsername[31] = 0; // Ensure null termination
            strcpy(http_username, adminUsername); // Update the authentication variable
            changed = true;
            credentialsChanged = true;
        }
    }
    
    if (server.hasArg("password") && server.arg("password").length() > 0 && 
        server.arg("password").length() < 32) {
        if (strcmp(adminPassword, server.arg("password").c_str()) != 0) {
            strncpy(adminPassword, server.arg("password").c_str(), 31);
            adminPassword[31] = 0; // Ensure null termination
            strcpy(http_password, adminPassword); // Update the authentication variable
            changed = true;
            credentialsChanged = true;
        }
    }
    
    if (server.hasArg("deviceName") && server.arg("deviceName").length() > 0 && 
        server.arg("deviceName").length() < 32) {
        if (strcmp(deviceName, server.arg("deviceName").c_str()) != 0) {
            strncpy(deviceName, server.arg("deviceName").c_str(), 31);
            deviceName[31] = 0; // Ensure null termination
            changed = true;
        }
    }
    
    if (server.hasArg("relayLogic")) {
        String relayLogic = server.arg("relayLogic");
        bool newRelayOn = (relayLogic == "active_high") ? HIGH : LOW;
        bool newRelayOff = (relayLogic == "active_high") ? LOW : HIGH;
        
        if (relayOn != newRelayOn) {
            relayOn = newRelayOn;
            relayOff = newRelayOff;
            changed = true;
            
            // Apply the new relay state immediately
            bool currentState = (digitalRead(RELAY_PIN) == relayOn);
            digitalWrite(RELAY_PIN, currentState ? relayOn : relayOff);
        }
    }
    
    if (changed) {
        Serial.println("Saving system configuration changes");
        saveSettings();
        
        if (credentialsChanged) {
            Serial.println("Credentials changed - new auth will be required");
        }
    }
    
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleSaveLocation() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }
    
    bool changed = false;
    
    if (server.hasArg("latitude")) {
        float newLat = server.arg("latitude").toFloat();
        if (abs(locationLatitude - newLat) > 0.00001 && newLat >= -90 && newLat <= 90) {
            locationLatitude = newLat;
            changed = true;
        }
    }
    
    if (server.hasArg("longitude")) {
        float newLong = server.arg("longitude").toFloat();
        if (abs(locationLongitude - newLong) > 0.00001 && newLong >= -180 && newLong <= 180) {
            locationLongitude = newLong;
            changed = true;
        }
    }
    
    if (changed) {
        Serial.printf("Location updated to: %f, %f\n", locationLatitude, locationLongitude);
        saveSettings();
        
        // Apply changes to the sunset calculator
        sun.setPosition(locationLatitude, locationLongitude, (timezoneOffsetSec + daylightOffsetSec) / 3600.0);
        
        // Recalculate sunrise/sunset times
        updateSunriseSunsetTime();
        
        // Force time sync
        lastTimeSync = 0;
    }
    
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleReboot() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }
    
    server.send(200, "text/html", "<html><body><h1>Rebooting...</h1><p>The device is rebooting. Please wait 30 seconds before reconnecting.</p><script>setTimeout(function(){location.href='/'},30000);</script></body></html>");
    delay(500);
    ESP.restart();
}

void handleNotFound() {
    if (server.method() == HTTP_OPTIONS) {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(200);
        return;
    }
    
    String message = "File Not Found\n\n";
    message += "URI: " + server.uri() + "\n";
    message += "Method: " + String((server.method() == HTTP_GET) ? "GET" : "POST") + "\n";
    server.send(404, "text/plain", message);
}

//================ SETUP & LOOP ================
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, relayOff);
  digitalWrite(ERROR_LED_PIN, HIGH);
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  lastTimeSync = millis();
  wifiEnabled = true;
  
  // Load settings from EEPROM first
  loadSettings();
  
  // Print all auth details for debugging
  Serial.println("\n*** AUTHENTICATION DEBUG ***");
  Serial.printf("HTTP username: [%s]\n", http_username);
  Serial.printf("HTTP password: [%s]\n", http_password);
  Serial.printf("Admin username: [%s]\n", adminUsername);
  Serial.printf("Admin password: [%s]\n", adminPassword);
  
  // Check if credentials are valid, if not set defaults
  if (strlen(adminUsername) == 0 || strlen(adminPassword) == 0) {
    strcpy(adminUsername, "admin");
    strcpy(adminPassword, "admin");
    strcpy(http_username, "admin");
    strcpy(http_password, "admin");
    
    // Save the default credentials to EEPROM
    saveSettings();
    Serial.println("Set and saved default credentials: admin/admin");
  }
  
  // Check for any non-printable characters that might cause issues
  bool hasBadChars = false;
  for (int i = 0; i < strlen(http_username); i++) {
    if (http_username[i] < 32 || http_username[i] > 126) {
      hasBadChars = true;
      Serial.printf("Bad character in username at position %d: %d\n", i, http_username[i]);
    }
  }
  
  for (int i = 0; i < strlen(http_password); i++) {
    if (http_password[i] < 32 || http_password[i] > 126) {
      hasBadChars = true;
      Serial.printf("Bad character in password at position %d: %d\n", i, http_password[i]);
    }
  }
  
  if (hasBadChars) {
    Serial.println("WARNING: Non-printable characters detected in credentials!");
    strcpy(http_username, "admin");
    strcpy(http_password, "admin");
    strcpy(adminUsername, "admin");
    strcpy(adminPassword, "admin");
    saveSettings();
    Serial.println("Credentials reset to admin/admin due to corruption");
  }
  
  Serial.printf("Using credentials: %s / %s\n", http_username, http_password);
  
  connectToWiFi();
  
  ArduinoOTA.begin();
  ArduinoOTA.setHostname(deviceName); // Use the configured device name
  ArduinoOTA.setPassword(adminPassword); // Use the admin password for OTA
  
  // Apply configured timezone and DST settings
  timeClient.setTimeOffset(timezoneOffsetSec + daylightOffsetSec);
  timeClient.begin();
  updateLocalTime();

  // Use stored latitude and longitude
  sun.setPosition(locationLatitude, locationLongitude, (timezoneOffsetSec + daylightOffsetSec) / 3600.0);
  updateSunriseSunsetTime();
  setupWebServer();
  server.begin();
  
  Serial.println("Setup complete");
}

void loop() {
    ESP.wdtFeed();
    if (WiFi.status() == WL_CONNECTED) {
        ArduinoOTA.handle();
        server.handleClient();
        if ((millis() - lastTimeSync) > SYNC_INTERVAL) {
            updateLocalTime();
            updateSunriseSunsetTime();
            lastTimeSync = millis();
        }
    } else if (wifiEnabled) {
        connectToWiFi();
    }
    
    if (manualOverride) {
        unsigned long overrideElapsedMinutes = (millis() - manualOverrideStartTime) / 60000;
        
        if (overrideElapsedMinutes >= manualOverrideDuration) {
            manualOverride = false;
            currentState = NORMAL;
            Serial.println("Manual override expired, returning to automatic control");
        } else {
            digitalWrite(RELAY_PIN, manualLightState ? relayOn : relayOff);
            digitalWrite(STATUS_LED_PIN, manualLightState);
        }
    } else {
        time_t now = timeClient.getEpochTime();
        int currentHour = hour(now);
        int currentMinute = minute(now);
        isWithinMonitoringWindow(currentHour, currentMinute);
        bool shouldBeOn = shouldActivateLights(currentHour, currentMinute);
        digitalWrite(RELAY_PIN, shouldBeOn ? relayOn : relayOff);
        digitalWrite(ERROR_LED_PIN, wifiEnabled && (WiFi.status() != WL_CONNECTED));
        if (cloudTriggeredActivation) {
            digitalWrite(STATUS_LED_PIN, (millis() / 500) % 2); // Blink for cloud triggered
        } else {
            digitalWrite(STATUS_LED_PIN, shouldBeOn); // Steady for normal schedule
        }
    }
    
    delay(100);
    yield();
}

//================ HELPER FUNCTIONS ================
void setupWebServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/settime", HTTP_POST, handleSetTime);
    server.on("/toggle", HTTP_GET, handleToggle);
    server.on("/cloudcheck", HTTP_GET, handleCloudMonitoring);
    server.on("/saveconfig", HTTP_POST, handleSaveConfig);
    server.on("/saveadvanced", HTTP_POST, handleSaveAdvanced);
    server.on("/savelocation", HTTP_POST, handleSaveLocation);
    server.on("/savesystem", HTTP_POST, handleSaveSystem);
    server.on("/reboot", HTTP_GET, handleReboot);
    
    server.on("/api/status", HTTP_GET, handleSystemStatus);
    
    server.on("/reset", HTTP_GET, []() {
        loadSettings();
        server.sendHeader("Location", "/");
        server.send(303);
    });
    
    server.on("/updateonline", HTTP_GET, []() {
        updateSunriseSunsetTime();
        server.sendHeader("Location", "/");
        server.send(303);
    });
    server.on("/reconnect", HTTP_GET, []() {
        enableWiFi();
        connectToWiFi();
        server.sendHeader("Location", "/");
        server.send(303);
    });
    // Add a special reset credentials endpoint
    server.on("/factory_reset", HTTP_GET, []() {
        Serial.println("FACTORY RESET requested - resetting credentials to admin/admin");
        strcpy(adminUsername, "admin");
        strcpy(adminPassword, "admin");
        strcpy(http_username, "admin");
        strcpy(http_password, "admin");
        saveSettings();
        
        // Send response without requiring authentication
        server.send(200, "text/html", "<html><body><h1>Credentials Reset</h1>"
                   "<p>Username and password have been reset to: admin/admin</p>"
                   "<p><a href='/'>Go to login page</a></p></body></html>");
    });
    
    server.on("/debug_auth", HTTP_GET, []() {
        String debug = "<html><body><h1>Auth Debug</h1>";
        debug += "<p>Current credentials: " + String(http_username) + " / " + String(http_password) + "</p>";
        debug += "<p>EEPROM credentials: " + String(adminUsername) + " / " + String(adminPassword) + "</p>";
        debug += "</body></html>";
        
        // Send without auth to allow debugging
        server.send(200, "text/html", debug);
    });
    server.onNotFound(handleNotFound);
    server.enableCORS(true);
    Serial.println("Web server routes configured");
}