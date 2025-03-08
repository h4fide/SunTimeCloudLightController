#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Light Controller</title>
    <style>
        :root {
            --bg-color: #121212;
            --card-bg: #1e1e1e;
            --text-color: #e0e0e0;
            --accent-color: #3498db;
            --success-color: #2ecc71;
            --warning-color: #f39c12;
            --danger-color: #e74c3c;
            --border-color: #333;
            --input-bg: #2a2a2a;
        }
        
        body { 
            font-family: 'Segoe UI', Arial, sans-serif; 
            margin: 0; 
            padding: 0; 
            color: var(--text-color); 
            background-color: var(--bg-color);
            line-height: 1.6;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }
        
        .header {
            text-align: center;
            padding: 20px 0;
            border-bottom: 1px solid var(--border-color);
            margin-bottom: 30px;
        }
        
        .header h1 {
            margin: 0;
            color: var(--accent-color);
        }
        
        .main-cards {
            display: flex;
            flex-direction: column;
            gap: 20px;
            margin-bottom: 30px;
            max-width: 800px;
            margin-left: auto;
            margin-right: auto;
        }
        
        @media (max-width: 768px) {
            .main-cards {
                grid-template-columns: 1fr;
            }
        }
        
        .card { 
            background: var(--card-bg); 
            border: 1px solid var(--border-color); 
            padding: 30px; 
            margin-bottom: 20px; 
            border-radius: 12px; 
            box-shadow: 0 8px 16px rgba(0, 0, 0, 0.4);
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }
        
        .card:hover {
            box-shadow: 0 12px 20px rgba(0, 0, 0, 0.5);
        }
        
        .card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 5px;
            background: linear-gradient(90deg, var(--accent-color), var(--success-color));
        }
        
        .light-control-card::before {
            background: linear-gradient(90deg, #3498db, #2ecc71);
        }
        
        .time-card::before {
            background: linear-gradient(90deg, #9b59b6, #3498db);
        }
        
        .cloud-card::before {
            background: linear-gradient(90deg, #f39c12, #e74c3c);
        }
        
        .status { 
            display: flex; 
            flex-direction: column;
            align-items: center; 
            margin-bottom: 15px;
        }
        
        .indicator { 
            width: 60px; 
            height: 60px; 
            border-radius: 50%; 
            margin-bottom: 15px; 
            box-shadow: 0 0 20px [COLOR_STATUS];
            display: flex;
            align-items: center;
            justify-content: center;
        }
        
        .status-data {
            text-align: center;
        }
        
        .status-data p {
            margin: 8px 0;
            font-size: 1.1em;
        }
        
        button { 
            padding: 12px 24px; 
            margin: 8px 0; 
            cursor: pointer; 
            background: var(--accent-color);
            color: white;
            border: none;
            border-radius: 8px;
            font-weight: bold;
            transition: all 0.3s ease;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        button:hover {
            box-shadow: 0 5px 15px rgba(0, 0, 0, 0.3);
        }
        
        .card-content {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        
        .time-display {
            font-size: 2.5em;
            font-weight: bold;
            margin: 20px 0;
            text-shadow: 0 0 10px rgba(52, 152, 219, 0.5);
        }
        
        .cloud-emoji {
            font-size: 5em;
            margin-bottom: 15px;
            animation: float 3s ease-in-out infinite;
        }
        
        @keyframes float {
            0% { transform: translateY(0px); }
            50% { transform: translateY(-10px); }
            100% { transform: translateY(0px); }
        }
        
        input, select { 
            padding: 8px 10px; 
            margin: 5px 0; 
            background: var(--input-bg);
            border: 1px solid var(--border-color);
            color: var(--text-color);
            border-radius: 4px;
        }
        
        input:focus, select:focus {
            outline: 2px solid var(--accent-color);
            border-color: var(--accent-color);
        }
        
        .time-inputs { 
            display: flex; 
            gap: 10px; 
            align-items: center;
        }
        
        .section-title { 
            margin-top: 0; 
            border-bottom: 1px solid var(--border-color); 
            padding-bottom: 10px;
            color: var(--accent-color);
        }
        
        .form-row { 
            margin-bottom: 15px; 
        }
        
        .priority-note { 
            font-size: 13px; 
            font-style: italic; 
            color: #888; 
            margin-top: 2px;
        }
        
        .chart-container {
            height: 400px;
            position: relative;
            margin-bottom: 20px;
            background: var(--card-bg);
            border: 1px solid var(--border-color);
            border-radius: 8px;
            padding: 15px;
        }
        
        .chart-tabs {
            display: flex;
            border-bottom: 1px solid var(--border-color);
            margin-bottom: 15px;
        }
        
        .chart-tab {
            padding: 10px 20px;
            border: none;
            background: none;
            cursor: pointer;
            border-radius: 4px 4px 0 0;
            margin-right: 2px;
            color: var(--text-color);
            transition: all 0.2s ease;
        }
        
        .chart-tab.active {
            background-color: var(--accent-color);
            color: white;
        }
        
        .chart-tab:hover:not(.active) {
            background-color: rgba(52, 152, 219, 0.2);
        }
        
        .chart-tab-content {
            display: none;
            height: 100%;
        }
        
        .chart-tab-content.active {
            display: block;
        }
        
        .chart-controls {
            display: flex;
            align-items: center;
            gap: 15px;
            margin-top: 15px;
        }
        
        .settings-container {
            max-height: 0;
            overflow: hidden;
            transition: all 0.5s ease;
        }
        
        .settings-container.visible {
            max-height: 2000px;
        }
        
        .settings-toggle {
            display: flex;
            justify-content: center;
            margin: 20px 0;
        }
        
        .settings-toggle button {
            display: flex;
            align-items: center;
            gap: 8px;
            padding: 12px 25px;
            font-size: 16px;
        }
        
        .settings-tabs {
            display: flex;
            background: var(--input-bg);
            border-radius: 8px;
            overflow: hidden;
            margin-bottom: 20px;
        }
        
        .settings-tab {
            padding: 12px;
            border: none;
            background: none;
            cursor: pointer;
            flex: 1;
            color: var(--text-color);
            transition: all 0.2s ease;
        }
        
        .settings-tab.active {
            background: var(--accent-color);
            color: white;
        }
        
        .tab-content {
            display: none;
            padding: 20px;
            background: var(--card-bg);
            border-radius: 8px;
            margin-bottom: 20px;
        }
        
        .tab-content.active {
            display: block;
        }
        
        .footer {
            text-align: center;
            padding: 20px 0;
            border-top: 1px solid var(--border-color);
            margin-top: 30px;
            color: #888;
        }
        
        .center-content {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            text-align: center;
        }
        
        .center-content .form-row {
            display: flex;
            flex-direction: column;
            align-items: center;
            margin-bottom: 20px;
        }
        
        .center-content label {
            margin-bottom: 8px;
        }

        .coverage-bar-container {
            width: 100%;
            height: 20px;
            background-color: var(--input-bg);
            border-radius: 10px;
            margin: 15px 0;
            overflow: hidden;
            position: relative;
        }
        
        .coverage-bar {
            height: 100%;
            background: linear-gradient(90deg, #3498db, #9b59b6);
            border-radius: 10px;
            transition: width 0.5s ease-out;
        }
        
        .coverage-label {
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-weight: bold;
            text-shadow: 1px 1px 3px rgba(0, 0, 0, 0.7);
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Light Controller</h1>
        </div>
        
        <div class="main-cards">
            <div class="card light-control-card">
                <h2 class="section-title">Light Control</h2>
                <div class="card-content">
                    <div class="status">
                        <div class="indicator" style="background-color: [COLOR_STATUS];">
                            <span style="font-size: 24px; color: white; text-shadow: 0 0 5px black;">
                            </span>
                        </div>
                        <div class="status-data">
                            <p><b>Light Status:</b> [LIGHT_STATE]</p>
                        </div>
                    </div>
                    <button class="main-action" onclick="location.href='/toggle'">[BUTTON_ICON] [BUTTON_TEXT]</button>
                    <p class="priority-note">Manual control overrides automatic scheduling</p>
                </div>
            </div>

            <div class="card time-card">
                <h2 class="section-title">Current Time</h2>
                <div class="card-content">
                    <div class="time-display">
                        [CURRENT_TIME]
                    </div>
                </div>
            </div>

            <div class="card cloud-card">
                <h2 class="section-title">Cloud Status</h2>
                <div class="card-content">
                    <div class="cloud-emoji">
                        <script>
                            // Display different emoji based on cloud coverage
                            const cloudCoverage = [CLOUD_COVERAGE];
                            let cloudEmoji = "&#x2600;"; // Sunny ☀️
                            
                            if (cloudCoverage >= 80) {
                                cloudEmoji = "&#x2601;"; // Cloudy ☁️
                            } else if (cloudCoverage >= 60) {
                                cloudEmoji = "&#x1F325;"; // Very cloudy 🌥️
                            } else if (cloudCoverage >= 40) {
                                cloudEmoji = "&#x26C5;"; // Partly cloudy ⛅
                            } else if (cloudCoverage >= 20) {
                                cloudEmoji = "&#x1F324;"; // Mostly sunny 🌤️
                            }
                            
                            document.write(cloudEmoji);
                        </script>
                    </div>
                    <p><b>Status:</b> [CLOUD_STATUS]</p>
                    <div class="coverage-bar-container">
                        <div class="coverage-bar" style="width: [CLOUD_COVERAGE]%;"></div>
                        <div class="coverage-label">[CLOUD_COVERAGE]% Coverage</div>
                    </div>
                    <button onclick="location.href='/cloudcheck'">Check Clouds Now</button>
                </div>
            </div>
        </div>

        <div class="card">
            <h2 class="section-title">Light Data</h2>
            
            <div class="chart-tabs">
                <button class="chart-tab active" onclick="activateChartTab(event, 'cloudChart')">Cloud Coverage</button>
                <button class="chart-tab" onclick="activateChartTab(event, 'lightChart')">Light Status</button>
                <button class="chart-tab" onclick="activateChartTab(event, 'systemChart')">System Status</button>
            </div>
            
            <div class="chart-container">
                <div id="cloudChartContainer" class="chart-tab-content active">
                    <canvas id="cloudChart"></canvas>
                </div>
                <div id="lightChartContainer" class="chart-tab-content">
                    <canvas id="lightChart"></canvas>
                </div>
                <div id="systemChartContainer" class="chart-tab-content">
                    <canvas id="systemChart"></canvas>
                </div>
            </div>
            
            <div class="chart-controls">
                <label for="timeRange">Time Range:</label>
                <select id="timeRange" onchange="updateAllCharts()">
                    <option value="24">Last 24 Hours</option>
                    <option value="48">Last 48 Hours</option>
                    <option value="168" selected>Full Week</option>
                </select>
                <button onclick="updateAllCharts()">Refresh Data</button>
            </div>
        </div>

        <div class="card">
            <h2 class="section-title">Sun Times</h2>
            <form action="/settime" method="post" class="center-content">
                <div class="form-row">
                    <label>Sunrise Time:</label>
                    <div class="time-inputs">
                        <input type="number" name="sunriseHour" min="0" max="23" value="[SUNRISE_HOUR]" style="width:50px">
                        <span>:</span>
                        <input type="number" name="sunriseMinute" min="0" max="59" value="[SUNRISE_MINUTE]" style="width:50px">
                    </div>
                </div>
                <div class="form-row">
                    <label>Sunset Time:</label>
                    <div class="time-inputs">
                        <input type="number" name="sunsetHour" min="0" max="23" value="[SUNSET_HOUR]" style="width:50px">
                        <span>:</span>
                        <input type="number" name="sunsetMinute" min="0" max="59" value="[SUNSET_MINUTE]" style="width:50px">
                    </div>
                </div>
                <div style="display: flex; gap: 10px; margin-top: 10px;">
                    <button type="submit">Save</button>
                    <button type="button" onclick="location.href='/updateonline'">Update Online</button>
                </div>
            </form>
        </div>

        <div class="settings-toggle">
            <button onclick="toggleSettings()" id="settingsToggle">
                <span id="settingsIcon">&#9881;&#65039;</span>
                <span id="settingsText">Settings</span>
            </button>
        </div>

        <div class="settings-container" id="settingsContainer">
            <div class="settings-tabs">
                <button class="settings-tab active" onclick="openTab(event, 'basicConfig')">Basic</button>
                <button class="settings-tab" onclick="openTab(event, 'advancedConfig')">Advanced</button>
                <button class="settings-tab" onclick="openTab(event, 'locationConfig')">Location</button>
                <button class="settings-tab" onclick="openTab(event, 'systemConfig')">System</button>
            </div>
            
            <div id="basicConfig" class="tab-content active">
                <form action="/saveconfig" method="post">
                    <div class="form-row">
                        <label>Cloud Threshold (%):</label>
                        <input type="number" name="cloudThreshold" min="0" max="100" value="[CLOUD_THRESHOLD]" style="width:70px">
                        <p class="priority-note">percentage cloud coverage to trigger lights default (80%)</p>
                    </div>
                    <div class="form-row">
                        <label>Cloud Hysteresis (%):</label>
                        <input type="number" name="cloudHysteresis" min="0" max="20" value="[CLOUD_HYSTERESIS]" style="width:70px">
                        <p class="priority-note">hysteresis to prevent rapid light changes in default (20%)</p>
                    </div>
                    <div class="form-row">
                        <label>Monitoring Window (mins before):</label>
                        <input type="number" name="monitoringWindow" min="5" max="120" value="[MONITORING_WINDOW]" style="width:70px">
                        <p class="priority-note">how long before sunrise/sunset to start cloud monitoring (5-120 mins) default (30)</p>
                    </div>
                    <div class="form-row">
                        <label>Manual Override Duration (mins):</label>
                        <input type="number" name="overrideDuration" min="5" max="1440" value="[OVERRIDE_DURATION]" style="width:70px">
                        <p class="priority-note">how long manual control lasts before automatic control resumes (5-1440 mins) default (30)</p>
                    </div>
                    <button type="submit">Save</button>
                </form>
            </div>
            
            <div id="advancedConfig" class="tab-content">
                <form action="/saveadvanced" method="post">
                    <div class="form-row">
                        <label>Sunrise Offset (minutes):</label>
                        <input type="number" name="sunriseOffset" value="[SUNRISE_OFFSET]" style="width:70px">
                        <p class="priority-note">Adjust sunrise time (negative = earlier, positive = later) default (30)</p>
                    </div>
                    <div class="form-row">
                        <label>Sunset Offset (minutes):</label>
                        <input type="number" name="sunsetOffset" value="[SUNSET_OFFSET]" style="width:70px">
                        <p class="priority-note">Adjust sunset time (negative = earlier, positive = later) default (30)</p>
                    </div>
                    <div class="form-row">
                        <label>Max Monitoring Retries:</label>
                        <input type="number" name="maxRetries" min="1" max="10" value="[MAX_RETRIES]" style="width:70px">
                        <p class="priority-note">How many times to retry cloud checks if they fail default (10)</p>
                    </div>
                    <div class="form-row">
                        <label>Time Offset (seconds):</label>
                        <input type="number" name="timezoneOffset" value="[TIMEZONE_OFFSET]" style="width:100px">
                        <p class="priority-note">Adjust to match your timezone (in seconds) default (0)</p>
                    </div>
                    <div class="form-row">
                        <label>Daylight Saving Offset (seconds):</label>
                        <input type="number" name="daylightOffset" value="[DAYLIGHT_OFFSET]" style="width:100px">
                        <p class="priority-note">Adjust for daylight saving time (in seconds) default (0)</p>
                    </div>
                    <button type="submit">Save</button>
                </form>
            </div>
            
            <div id="locationConfig" class="tab-content">
                <form action="/savelocation" method="post">
                    <div class="form-row">
                        <label>Latitude:</label>
                        <input type="number" name="latitude" step="0.000001" value="[LATITUDE]" style="width:150px">
                    </div>
                    <div class="form-row">
                        <label>Longitude:</label>
                        <input type="number" name="longitude" step="0.000001" value="[LONGITUDE]" style="width:150px">
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
                    <button type="submit">Save</button>
                </form>
                <div style="margin-top:20px; display: flex; gap: 10px;">
                    <button class="warning" onclick="location.href='/reset'">Reset Settings</button>
                    <button onclick="location.href='/reconnect'">Reconnect WiFi</button>
                    <button class="danger" onclick="if(confirm('Are you sure you want to reboot the device?')) location.href='/reboot'">Reboot Device</button>
                </div>
            </div>
        </div>

        <div class="footer">
            <p>Cloud Light Controller v1.1</p>
        </div>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script>
        function openTab(evt, tabName) {
            var tabcontent = document.getElementsByClassName("tab-content");
            for (var i = 0; i < tabcontent.length; i++) {
                tabcontent[i].className = tabcontent[i].className.replace(" active", "");
            }
            
            var tablinks = document.getElementsByClassName("settings-tab");
            for (var i = 0; i < tablinks.length; i++) {
                tablinks[i].className = tablinks[i].className.replace(" active", "");
            }
            
            document.getElementById(tabName).className += " active";
            evt.currentTarget.className += " active";
        }

        function toggleSettings() {
            const container = document.getElementById('settingsContainer');
            const button = document.getElementById('settingsToggle');
            const text = document.getElementById('settingsText');
            
            if (container.classList.contains('visible')) {
                container.classList.remove('visible');
                text.innerText = 'Settings';
            } else {
                container.classList.add('visible');
                text.innerText = 'Hide Settings';
            }
        }

        // Chart objects
        let cloudChart;
        let lightChart;
        let systemChart;
        
        function initCharts() {
            Chart.defaults.color = '#e0e0e0';
            Chart.defaults.borderColor = '#333';
            
            // Initialize cloud coverage chart
            const cloudCtx = document.getElementById('cloudChart').getContext('2d');
            cloudChart = new Chart(cloudCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Cloud Coverage (%)',
                        data: [],
                        borderColor: 'rgba(75, 192, 192, 1)',
                        pointBackgroundColor: 'rgba(75, 192, 192, 1)',
                        borderWidth: 2,
                        fill: {
                            target: 'origin',
                            above: 'rgba(75, 192, 192, 0.2)',
                        },
                        tension: 0.2
                    }]
                },
                options: {
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 100,
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            }
                        },
                        x: {
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            }
                        }
                    },
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            labels: {
                                color: '#e0e0e0',
                                font: {
                                    size: 14
                                }
                            }
                        }
                    }
                }
            });
            
            // Initialize light status chart
            const lightCtx = document.getElementById('lightChart').getContext('2d');
            lightChart = new Chart(lightCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Light Status',
                        data: [],
                        borderColor: 'rgba(255, 159, 64, 1)',
                        backgroundColor: 'rgba(255, 159, 64, 0.3)',
                        borderWidth: 3,
                        stepped: true
                    }]
                },
                options: {
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 1,
                            ticks: {
                                callback: function(value) {
                                    return value == 0 ? 'OFF' : 'ON';
                                }
                            },
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            }
                        },
                        x: {
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            }
                        }
                    },
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            labels: {
                                color: '#e0e0e0',
                                font: {
                                    size: 14
                                }
                            }
                        }
                    }
                }
            });
            
            // Initialize system status chart
            const systemCtx = document.getElementById('systemChart').getContext('2d');
            systemChart = new Chart(systemCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'System State',
                        data: [],
                        borderColor: 'rgba(153, 102, 255, 1)',
                        backgroundColor: 'rgba(153, 102, 255, 0.3)',
                        borderWidth: 3,
                        stepped: true
                    }]
                },
                options: {
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 4,
                            ticks: {
                                callback: function(value) {
                                    const states = ['NORMAL', 'MONITORING', 'ACTIVE', 'SCHEDULED', 'MANUAL'];
                                    return states[value] || value;
                                }
                            },
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            }
                        },
                        x: {
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            }
                        }
                    },
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            labels: {
                                color: '#e0e0e0',
                                font: {
                                    size: 14
                                }
                            }
                        }
                    }
                }
            });
            
            // Load data for all charts
            updateAllCharts();
        }

        function updateAllCharts() {
            loadLogData('cloud');
            loadLogData('light');
            loadLogData('system');
        }

        function loadLogData(type) {
            fetch('/api/logs?type=' + type)
                .then(response => response.json())
                .then(data => {
                    updateChart(data, type);
                })
                .catch(error => {
                    console.error('Error fetching log data:', error);
                });
        }

        function updateChart(data, type) {
            let chart;
            
            // Select the appropriate chart
            if (type === 'cloud') {
                chart = cloudChart;
            } else if (type === 'light') {
                chart = lightChart;
            } else if (type === 'system') {
                chart = systemChart;
            } else {
                return; // Unknown chart type
            }
            
            // Clear existing data
            chart.data.labels = [];
            chart.data.datasets[0].data = [];
            
            if (data.length === 0) {
                chart.update();
                return;
            }
            
            // Get time range filter
            const timeRangeHours = parseInt(document.getElementById('timeRange').value);
            const cutoffTime = Date.now() - (timeRangeHours * 60 * 60 * 1000);
            
            // Filter and add new data
            const filteredData = data.filter(entry => entry.time * 1000 >= cutoffTime);
            
            filteredData.forEach(entry => {
                // Convert timestamp to readable date/time
                const date = new Date(entry.time * 1000);
                const timeString = date.toLocaleTimeString();
                const dateString = date.toLocaleDateString();
                const labelString = dateString + ' ' + timeString;
                
                chart.data.labels.push(labelString);
                chart.data.datasets[0].data.push(entry.value);
            });
            
            chart.update();
        }
        
        function activateChartTab(evt, chartId) {
            // Hide all chart contents
            const chartContents = document.getElementsByClassName('chart-tab-content');
            for (let i = 0; i < chartContents.length; i++) {
                chartContents[i].classList.remove('active');
            }
            
            // Deactivate all tabs
            const tabs = document.getElementsByClassName('chart-tab');
            for (let i = 0; i < tabs.length; i++) {
                tabs[i].classList.remove('active');
            }
            
            // Show the selected chart content
            document.getElementById(chartId + 'Container').classList.add('active');
            
            // Activate the clicked tab
            evt.currentTarget.classList.add('active');
            
            // Update the chart to make sure it renders correctly
            if (chartId === 'cloudChart') {
                cloudChart.update();
            } else if (chartId === 'lightChart') {
                lightChart.update();
            } else if (chartId === 'systemChart') {
                systemChart.update();
            }
        }

        // Initialize charts on page load
        document.addEventListener('DOMContentLoaded', initCharts);
    </script>
</body>
</html>
)rawliteral";

#endif