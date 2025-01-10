#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <style>
    :root {
    --bg-primary: #1a1a1a;
    --bg-secondary: #2d2d2d;
    --text-primary: #ffffff;
    --text-secondary: #b3b3b3;
    --accent-color: #6d28d9;
    --accent-hover: #7c3aed;
    --border-color: #404040;
    --input-bg: #333333;
}

body {
    font-family: 'Inter', system-ui, -apple-system, sans-serif;
    background-color: var(--bg-primary);
    margin: 0;
    padding: 20px;
    color: var(--text-primary);
    min-height: 100vh;
}

.container {
    max-width: 600px;
    margin: 0 auto;
    background: var(--bg-secondary);
    padding: 24px;
    border-radius: 12px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
}

.section {
    margin-bottom: 24px;
    padding: 20px;
    border: 1px solid var(--border-color);
    border-radius: 8px;
    background: rgba(255, 255, 255, 0.03);
    transition: transform 0.2s;
}

.section:hover {
    transform: translateY(-2px);
}

h2 {
    color: var(--text-primary);
    margin-top: 0;
    font-size: 1.5rem;
    font-weight: 600;
    margin-bottom: 1rem;
}

.status-indicator {
    display: flex;
    align-items: center;
    gap: 12px;
    margin: 15px 0;
    padding: 10px;
    background: rgba(255, 255, 255, 0.05);
    border-radius: 6px;
}

.status-dot {
    width: 14px;
    height: 14px;
    border-radius: 50%;
    display: inline-block;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.3);
}

.button {
    background-color: var(--accent-color);
    color: white;
    padding: 12px 24px;
    border: none;
    border-radius: 6px;
    cursor: pointer;
    width: 100%;
    font-size: 1rem;
    font-weight: 500;
    transition: all 0.2s;
}

.button:hover {
    background-color: var(--accent-hover);
    transform: translateY(-1px);
}

input[type="number"] {
    width: 70px;
    padding: 10px;
    margin: 5px;
    border: 1px solid var(--border-color);
    border-radius: 6px;
    background: var(--input-bg);
    color: var(--text-primary);
    font-size: 1rem;
}

input[type="number"]:focus {
    outline: none;
    border-color: var(--accent-color);
    box-shadow: 0 0 0 2px rgba(109, 40, 217, 0.2);
}

.time-display {
    font-size: 1.4em;
    color: var(--text-primary);
    text-align: center;
    margin: 15px 0;
    font-weight: 600;
}

.sleep-status {
    color: var(--text-secondary);
    font-size: 1rem;
    text-align: center;
    padding: 10px;
    background: rgba(255, 255, 255, 0.03);
    border-radius: 6px;
}

label {
    color: var(--text-secondary);
    margin-right: 8px;
}

.time-input-group {
    display: flex;
    align-items: center;
    margin: 12px 0;
    gap: 8px;
}

.status-emoji {
    
    font-size: 6rem;
    text-align: center;
}

@media (max-width: 480px) {
    .container {
        padding: 16px;
    }
    
    .section {
        padding: 16px;
    }
    
    input[type="number"] {
        width: 60px;
    }
}
    </style>
</head>
<body>
    <div class='container'>
        <div class='section'>
            <h2>Status</h2>
            <div class='status-indicator'>
                <div class='status-dot' style='background-color: [COLOR_STATUS]'></div>
                <span>Light is [LIGHT_STATE]</span>
            </div>
            <div class='time-display'>[CURRENT_TIME]</div>
        </div>

        <div class='section'>
            <h2>Cloud Coverage</h2>
            <div id='cloud-emoji' class='status-emoji'>&#x2601;</div>
            <br>
            <form action='/cloudcheck' method='get'>
                <label for="cloudCoverage">Cloud Coverage:</label>
                <div class="cloudcoverage-display">
                    <input type='number' name='cloudCoverage' min='0' max='100' value='[CLOUD_COVERAGE]' readonly>
                    <span>%</span>
                </div>
                <input type='submit' value='Refresh' class='button'>
                <br>
            </form>
            <br>
 
            <div id='cloud-status' class='sleep-status'>[CLOUD_STATUS]</div>
            
        </div>
        <br>


        <div class='section'>
            <h2>Control</h2>
            <form action='/toggle' method='get'>
                <input type='submit' value='[BUTTON_TEXT]' class='button'>
            </form>
        </div>

        <div class='section'>
            <h2>Time Settings</h2>
            <form action='/settime' method='get'>
                <div class="time-input-group">
                    <label>Sunrise:</label>
                    <input type='number' name='sunriseHour' min='0' max='23' value='[SUNRISE_HOUR]'>
                    <input type='number' name='sunriseMinute' min='0' max='59' value='[SUNRISE_MINUTE]'>
                </div>
                <div class="time-input-group">
                    <label>Sunset:</label>
                    <input type='number' name='sunsetHour' min='0' max='23' value='[SUNSET_HOUR]'>
                    <input type='number' name='sunsetMinute' min='0' max='59' value='[SUNSET_MINUTE]'>
                </div>
                <br>
                <input type='submit' value='Save Settings' class='button'>
            </form>
            <br>
            <form action=#' method='get'>
                <input type='submit' value='Reset' class='button'>
            </form>
        </div>
    </div>

    <script>
        function updateCloudEmoji(coverage) {
            const cloudEmoji = document.getElementById('cloud-emoji');
            let emoji = '&#x1F4AD;'; // Default thinking cloud
            let status = 'Unknown';

            if (!isNaN(coverage)) {
                if (coverage === -1) {
                    emoji = '&#x1f504;'; // Refresh
                    status = 'Need refresh';
                } else if (coverage < 30) {
                    emoji = '&#x2600;&#xFE0F;'; // Sun
                    status = 'Clear sky';
                } else if (coverage < 50) {
                    emoji = '&#x1F324;&#xFE0F;'; // Sun behind small cloud
                    status = 'Partly cloudy';
                } else if (coverage < 70) {
                    emoji = '&#x26C5;'; // Sun behind cloud
                    status = 'Mostly cloudy';
                } else {
                    emoji = '&#x2601;&#xFE0F;'; // Cloud
                    status = 'Cloudy';
                }
            }

            cloudEmoji.innerHTML = emoji; // Use innerHTML instead of textContent for HTML entities
            
        }

        // Initial update
        updateCloudEmoji([CLOUD_COVERAGE]);

        // Auto refresh every 5 minutes
        setInterval(() => {
            fetch('/cloudcheck')
                .then(response => response.text())
                .then(() => {
                    location.reload();
                })
                .catch(error => console.error('Error:', error));
        }, 300000);
    </script>
</body>
</html>
)rawliteral";

#endif