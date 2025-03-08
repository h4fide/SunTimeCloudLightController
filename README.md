# Sunrise Sunset and Cloud Coverage Light Control System

An ESP8266-based automated light control system that manages lighting based on sunrise/sunset times and cloud coverage conditions.

<div align="center">
   <img src="https://cdn3d.iconscout.com/3d/premium/thumb/cloudy-3d-icon-download-in-png-blend-fbx-gltf-file-formats--weather-nature-cloud-pack-icons-8639616.png?f=webp" alt="Cloud Light Controller Icon" width="150">
</div>

---


## Core Functionality

The system uses a combination of sunrise/sunset calculations and cloud coverage monitoring to control lighting. The core functionality includes:

### Simple `Example` of Sunset 🌇 Logic 
```
IF Cloud Coverage < 75% THEN
    IF Sunset - 30min <= CurrentTime THEN
        Turn On Light
    ...
```

### Light Activation Logic

The system uses the following decision logic for light activation:

$$
L(t) = 
\begin{cases} 
1 & \text{if } M_{\text{override}} = \text{true} \text{ and } M_{\text{state}} = \text{ON} \\
1 & \text{if } t < t_{\text{sunrise}} \text{ or } t \geq t_{\text{sunset}} \text{ (night time)} \\
1 & \text{if } (t_{\text{sunrise}} - \Delta t_m) \leq t < t_{\text{sunrise}} \text{ and } C > T_c \text{ (cloudy morning)} \\
1 & \text{if } (t_{\text{sunset}} - \Delta t_m) \leq t < t_{\text{sunset}} \text{ and } C > T_c \text{ (cloudy evening)} \\
0 & \text{otherwise (daytime)}
\end{cases}
$$

Where:
- $L(t)$ is the light state (1=ON, 0=OFF) at time $t$
- $t_{\text{sunrise}}$ and $t_{\text{sunset}}$ are calculated daily sunrise/sunset times
- $\Delta t_m$ is the monitoring window (configurable, default 30 min)
- $C$ is the current cloud coverage percentage
- $T_c$ is the cloud coverage threshold (configurable, default 78%)
- $M_{\text{override}}$ indicates if manual override is active
- $M_{\text{state}}$ is the manual override light state


## ✨ Feature Highlights

| Category | Features |
|----------|----------|
| 🌅 **Light Control** | • Astronomical calculations for accurate sunrise/sunset times<br>• Smart cloud coverage detection and monitoring<br>• Configurable time offsets and thresholds |
| 📊 **Monitoring** | • Real-time cloud coverage data analysis<br>• Data visualization dashboard with history charts<br>• System status and event logging |
| 🌐 **Connectivity** | • Web interface with secure authentication<br>• OTA (Over-The-Air) updates<br>• Offline capability for reliable operation |
| 🔧 **System** | • EEPROM settings persistence across power cycles<br>• Comprehensive logging of cloud coverage and light states<br>• LED status indicators for visual monitoring<br>• Failsafe operation during connection loss |
| 👋 **User Control** | • Manual override options<br>• Customizable cloud coverage thresholds<br>• Adjustable timing parameters<br>• Password-protected access with configurable credentials |

*The system intelligently adapts to natural lighting conditions based on astronomical calculations and real-time weather data while providing extensive monitoring and control capabilities.*


## Notes 📝
- I Don't have RTC module so I used NTPClient to get the current time from the internet 😢
- I used [Open-Meteo](https://open-meteo.com/) API to get the cloud coverage data
- I used the [sunrise.h](https://github.com/buelowp/sunset) library to calculate sunrise and sunset times

## Hardware Requirements 🛠️

- ESP8266 development board (NodeMCU or similar)
- Relay module (for light control)
- 2x LEDs for status indication (optional)
- Power supply
- Light fixture connection
- PIR sensor (optional)

## Pin Configuration 🔢
The hardware consists of an ESP8266 microcontroller connected to:

$$
\text{ESP8266} \rightarrow 
\begin{cases} 
\text{Relay (GPIO12/D6)} & \text{Light control} \\
\text{LED (GPIO14/D5)} & \text{Error indication (optional)} \\
\text{LED (GPIO13/D7)} & \text{Status indication (optional)}
\end{cases}
$$


## Step by Step Guide 📝

1. **Hardware Assembly**
   - Connect relay module to ESP8266 GPIO12 (D6)
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
## Troubleshooting 🛠️

- **Error LED On:**
    - Wifi connection error
- **Status LED Blinking when is:**
    - No internet connection
    - Cloud coverage API error
    - NTP time sync error
    
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

## HTTP Endpoints 🌐

### **API Endpoints**
- `/api/status` (GET) - Get system status as JSON
- `/api/logs` (GET) - Get logs as JSON (accepts typeparameter: cloud, light, system, error)
- `/toggle?api=1` (GET) - Toggle lights and return JSON status

### **System Endpoints**
- `/reboot` (GET) - Reboot device
- `/reset` (GET) - Reset to saved settings
- `/updateonline` (GET) - Update sunrise/sunset times
- `/reconnect` (GET) - Force WiFi reconnection
- `/factory_reset` (GET) - Reset credentials
- `/debug_auth` (GET) - Show authentication debug info

All endpoints require basic authentication with the configured username and password.

## Contributing 🤝

Feel free to submit issues and enhancement requests.



