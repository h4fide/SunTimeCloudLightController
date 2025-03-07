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

    <div class="card">
        <h2 class="section-title">Configuration</h2>
        <form action="/saveconfig" method="post">
            <div class="form-row">
                <label>Cloud Threshold (%):</label>
                <input type="number" name="cloudThreshold" min="0" max="100" value="[CLOUD_THRESHOLD]" style="width:60px">
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
            <div class="form-row">
                <label>Admin Username:</label>
                <input type="text" name="username" value="[ADMIN_USERNAME]">
            </div>
            <div class="form-row">
                <label>Admin Password:</label>
                <input type="password" name="password" placeholder="Enter to change">
            </div>
            <button type="submit">Save Configuration</button>
        </form>
        <div style="margin-top:10px">
            <button onclick="location.href='/reset'" style="background:#ffcc00">Reset Settings</button>
            <button onclick="location.href='/reconnect'" style="background:#3498db;color:white">Reconnect WiFi</button>
        </div>
    </div>

    <div class="card">
        <p style="text-align:center;font-size:12px">Cloud Light Controller v1.0</p>
    </div>
</body>
</html>
)rawliteral";

#endif