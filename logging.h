#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>
#include <EEPROM.h>
#include <TimeLib.h>

#define LOG_EEPROM_START 512
#define LOG_EEPROM_SIZE 3072  // 3KB for logs
#define MAX_LOG_ENTRIES 100   // Maximum number of log entries to store

// Log entry types
enum LogEntryType {
  LOG_CLOUD_COVERAGE = 0,
  LOG_LIGHT_STATE = 1,
  LOG_SYSTEM_STATE = 2,
  LOG_ERROR = 3
};

// Compact log entry structure (8 bytes per entry)
struct LogEntry {
  uint32_t timestamp;    // Unix timestamp (4 bytes)
  uint8_t type;          // LogEntryType (1 byte)
  uint8_t value;         // Value 0-255 (1 byte)
  uint16_t extraData;    // Extra data (2 bytes) - can store cloud coverage as int, etc.
};

// Log storage manager class
class LogManager {
private:
  uint16_t logCount;             // Current number of log entries
  uint16_t logHead;              // Current position in circular buffer
  uint32_t lastResetTimestamp;   // When was the log last reset
  bool initialized;              // Has the log been initialized

  uint16_t getMetadataAddress() {
    return LOG_EEPROM_START;
  }
  
  uint16_t getLogEntryAddress(uint16_t index) {
    return LOG_EEPROM_START + sizeof(uint16_t)*3 + sizeof(uint32_t) + 
           (index % MAX_LOG_ENTRIES) * sizeof(LogEntry);
  }

  bool shouldResetWeekly(uint32_t currentTime) {
    // Get current week number (days since epoch / 7)
    uint32_t currentWeek = currentTime / 86400 / 7;
    uint32_t lastResetWeek = lastResetTimestamp / 86400 / 7;
    
    return currentWeek > lastResetWeek;
  }

public:
  LogManager() : logCount(0), logHead(0), lastResetTimestamp(0), initialized(false) {}

  void begin() {
    EEPROM.begin(LOG_EEPROM_START + LOG_EEPROM_SIZE);
    
    uint16_t metaAddr = getMetadataAddress();
    EEPROM.get(metaAddr, logCount);
    metaAddr += sizeof(uint16_t);
    EEPROM.get(metaAddr, logHead);
    metaAddr += sizeof(uint16_t);
    
    EEPROM.get(metaAddr, lastResetTimestamp);
    
    if (logCount > MAX_LOG_ENTRIES || logHead >= MAX_LOG_ENTRIES) {
      logCount = 0;
      logHead = 0;
      saveMetadata();
    }
    
    initialized = true;
  }

  // Save metadata to EEPROM
  void saveMetadata() {
    uint16_t metaAddr = getMetadataAddress();
    EEPROM.put(metaAddr, logCount);
    metaAddr += sizeof(uint16_t);
    EEPROM.put(metaAddr, logHead);
    metaAddr += sizeof(uint16_t);
    EEPROM.put(metaAddr, lastResetTimestamp);
    EEPROM.commit();
  }

  // Add a new log entry
  void addLog(LogEntryType type, uint8_t value, uint16_t extraData = 0) {
    if (!initialized) begin();
    
    uint32_t currentTime = now();
    
    // Check if we need to reset logs (weekly)
    if (shouldResetWeekly(currentTime)) {
      resetLogs(currentTime);
    }
    
    LogEntry entry;
    entry.timestamp = currentTime;
    entry.type = type;
    entry.value = value;
    entry.extraData = extraData;
    
    EEPROM.put(getLogEntryAddress(logHead), entry);
    
    logHead = (logHead + 1) % MAX_LOG_ENTRIES;
    if (logCount < MAX_LOG_ENTRIES) logCount++;
    
    saveMetadata();
    EEPROM.commit();
  }

  void logCloudCoverage(float cloudCoverage) {
    if (cloudCoverage >= 0 && cloudCoverage <= 100) {
      uint16_t cloudValue = (uint16_t)(cloudCoverage * 10);
      addLog(LOG_CLOUD_COVERAGE, 0, cloudValue);
    }
  }

  void logLightState(bool isOn) {
    addLog(LOG_LIGHT_STATE, isOn ? 1 : 0);
  }

  void logSystemState(uint8_t state) {
    addLog(LOG_SYSTEM_STATE, state);
  }

  void logError(uint8_t errorCode) {
    addLog(LOG_ERROR, errorCode);
  }

  void resetLogs(uint32_t resetTime) {
    logCount = 0;
    logHead = 0;
    lastResetTimestamp = resetTime;
    saveMetadata();
  }

  bool getLogEntry(uint16_t index, LogEntry* entry) {
    if (!initialized) begin();
    if (index >= logCount) return false;
    
    uint16_t actualIndex;
    if (logCount < MAX_LOG_ENTRIES) {
      actualIndex = index;
    } else {
      actualIndex = (logHead + index) % MAX_LOG_ENTRIES;
    }
    
    EEPROM.get(getLogEntryAddress(actualIndex), *entry);
    return true;
  }

  uint16_t getLogCount() {
    if (!initialized) begin();
    return logCount;
  }

  String getLogsAsJson(LogEntryType type) {
    if (!initialized) begin();
    
    String json = "[";
    bool first = true;
    
    for (uint16_t i = 0; i < logCount; i++) {
      LogEntry entry;
      if (getLogEntry(i, &entry) && entry.type == type) {
        if (!first) json += ",";
        first = false;
        
        json += "{\"time\":";
        json += entry.timestamp;
        json += ",\"value\":";
        
        if (type == LOG_CLOUD_COVERAGE) {
          float cloudValue = entry.extraData / 10.0;
          json += cloudValue;
        } else {
          json += entry.value;
        }
        
        json += "}";
      }
    }
    
    json += "]";
    return json;
  }
};

extern LogManager logManager;

#endif