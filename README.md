# Sunrise Sunset and Cloud Coverage Light Control System

An ESP8266-based automated light control system that manages lighting based on sunrise/sunset times and cloud coverage conditions.

### Sunset 🌇 Logic `Example`
```
IF Cloud Coverage < 75% THEN
    IF Sunset - 30min <= CurrentTime THEN
        Turn On Light
    ...
```

## Features ✨

- Automatic light control based on sunrise and sunset times
- Intelligent cloud coverage monitoring with adaptive scheduling
- Web interface with authentication for remote control
- OTA (Over-The-Air) firmware updates
- Configurable time offsets and thresholds
- EEPROM-based settings persistence
- LED status indicators for system monitoring
- Failsafe operation with offline capabilities
- Can operate in offline mode 
- Manual override for light control

## Upcoming Features 🚀

- [+] Weather forecast integration
- [+] Energy consumption monitoring
- [+] Customizable sunrise/sunset algorithms
- [+] Add Settings for the web interface



## Notes 📝
- I Don't have RTC module so I used NTPClient to get the current time from the internet 😢
- I used [Open-Meteo](https://open-meteo.com/) API to get the cloud coverage data
- I used the [sunrise.h](https://github.com/buelowp/sunset) library to calculate sunrise and sunset times

## Hardware Requirements 🛠️

- ESP8266 development board (NodeMCU or similar)
- Relay module (for light control)
- 2x LEDs for status indication
- Power supply
- Light fixture connection

## Pin Configuration 🔢

- **RELAY_PIN** --> `GPIO2 (D4)` - Main light control
- **ERROR_LED_PIN** --> `GPIO14 (D5)` - Error indicator
- **STATUS_LED_PIN** --> `GPIO13 (D7)` - Status indicator

## Setup Instructions 📋

1. **Hardware Assembly**
   - Connect relay module to ESP8266 GPIO2 (D4)
   - Connect error LED to GPIO14 (D5) ***Optional***
   - Connect status LED to GPIO13 (D7) ***Optional***
   - Connect power supply to ESP8266 and relay
   - Wire the light fixture through relay

2. **Software Installation**
   - Install Arduino IDE
   - Add ESP8266 board support
   - Install required libraries:
     * ESP8266WiFi
     * NTPClient
     * TimeLib
     * ArduinoOTA
     * sunset.h
     * ESP8266WebServer
     * ArduinoJson

3. **Configuration ⚙️**
   - Open [config.h](config.h) in Arduino IDE
   - Update WiFi credentials (`SSID` and `PASSWORD`)
   ```cpp	
   const char *WIFI_SSID = "YOUR_SSID";
   const char *WIFI_PASSWORD = "YOUR_PASSWORD";
    ```
   - Set your location (`LATITUDE` and `LONGITUDE`)<br>
   **Note:** You can get your location from [Google Maps](https://www.google.com/maps)
    ```cpp
     const float LATITUDE = 0.0;
     const float LONGITUDE = 0.0;
     ```
   - Adjust `CLOUD_COVERAGE_THRESHOLD` if needed (default 78%)
    
    ```cpp
    const int CLOUD_COVERAGE_THRESHOLD = 78;
    ```    
    - Set `time offsets` if needed (default 0)
     ```cpp
    const int SUNRISE_OFFSET = 0;
    const int SUNSET_OFFSET = 0;
    ```
    - Set the `time zone` (default GMT 0)
    ```cpp
    const int TIME_ZONE = 0;
    ```
   - **Upload** code to ESP8266 ⬆️

4. **First Run**
   - The system will connect to WiFi
   - Access web interface at ``http://<esp8266_ip>``
   - Default login: ``admin/admin``
   - Verify sunrise/sunset times via ``serial monitor``
   - Test manual controls

## Operation Modes 🔄

1. **Normal Mode**
   - Follows sunrise/sunset schedule
   - LED indicates light state

2. **Cloud Monitoring Mode**
   - Activates 30 minutes before sunrise/sunset
   - Checks cloud coverage
   - Adjusts timing based on conditions

3. **Manual Override**
   - Via web interface
   - Toggle light state
   - Update times manually

## Troubleshooting 🛠️

- **Error LED On:**
    - Wifi connection error
- **Status LED Blinking when is:**
    - No internet connection
    - Cloud coverage API error
    - NTP time sync error


## Contributing 🤝

Feel free to submit issues and enhancement requests.


Main Endpoints:
/ - Root/homepage (GET)
/settime - Set sunrise/sunset times (POST)
/toggle - Toggle lights manually (GET)
/cloudcheck - Check cloud conditions (GET)
/saveconfig - Save configuration settings (POST)

API Endpoints:
/api/status - Get system status in JSON format (GET)

Utility Endpoints:
/reset - Load settings from EEPROM (GET)
/updateonline - Update sunrise/sunset times from online (GET)
/reconnect - Reconnect WiFi (GET)
/factory_reset - Reset credentials to default (GET)
/debug_auth - Show authentication debug info (GET)

