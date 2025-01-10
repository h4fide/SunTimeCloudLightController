# Sunrise Sunset and Cloud Coverage Light Control System
An ESP8266-based automated light control system that manages lighting based on sunrise/sunset times and cloud coverage conditions.



### Sunset 🌇 Logic Example
```
IF Cloud Coverage < 75% THEN
    IF Sunset - 30min <= CurrentTime THEN
        Turn On Light
    ...
```
## Features ✨

- Automatic light control based on sunrise and sunset times
- Cloud coverage monitoring and adaptive light scheduling
- Web interface for manual control and configuration
- OTA (Over-The-Air) updates support
- Secure web interface with authentication
- LED status indicators for system state
- EEPROM-based settings persistence
- Configurable time offsets and thresholds

## Notes 📝
- I don't have RTC module so I used NTPClient to get the current time from the internet 😢
- I used [Open-Meteo](https://open-meteo.com/) API to get the cloud coverage data
- I used the [sunrise.h](https://github.com/buelowp/sunset) library to calculate sunrise and sunset times

## Hardware Requirements    🛠️

- ESP8266 board (NodeMCU or similar)
- Relay module
- 2x LED indicators (Status and Error)
- Power supply
- I Used AC 220V PIR Motion Sensor 
- Light

## Pin Configuration 🔢

- **RELAY_PIN** --> GPIO2 (D4) - Main light control
- **ERROR_LED_PIN** --> GPIO14 (D5) - Error indicator
- **STATUS_LED_PIN** --> GPIO13 (D7) - Status indicator

## Setup Instructions 📋

1. Hardware Assembly
   - Connect relay module to ESP8266 GPIO2 (D4)
   - Connect error LED to GPIO14 (D5) **Optional**
   - Connect status LED to GPIO13 (D7) **Optional**
   - Connect power supply to ESP8266 and relay
   - Wire the light fixture through relay

2. Software Installation
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

3. Configuration
   - Open [SunTimeCloudLightController.ino](SunTimeCloudLightController.ino) in Arduino IDE
   - Update WiFi credentials (ssid and password)
   ```cpp	
    const char* your_ssid
    const char* your_password
    ```
   - Set your location (LATITUDE and LONGITUDE)<br>
   **Note:** You can get your location from [Google Maps](https://www.google.com/maps)
    ```cpp
     const float LATITUDE = 0.0;
     const float LONGITUDE = 0.0;
     ```
   - Adjust CLOUD_COVERAGE_THRESHOLD if needed (default 78%)
    
    ```cpp
    const int CLOUD_COVERAGE_THRESHOLD = 78;
    ```
     
   - Upload code to ESP8266

4. First Run
   - The system will connect to WiFi
   - Access web interface
   - Default login: admin/admin
   - Verify sunrise/sunset times vi
   - Test manual controls

## Operation Modes 🔄

1. Normal Mode
   - Follows sunrise/sunset schedule
   - LED indicates light state

2. Cloud Monitoring Mode
   - Activates 30 minutes before sunrise/sunset
   - Checks cloud coverage
   - Adjusts timing based on conditions

3. Manual Override
   - Via web interface
   - Toggle light state
   - Update times manually

## Troubleshooting 🛠️

- Error LED On: WiFi connection issues
- Blinking LED: For Wifi , NTP, and API errors

## Contributing 🤝

Feel free to submit issues and enhancement requests.
