#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>
#include <EEPROM.h>
#include <TimeLib.h>

#define LOG_EEPROM_START 512
#define LOG_EEPROM_SIZE 3072  // 3KB for logs
#define MAX_LOG_ENTRIES 100   // Maximum number of log entries to store
#define COMMIT_THRESHOLD 5    // Commit to EEPROM after this many writes

enum LogEntryType {
  LOG_CLOUD_COVERAGE = 0,
  LOG_LIGHT_STATE = 1,
  LOG_SYSTEM_STATE = 2,
  LOG_ERROR = 3
};

struct LogEntry {
  uint32_t timestamp;    
  uint8_t type;          
  uint8_t value;         
  uint16_t extraData;    
};

class LogManager {
private:
  uint16_t logCount;             
  uint16_t logHead;              
  uint32_t lastResetTimestamp;   
  bool initialized;
  uint8_t uncommittedWrites;     // Track writes before committing

  uint16_t getMetadataAddress() {
    return LOG_EEPROM_START;
  }
  
  uint16_t getLogEntryAddress(uint16_t index) {
    return LOG_EEPROM_START + sizeof(uint16_t)*3 + sizeof(uint32_t) + 
           (index % MAX_LOG_ENTRIES) * sizeof(LogEntry);
  }

  bool shouldResetWeekly(uint32_t currentTime) {
    if (lastResetTimestamp == 0) return false; // First run, don't reset
    
    // Get current week number (days since epoch / 7)
    uint32_t currentWeek = currentTime / 86400 / 7;
    uint32_t lastResetWeek = lastResetTimestamp / 86400 / 7;
    
    return currentWeek > lastResetWeek;
  }

  // Commit EEPROM if we've reached the threshold or force is true
  void commitIfNeeded(bool force = false) {
    if (force || uncommittedWrites >= COMMIT_THRESHOLD) {
      bool success = EEPROM.commit();
      if (success) {
        uncommittedWrites = 0;
      }
    }
  }

public:
  LogManager() : logCount(0), logHead(0), lastResetTimestamp(0), initialized(false), uncommittedWrites(0) {}

  void begin() {
    if (initialized) return; // Only initialize once
    
    // Start with total EEPROM size needed
    EEPROM.begin(LOG_EEPROM_START + LOG_EEPROM_SIZE);
    
    uint16_t metaAddr = getMetadataAddress();
    EEPROM.get(metaAddr, logCount);
    metaAddr += sizeof(uint16_t);
    EEPROM.get(metaAddr, logHead);
    metaAddr += sizeof(uint16_t);
    
    EEPROM.get(metaAddr, lastResetTimestamp);
    
    // Validate the data we loaded
    if (logCount > MAX_LOG_ENTRIES || logHead >= MAX_LOG_ENTRIES || 
        lastResetTimestamp > now() + 86400) { // Don't accept future timestamps (allow 1 day for clock inaccuracy)
      Serial.println("Invalid log data detected, resetting logs");
      logCount = 0;
      logHead = 0;
      lastResetTimestamp = now();
      saveMetadata();
    }
    
    initialized = true;
    Serial.printf("Log system initialized. Count: %d, Head: %d\n", logCount, logHead);
  }

  // Save metadata to EEPROM
  void saveMetadata() {
    uint16_t metaAddr = getMetadataAddress();
    EEPROM.put(metaAddr, logCount);
    metaAddr += sizeof(uint16_t);
    EEPROM.put(metaAddr, logHead);
    metaAddr += sizeof(uint16_t);
    EEPROM.put(metaAddr, lastResetTimestamp);
    
    // Always commit metadata changes immediately
    commitIfNeeded(true);
  }

  void addLog(LogEntryType type, uint8_t value, uint16_t extraData = 0) {
    if (!initialized) begin();
    
    uint32_t currentTime = now();
    
    if (shouldResetWeekly(currentTime)) {
      resetLogs(currentTime);
    }
    
    LogEntry entry;
    entry.timestamp = currentTime;
    entry.type = type;
    entry.value = value;
    entry.extraData = extraData;
    
    EEPROM.put(getLogEntryAddress(logHead), entry);
    uncommittedWrites++;
    
    logHead = (logHead + 1) % MAX_LOG_ENTRIES;
    if (logCount < MAX_LOG_ENTRIES) logCount++;
    
    saveMetadata();
    commitIfNeeded();
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
    Serial.println("Log system reset");
  }

  bool getLogEntry(uint16_t index, LogEntry* entry) {
    if (!initialized) begin();
    if (index >= logCount) return false;
    
    uint16_t actualIndex;
    if (logCount < MAX_LOG_ENTRIES) {
      actualIndex = index;
    } else {
      // If buffer has wrapped around, calculate the correct index
      actualIndex = (logHead - logCount + index + MAX_LOG_ENTRIES) % MAX_LOG_ENTRIES;
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
          json += (int)entry.value;
        }
        
        if (type == LOG_SYSTEM_STATE) {
          json += ",\"stateName\":\"";
          switch (entry.value) {
            case 0: json += "NORMAL"; break;
            case 1: json += "MONITORING"; break;
            case 2: json += "ACTIVE"; break;
            case 3: json += "SCHEDULED"; break;
            case 4: json += "MANUAL"; break;
            default: json += "UNKNOWN"; break;
          }
          json += "\"";
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