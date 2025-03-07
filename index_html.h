#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Light Controller</title>
    <style>
        body { font-family: Arial; margin: 0; padding: 20px; color: #333; }
        .card { background: #fff; border: 1px solid #ddd; padding: 15px; margin-bottom: 15px; border-radius: 4px; }
        .status { display: flex; align-items: center; }
        .indicator { width: 20px; height: 20px; border-radius: 50%; margin-right: 10px; }
        button { padding: 8px 12px; margin: 5px 0; cursor: pointer; }
        input { padding: 5px; margin: 5px 0; }
        .time-inputs { display: flex; gap: 10px; }
        .section-title { margin-top: 0; border-bottom: 1px solid #eee; padding-bottom: 8px; }
        .form-row { margin-bottom: 8px; }
        .priority-note { font-size: 12px; font-style: italic; color: #666; }
        .tab-container { border: 1px solid #ddd; margin-bottom: 10px; border-radius: 4px; overflow: hidden; }
        .tab-buttons { display: flex; background: #f5f5f5; }
        .tab-button { padding: 10px; border: none; background: none; cursor: pointer; flex: 1; }
        .tab-button.active { background: #fff; border-bottom: 2px solid #3498db; }
        .tab-content { display: none; padding: 15px; }
        .tab-content.active { display: block; }
    </style>
</head>
<body>
    <div class="card">
        <h2 class="section-title">System Status</h2>
        <div class="status">
            <div class="indicator" style="background-color: [COLOR_STATUS]"></div>
            <div>
                <p><b>Light:</b> [LIGHT_STATE]</p>
                <p><b>Current Time:</b> [CURRENT_TIME]</p>
                <p><b>Cloud Coverage:</b> [CLOUD_COVERAGE]%</p>
                <p><b>Status:</b> [CLOUD_STATUS]</p>
            </div>
        </div>
        <button onclick="location.href='/cloudcheck'">Check Clouds Now</button>
        <button onclick="location.href='/updateonline'">Update Sun Times</button>
    </div>

    <div class="card">
        <h2 class="section-title">Manual Control</h2>
        <p class="priority-note">Manual control will override automatic scheduling for the configured duration</p>
        <button onclick="location.href='/toggle'">[BUTTON_ICON] [BUTTON_TEXT]</button>
    </div>

    <div class="card">
        <h2 class="section-title">Time Settings</h2>
        <form action="/settime" method="post">
            <div class="form-row">
                <label>Sunrise Time:</label>
                <div class="time-inputs">
                    <input type="number" name="sunriseHour" min="0" max="23" value="[SUNRISE_HOUR]" style="width:40px">
                    <span>:</span>
                    <input type="number" name="sunriseMinute" min="0" max="59" value="[SUNRISE_MINUTE]" style="width:40px">
                </div>
            </div>
            <div class="form-row">
                <label>Sunset Time:</label>
                <div class="time-inputs">
                    <input type="number" name="sunsetHour" min="0" max="23" value="[SUNSET_HOUR]" style="width:40px">
                    <span>:</span>
                    <input type="number" name="sunsetMinute" min="0" max="59" value="[SUNSET_MINUTE]" style="width:40px">
                </div>
            </div>
            <button type="submit">Save Times</button>
        </form>
    </div>

    <div class="tab-container">
        <div class="tab-buttons">
            <button class="tab-button active" onclick="openTab(event, 'basicConfig')">Basic Config</button>
            <button class="tab-button" onclick="openTab(event, 'advancedConfig')">Advanced</button>
            <button class="tab-button" onclick="openTab(event, 'locationConfig')">Location</button>
            <button class="tab-button" onclick="openTab(event, 'systemConfig')">System</button>
        </div>
        
        <div id="basicConfig" class="tab-content active">
            <form action="/saveconfig" method="post">
                <div class="form-row">
                    <label>Cloud Threshold (%):</label>
                    <input type="number" name="cloudThreshold" min="0" max="100" value="[CLOUD_THRESHOLD]" style="width:60px">
                </div>
                <div class="form-row">
                    <label>Cloud Hysteresis (%):</label>
                    <p class="priority-note">Prevents rapid on/off switching when cloud coverage fluctuates</p>
                    <input type="number" name="cloudHysteresis" min="0" max="20" value="[CLOUD_HYSTERESIS]" style="width:60px">
                </div>
                <div class="form-row">
                    <label>Monitoring Window (mins before):</label>
                    <p class="priority-note">how long before sunrise/sunset to start cloud monitoring (5-120 mins)</p>
                    <input type="number" name="monitoringWindow" min="5" max="120" value="[MONITORING_WINDOW]" style="width:60px">
                </div>
                <div class="form-row">
                    <label>Manual Override Duration (mins):</label>
                    <p class="priority-note">how long manual control lasts before automatic control resumes (5-1440 mins)</p>
                    <input type="number" name="overrideDuration" min="5" max="1440" value="[OVERRIDE_DURATION]" style="width:60px">
                </div>
                <button type="submit">Save Configuration</button>
            </form>
        </div>
        
        <div id="advancedConfig" class="tab-content">
            <form action="/saveadvanced" method="post">
                <div class="form-row">
                    <label>Sunrise Offset (minutes):</label>
                    <p class="priority-note">Adjust sunrise time (negative = earlier, positive = later)</p>
                    <input type="number" name="sunriseOffset" value="[SUNRISE_OFFSET]" style="width:60px">
                </div>
                <div class="form-row">
                    <label>Sunset Offset (minutes):</label>
                    <p class="priority-note">Adjust sunset time (negative = earlier, positive = later)</p>
                    <input type="number" name="sunsetOffset" value="[SUNSET_OFFSET]" style="width:60px">
                </div>
                <div class="form-row">
                    <label>Max Monitoring Retries:</label>
                    <p class="priority-note">How many times to retry cloud checks if they fail</p>
                    <input type="number" name="maxRetries" min="1" max="10" value="[MAX_RETRIES]" style="width:60px">
                </div>
                <div class="form-row">
                    <label>Time Offset (seconds):</label>
                    <p class="priority-note">Adjust to match your timezone (in seconds)</p>
                    <input type="number" name="timezoneOffset" value="[TIMEZONE_OFFSET]" style="width:80px">
                </div>
                <div class="form-row">
                    <label>Daylight Saving Offset (seconds):</label>
                    <input type="number" name="daylightOffset" value="[DAYLIGHT_OFFSET]" style="width:80px">
                </div>
                <button type="submit">Save Advanced Configuration</button>
            </form>
        </div>
        
        <div id="locationConfig" class="tab-content">
            <form action="/savelocation" method="post">
                <div class="form-row">
                    <label>Latitude:</label>
                    <input type="number" name="latitude" step="0.000001" value="[LATITUDE]" style="width:120px">
                </div>
                <div class="form-row">
                    <label>Longitude:</label>
                    <input type="number" name="longitude" step="0.000001" value="[LONGITUDE]" style="width:120px">
                </div>
                <p class="priority-note">Changes to location require a device reboot to take effect</p>
                <button type="submit">Save Location</button>
            </form>
        </div>
        
        <div id="systemConfig" class="tab-content">
            <form action="/savesystem" method="post">
                <div class="form-row">
                    <label>Admin Username:</label>
                    <input type="text" name="username" value="[ADMIN_USERNAME]">
                </div>
                <div class="form-row">
                    <label>Admin Password:</label>
                    <input type="password" name="password" placeholder="Enter to change">
                </div>
                <div class="form-row">
                    <label>Device Name:</label>
                    <input type="text" name="deviceName" value="[DEVICE_NAME]">
                </div>
                <div class="form-row">
                    <label>Relay Logic:</label>
                    <select name="relayLogic">
                        <option value="active_low" [RELAY_LOW_SELECTED]>Active Low (most relays)</option>
                        <option value="active_high" [RELAY_HIGH_SELECTED]>Active High</option>
                    </select>
                </div>
                <button type="submit">Save System Configuration</button>
            </form>
            <div style="margin-top:10px">
                <button onclick="location.href='/reset'" style="background:#ffcc00">Reset Settings</button>
                <button onclick="location.href='/reconnect'" style="background:#3498db;color:white">Reconnect WiFi</button>
                <button onclick="if(confirm('Are you sure you want to reboot the device?')) location.href='/reboot'" style="background:#e74c3c;color:white">Reboot Device</button>
            </div>
        </div>
    </div>

    <div class="card">
        <p style="text-align:center;font-size:12px">Cloud Light Controller v1.1</p>
    </div>

    <script>
        function openTab(evt, tabName) {
            var i, tabcontent, tablinks;
            tabcontent = document.getElementsByClassName("tab-content");
            for (i = 0; i < tabcontent.length; i++) {
                tabcontent[i].className = tabcontent[i].className.replace(" active", "");
            }
            tablinks = document.getElementsByClassName("tab-button");
            for (i = 0; i < tablinks.length; i++) {
                tablinks[i].className = tablinks[i].className.replace(" active", "");
            }
            document.getElementById(tabName).className += " active";
            evt.currentTarget.className += " active";
        }
    </script>
</body>
</html>
)rawliteral";

#endif