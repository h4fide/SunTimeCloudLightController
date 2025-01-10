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

// Buffer Configuration
const int BUFFER_SIZE = 6144;  
char buffer[BUFFER_SIZE]; 

// WiFi control variables
unsigned long lastTimeSync = 0;
const unsigned long SYNC_INTERVAL = 24 * 60 * 60 * 1000;
bool wifiEnabled = true;

// Pin Definitions
const int RELAY_PIN = 2;      // 12 or D6 - Main light relay
const int ERROR_LED_PIN = 14;  // 14 or D5 - Error indicator
const int STATUS_LED_PIN = 13; // 13 or D7 - Status indicator

// Change these from const to regular variables so they can be modified
int TIME_RISE_OFFSET_ADDITIONAL = 0;  
int TIME_SET_OFFSET_ADDITIONAL = 0;   

//================ MONITORING FLAGS ================
bool monitoring_sunrise = true;
bool monitoring_sunset = true;
int monitoring_retry_count = 0;

// Relay State Configuration
int relayOn = LOW;
int relayOff = HIGH; 


//================ GLOBAL VARIABLES ================
// Time Variables
int sunriseHour, sunsetHour;
int sunriseMinute, sunsetMinute;
int localtime_h, localtime_m;

// Object Instances
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, TIMEZONE_OFFSET + DAYLIGHT_OFFSET);
ESP8266WebServer server(80);
SunSet sun;


//================ SYSTEM STATE ENUM ================
enum SystemState {
    NORMAL,
    MONITORING,
    ACTIVE,
    SCHEDULED
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
  
  // Validate values
  sunriseHour = constrain(sunriseHour, 0, 23);
  sunriseMinute = constrain(sunriseMinute, 0, 59);
  sunsetHour = constrain(sunsetHour, 0, 23);
  sunsetMinute = constrain(sunsetMinute, 0, 59);
  
  EEPROM.end();
  
  Serial.println("Loaded settings:");
  Serial.printf("Sunrise: %d:%d\n", sunriseHour, sunriseMinute);
  Serial.printf("Sunset: %d:%d\n", sunsetHour, sunsetMinute);
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
        case MONITORING:
            stateStr = "Monitoring clouds (" + 
                      String(TIME_OFFSET_MONITORING) + " min before ";
            // Check if monitoring before sunrise or sunset
            if (currentTime < sunriseTime) {
                stateStr += "sunrise)";
            } else {
                stateStr += "sunset)";
            }
            break;
        case ACTIVE:
            stateStr = "Triggered";
            break;
        case SCHEDULED:
            stateStr = "Scheduled ";
            break;
        case NORMAL:
            stateStr = "Normal";
            break;
        default:
            stateStr = "Normal Operation";
    }
    return stateStr;
}

//================ MONITORING WINDOW CHECK ================
bool isWithinMonitoringWindow(int currentHour, int currentMinute) {
    int currentTime = currentHour * 60 + currentMinute;
    int sunriseTime = sunriseHour * 60 + sunriseMinute;
    int sunsetTime = sunsetHour * 60 + sunsetMinute;
    
    bool beforeSunrise = (currentTime >= (sunriseTime - TIME_OFFSET_MONITORING) && 
                         currentTime < sunriseTime);
    bool beforeSunset = (currentTime >= (sunsetTime - TIME_OFFSET_MONITORING) && 
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
    if (monitoring_retry_count >= MAX_MONITORING_RETRIES) {
        monitoring_retry_count = 0;
        if (isSunrise) {
            monitoring_sunrise = false;
            monitoring_sunset = true;
            TIME_RISE_OFFSET_ADDITIONAL = 0;
            return false;
        } else {
            monitoring_sunset = false;
            monitoring_sunrise = true;
            TIME_SET_OFFSET_ADDITIONAL = 0;
            return true;
        }
    }
    
    float cloudCoverage = getCloudCoverage();
    if (cloudCoverage < 0) {

        monitoring_retry_count++;
        delay(RETRY_DELAY_MS);
        return monitorCloudConditions(isSunrise);
    }
    
    monitoring_retry_count = 0;
    
    if (cloudCoverage > CLOUD_COVERAGE_THRESHOLD) {
        if (isSunrise) {
            TIME_RISE_OFFSET_ADDITIONAL += 30;
            return false;
        } else {
            monitoring_sunset = false;
            monitoring_sunrise = true;
            TIME_SET_OFFSET_ADDITIONAL = 0;
            return true;
        }
    }
    
    if (isSunrise) {
        monitoring_sunrise = false;
        monitoring_sunset = true;
        TIME_RISE_OFFSET_ADDITIONAL = 0;
        return false;
    } else {
        monitoring_sunset = false;
        monitoring_sunrise = true;
        TIME_SET_OFFSET_ADDITIONAL = 0;
        return true;
    }
}

//================ LIGHT CONTROL FUNCTION ================
bool shouldActivateLights(int currentHour, int currentMinute) {
    int currentTime = currentHour * 60 + currentMinute;
    int sunriseTime = sunriseHour * 60 + sunriseMinute;
    int sunsetTime = sunsetHour * 60 + sunsetMinute;
    
    if (monitoring_sunrise && currentTime >= (sunriseTime - TIME_OFFSET_MONITORING)) {
        return monitorCloudConditions(true);
    }
    
    if (monitoring_sunset && currentTime >= (sunsetTime - TIME_OFFSET_MONITORING)) {
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
    
    int timeOffset = isSunrise ? TIME_RISE_OFFSET_MINUTES : TIME_SET_OFFSET_MINUTES;
    int additionalOffset = isSunrise ? TIME_RISE_OFFSET_ADDITIONAL : TIME_SET_OFFSET_ADDITIONAL;
    
    if (minutes < timeOffset) {
        minutes = minutes - timeOffset + 60;
        hours--;
    } else {
        minutes = minutes - timeOffset;
    }
    
    if (minutes < additionalOffset) {
        minutes = minutes - additionalOffset + 60;
        hours--;
    } else {
        minutes = minutes - additionalOffset;
    }
    
    return String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
}



void updateSunriseSunsetTime() {
    sun.setCurrentDate(year(), month(), day());
    sun.setTZOffset(1);
    
    double sunrise = sun.calcSunrise();
    double sunset = sun.calcSunset();
    
    String sunriseTime = formatTime(sunrise, true);
    sunriseHour = sunriseTime.substring(0, 1).toInt();
    sunriseMinute = sunriseTime.substring(2, 4).toInt();
    
    String sunsetTime = formatTime(sunset, false);
    sunsetHour = sunsetTime.substring(0, 2).toInt();
    sunsetMinute = sunsetTime.substring(3, 5).toInt();
    
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
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }
    
    String page = String(INDEX_HTML);
    bool isLightOn = (digitalRead(RELAY_PIN) == relayOn);
    
    String stateInfo = getSystemStateString();
    
    page.replace("[COLOR_STATUS]", isLightOn ? "#4CAF50" : "#ff4444");
    page.replace("[LIGHT_STATE]", isLightOn ? "ON" : "OFF");
    page.replace("[CURRENT_TIME]", timeClient.getFormattedTime());
    page.replace("[BUTTON_TEXT]", isLightOn ? "Turn Off" : "Turn On");
    page.replace("[CLOUD_COVERAGE]", String(currentCloudCoverage, 1));
    page.replace("[CLOUD_STATUS]", isMonitoring ? stateInfo : cloudStatus);
    page.replace("[CLOUD_THRESHOLD]", String(CLOUD_COVERAGE_THRESHOLD));
    page.replace("[SUNRISE_HOUR]", String(sunriseHour));
    page.replace("[SUNRISE_MINUTE]", String(sunriseMinute));
    page.replace("[SUNSET_HOUR]", String(sunsetHour));
    page.replace("[SUNSET_MINUTE]", String(sunsetMinute));
    
    server.send(200, "text/html", page);
    ESP.wdtFeed();
}


void handleSetTime() {
  if (!server.authenticate(http_username, http_password)) {
    return server.requestAuthentication();
  }

  if (server.hasArg("sunriseHour")) {
    sunriseHour = constrain(server.arg("sunriseHour").toInt(), 0, 23);
  }
  if (server.hasArg("sunriseMinute")) {
    sunriseMinute = constrain(server.arg("sunriseMinute").toInt(), 0, 59);
  }
  if (server.hasArg("sunsetHour")) {
    sunsetHour = constrain(server.arg("sunsetHour").toInt(), 0, 23);
  }
  if (server.hasArg("sunsetMinute")) {
    sunsetMinute = constrain(server.arg("sunsetMinute").toInt(), 0, 59);
  }

  Serial.println("Saving new values:");
  Serial.printf("Sunrise: %d:%d\n", sunriseHour, sunriseMinute);
  Serial.printf("Sunset: %d:%d\n", sunsetHour, sunsetMinute);
  
  saveSettings();
  
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleToggle() {
  if (!server.authenticate(http_username, http_password)) {
    return server.requestAuthentication();
  }
  
  bool currentState = (digitalRead(RELAY_PIN) == relayOn);
  digitalWrite(RELAY_PIN, currentState ? relayOff : relayOn);
  
  digitalWrite(STATUS_LED_PIN, !currentState);
  
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleCloudMonitoring() {
    if (!server.authenticate(http_username, http_password)) {
        return server.requestAuthentication();
    }

    float newCloudCoverage = getCloudCoverage();
    
    if (newCloudCoverage >= 0) { 
        currentCloudCoverage = newCloudCoverage;
        
        if (isWithinMonitoringWindow(hour(), minute())) {
            if (currentCloudCoverage > CLOUD_COVERAGE_THRESHOLD) {
                cloudTriggeredActivation = true;
                currentState = ACTIVE;
                digitalWrite(RELAY_PIN, relayOn);
            }
        }
    }

    server.sendHeader("Location", "/");
    server.send(303);
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

  loadSettings();
  connectToWiFi();
  
  ArduinoOTA.begin();
  ArduinoOTA.setHostname("LightController");
  ArduinoOTA.setPassword("admin");
  
  updateLocalTime();

  sun.setPosition(LATITUDE, LONGITUDE, 1);
  updateSunriseSunsetTime();
  
  setupWebServer();
  server.begin();
  

}

void loop() {
    ESP.wdtFeed();
    
    time_t now = timeClient.getEpochTime();
    int currentHour = hour(now);
    int currentMinute = minute(now);
    
    if (wifiEnabled) {
        unsigned long startTime = millis();
        while (millis() - startTime < 100) {
            ArduinoOTA.handle();
            server.handleClient();
            yield();
        }
        

        if ((millis() - lastTimeSync) > SYNC_INTERVAL && currentState != MONITORING) {
            if (WiFi.status() == WL_CONNECTED) {
                updateLocalTime();
                updateSunriseSunsetTime();
                WiFi.disconnect(true);
                WiFi.mode(WIFI_OFF);
                wifiEnabled = false;
            }
        }
    }
    
    bool shouldBeOn = shouldActivateLights(currentHour, currentMinute);
    digitalWrite(RELAY_PIN, shouldBeOn ? relayOn : relayOff);
    
    digitalWrite(ERROR_LED_PIN, wifiEnabled && (WiFi.status() != WL_CONNECTED));
    
    if (cloudTriggeredActivation) {
        digitalWrite(STATUS_LED_PIN, (millis() / 500) % 2);
    } else {
        digitalWrite(STATUS_LED_PIN, shouldBeOn);
    }
    
    delay(100);
    yield();
}

//================ HELPER FUNCTIONS ================
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/settime", handleSetTime);
  server.on("/toggle", handleToggle);
  server.on("/cloudcheck", handleCloudMonitoring);
    server.on("/reset", []() {
        loadSettings();
        server.sendHeader("Location", "/");
        server.send(303);
    });
  server.on("/updateonline", []() {
    updateSunriseSunsetTime();
    server.sendHeader("Location", "/");
    server.send(303);
  });
  
  server.begin();
  Serial.println("HTTP server started at http://" + WiFi.localIP().toString());
}