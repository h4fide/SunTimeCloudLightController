
# Sunrise Sunset Light Control V3

An ESP8266-based automated light control system that manages lighting based on sunrise/sunset times and cloud coverage conditions.

## Features

- Automatic light control based on sunrise and sunset times
- Cloud coverage monitoring and adaptive light scheduling
- Web interface for manual control and configuration
- OTA (Over-The-Air) updates support
- Secure web interface with authentication
- LED status indicators for system state
- EEPROM-based settings persistence
- Configurable time offsets and thresholds

## Hardware Requirements

- ESP8266 board (NodeMCU or similar)
- Relay module
- 2x LED indicators (Status and Error)
- Power supply
- Light (I used a 220v PIR sensor light)

## Pin Configuration

- RELAY_PIN: GPIO2 (D4) - Main light control
- ERROR_LED_PIN: GPIO14 (D5) - Error indicator
- STATUS_LED_PIN: GPIO13 (D7) - Status indicator

## Setup Instructions
