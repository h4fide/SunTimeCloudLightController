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
        --bg-primary: #121212;
        --bg-secondary: #1e1e1e;
        --text-primary: #ffffff;
        --text-secondary: #b3b3b3;
        --accent-color: #7c3aed;
        --accent-hover: #8b5cf6;
        --border-color: #404040;
        --input-bg: #2a2a2a;
        --success-color: #10b981;
        --warning-color: #f59e0b;
        --error-color: #ef4444;
        --card-shadow: 0 8px 20px rgba(0, 0, 0, 0.25);
        --transition-speed: 0.3s;
    }

    body {
        font-family: 'Inter', system-ui, -apple-system, sans-serif;
        background-color: var(--bg-primary);
        background-image: linear-gradient(to bottom right, #121212, #1a1a2e);
        margin: 0;
        padding: 20px;
        color: var(--text-primary);
        min-height: 100vh;
        transition: all var(--transition-speed);
    }

    .container {
        max-width: 650px;
        margin: 0 auto;
        background: var(--bg-secondary);
        padding: 30px;
        border-radius: 16px;
        box-shadow: var(--card-shadow);
        transition: transform 0.4s ease;
    }

    .header {
        text-align: center;
        margin-bottom: 30px;
        padding-bottom: 15px;
        border-bottom: 1px solid var(--border-color);
    }

    .header h1 {
        margin: 0;
        font-size: 2rem;
        background: linear-gradient(45deg, var(--accent-color), #9d4edd);
        -webkit-background-clip: text;
        background-clip: text;
        color: transparent;
        font-weight: 700;
    }

    .header .subtitle {
        color: var(--text-secondary);
        font-size: 0.95rem;
        margin-top: 8px;
    }

    .section {
        margin-bottom: 30px;
        padding: 25px;
        border-radius: 12px;
        background: rgba(255, 255, 255, 0.04);
        box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
        transition: transform 0.3s ease, box-shadow 0.3s ease;
        border: 1px solid rgba(255, 255, 255, 0.05);
    }

    .section:hover {
        transform: translateY(-3px);
        box-shadow: 0 8px 24px rgba(0, 0, 0, 0.2);
    }

    h2 {
        color: var(--text-primary);
        margin-top: 0;
        font-size: 1.5rem;
        font-weight: 600;
        margin-bottom: 1.2rem;
        display: flex;
        align-items: center;
        gap: 10px;
    }

    h2 .icon {
        font-size: 1.3rem;
        opacity: 0.9;
    }

    .status-indicator {
        display: flex;
        align-items: center;
        gap: 15px;
        margin: 15px 0;
        padding: 15px;
        background: rgba(255, 255, 255, 0.05);
        border-radius: 10px;
        transition: background-color 0.3s;
    }

    .status-indicator:hover {
        background: rgba(255, 255, 255, 0.07);
    }

    .status-dot {
        width: 16px;
        height: 16px;
        border-radius: 50%;
        display: inline-block;
        box-shadow: 0 0 12px rgba(0, 0, 0, 0.4);
        transition: all 0.3s;
    }

    .button {
        background-color: var(--accent-color);
        color: white;
        padding: 14px 24px;
        border: none;
        border-radius: 8px;
        cursor: pointer;
        width: 100%;
        font-size: 1rem;
        font-weight: 600;
        transition: all 0.2s;
        display: flex;
        align-items: center;
        justify-content: center;
        gap: 8px;
        position: relative;
        overflow: hidden;
    }

    .button:hover {
        background-color: var(--accent-hover);
        transform: translateY(-2px);
        box-shadow: 0 4px 12px rgba(124, 58, 237, 0.3);
    }

    .button:active {
        transform: translateY(1px);
    }

    .button .icon {
        font-size: 1.1rem;
    }
    
    .button::after {
        content: '';
        position: absolute;
        width: 100%;
        height: 100%;
        top: 0;
        left: 0;
        pointer-events: none;
        background-image: radial-gradient(circle, #fff 10%, transparent 10%);
        background-repeat: no-repeat;
        background-position: 50%;
        transform: scale(10, 10);
        opacity: 0;
        transition: transform 0.5s, opacity 0.8s;
    }
    
    .button:active::after {
        transform: scale(0, 0);
        opacity: 0.3;
        transition: 0s;
    }

    input[type="number"] {
        width: 70px;
        padding: 12px;
        margin: 5px;
        border: 1px solid var(--border-color);
        border-radius: 8px;
        background: var(--input-bg);
        color: var(--text-primary);
        font-size: 1rem;
        transition: all 0.3s;
    }

    input[type="number"]:focus {
        outline: none;
        border-color: var(--accent-color);
        box-shadow: 0 0 0 3px rgba(124, 58, 237, 0.2);
    }

    .time-display {
        font-size: 1.8em;
        color: var(--text-primary);
        text-align: center;
        margin: 20px 0;
        font-weight: 700;
        letter-spacing: 1px;
        text-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
    }

    .sleep-status {
        color: var(--text-secondary);
        font-size: 1.1rem;
        text-align: center;
        padding: 12px;
        background: rgba(255, 255, 255, 0.03);
        border-radius: 8px;
        transition: all 0.3s;
    }

    label {
        color: var(--text-secondary);
        margin-right: 8px;
        font-weight: 500;
        font-size: 0.95rem;
    }

    .time-input-group {
        display: flex;
        align-items: center;
        margin: 16px 0;
        gap: 10px;
        background: rgba(255, 255, 255, 0.02);
        padding: 12px;
        border-radius: 8px;
    }

    .time-input-group label {
        min-width: 70px;
    }

    .time-input-group .time-separator {
        color: var(--text-secondary);
        font-size: 1.2rem;
        margin: 0 2px;
    }

    .status-emoji {
        font-size: 7rem;
        text-align: center;
        margin-bottom: 15px;
        transition: transform 0.3s ease;
        text-shadow: 0 5px 15px rgba(0, 0, 0, 0.3);
        animation: float 5s ease-in-out infinite;
    }

    @keyframes float {
        0% { transform: translateY(0px); }
        50% { transform: translateY(-10px); }
        100% { transform: translateY(0px); }
    }

    .cloud-meter {
        height: 20px;
        background: linear-gradient(to right, #128ce2, #20325c, #1b1e29);
        border-radius: 20px;
        margin: 20px 0;
        position: relative;
        overflow: hidden;
    }

    .cloud-meter-marker {
        position: absolute;
        width: 4px;
        height: 26px;
        background: white;
        top: -3px;
        transform: translateX(-50%);
        box-shadow: 0 0 10px rgba(255, 255, 255, 0.7);
        z-index: 1;
    }

    .cloudcoverage-display {
        display: flex;
        align-items: center;
        justify-content: center;
        gap: 5px;
        margin: 15px 0;
        font-size: 1.2rem;
        font-weight: 600;
    }

    .cloudcoverage-display input {
        text-align: center;
        font-weight: 600;
        font-size: 1.1rem;
        width: 80px;
        background: rgba(255, 255, 255, 0.1);
    }

    .loader {
        display: none;
        width: 24px;
        height: 24px;
        border: 3px solid rgba(255, 255, 255, 0.3);
        border-radius: 50%;
        border-top-color: var(--accent-color);
        animation: spin 1s linear infinite;
        margin: 0 auto;
    }

    @keyframes spin {
        to { transform: rotate(360deg); }
    }

    .footer {
        text-align: center;
        color: var(--text-secondary);
        font-size: 0.85rem;
        margin-top: 30px;
        padding-top: 20px;
        border-top: 1px solid var(--border-color);
    }

    .card-grid {
        display: grid;
        grid-template-columns: 1fr 1fr;
        gap: 20px;
    }

    .button-group {
        display: grid;
        grid-template-columns: 1fr;
        gap: 15px;
        margin-top: 15px;
    }

    .hidden {
        display: none !important;
    }

    @media (max-width: 680px) {
        .container {
            padding: 20px;
        }
        
        .section {
            padding: 18px;
        }
        
        .card-grid {
            grid-template-columns: 1fr;
        }
        
        input[type="number"] {
            width: 60px;
        }
        
        .time-display {
            font-size: 1.5em;
        }
        
        .status-emoji {
            font-size: 5rem;
        }
    }
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>Sun & Cloud Controller</h1>
            <div class='subtitle'>Smart lighting based on time and weather</div>
        </div>
        
        <div class='section'>
            <h2><span class='icon'>🔆</span> Status</h2>
            <div class='status-indicator'>
                <div class='status-dot' style='background-color: [COLOR_STATUS]'></div>
                <span>Light is <strong>[LIGHT_STATE]</strong></span>
            </div>
            <div class='time-display'>[CURRENT_TIME]</div>
        </div>

        <div class='card-grid'>
            <div class='section'>
                <h2><span class='icon'>☁️</span> Cloud Coverage</h2>
                <div id='cloud-emoji' class='status-emoji'>&#x2601;</div>
                
                <div class='cloud-meter'>
                    <div class='cloud-meter-marker' id='cloud-marker'></div>
                </div>
                
                <form action='/cloudcheck' method='get' id='cloud-form'>
                    <div class="cloudcoverage-display">
                        <input type='number' name='cloudCoverage' min='0' max='100' value='[CLOUD_COVERAGE]' readonly>
                        <span>%</span>
                    </div>
                    <button type='submit' class='button'>
                        <span class='icon'>🔄</span> Refresh
                        <div class='loader' id='cloud-loader'></div>
                    </button>
                </form>
                <div id='cloud-status' class='sleep-status'>[CLOUD_STATUS]</div>
            </div>

            <div class='section'>
                <h2><span class='icon'>🔌</span> Control</h2>
                <p>Manually override automatic light control</p>
                <form action='/toggle' method='get' id='toggle-form'>
                    <button type='submit' class='button' id='toggle-btn'>
                        <span class='icon'>[BUTTON_ICON]</span> [BUTTON_TEXT]
                        <div class='loader' id='toggle-loader'></div>
                    </button>
                </form>
            </div>
        </div>

        <div class='section'>
            <h2><span class='icon'>⏰</span> Time Settings</h2>
            <form action='/settime' method='get' id='time-form'>
                <div class="time-input-group">
                    <label>Sunrise:</label>
                    <input type='number' name='sunriseHour' min='0' max='23' value='[SUNRISE_HOUR]'>
                    <span class='time-separator'>:</span>
                    <input type='number' name='sunriseMinute' min='0' max='59' value='[SUNRISE_MINUTE]'>
                </div>
                <div class="time-input-group">
                    <label>Sunset:</label>
                    <input type='number' name='sunsetHour' min='0' max='23' value='[SUNSET_HOUR]'>
                    <span class='time-separator'>:</span>
                    <input type='number' name='sunsetMinute' min='0' max='59' value='[SUNSET_MINUTE]'>
                </div>
                <div class='button-group'>
                    <button type='submit' class='button'>
                        <span class='icon'>💾</span> Save Settings
                        <div class='loader' id='save-loader'></div>
                    </button>
                    
                    <button type='button' class='button' style='background-color: #475569;' id='reset-btn'>
                        <span class='icon'>↩️</span> Reset
                    </button>
                </div>
            </form>
        </div>
        
        <div class='footer'>
        Made with ❤️
        </div>
    </div>

    <script>
        function updateCloudEmoji(coverage) {
            const cloudEmoji = document.getElementById('cloud-emoji');
            const cloudMarker = document.getElementById('cloud-marker');
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
                
                // Update cloud marker position
                if (coverage >= 0) {
                    const position = (coverage / 100) * 100;
                    cloudMarker.style.left = `${position}%`;
                }
            }

            cloudEmoji.innerHTML = emoji;
            document.getElementById('cloud-status').textContent = status;
        }

        // Handle form submissions with loading indicators
        document.querySelectorAll('form').forEach(form => {
            form.addEventListener('submit', function(e) {
                // Show loading indicator
                const loaderId = this.id.replace('form', 'loader');
                const loader = document.getElementById(loaderId);
                if (loader) {
                    loader.style.display = 'inline-block';
                    
                    // Find the button in this form and disable it
                    const button = this.querySelector('button');
                    if (button) {
                        button.disabled = true;
                    }
                }
            });
        });

        // Reset button functionality
        document.getElementById('reset-btn').addEventListener('click', function() {
            if (confirm('Reset all settings to default?')) {
                // You can implement actual reset logic here
                location.reload();
            }
        });

        // Initial update
        updateCloudEmoji([CLOUD_COVERAGE]);

        // Add button icon based on state
        const toggleBtn = document.getElementById('toggle-btn');
        const buttonIcon = '[BUTTON_TEXT]'.includes('ON') ? '🔆' : '🌙';
        toggleBtn.querySelector('.icon').innerHTML = buttonIcon;

        // Auto refresh every 5 minutes
        setInterval(() => {
            document.getElementById('cloud-loader').style.display = 'inline-block';
            fetch('/cloudcheck')
                .then(response => response.text())
                .then(() => {
                    location.reload();
                })
                .catch(error => {
                    console.error('Error:', error);
                    document.getElementById('cloud-loader').style.display = 'none';
                });
        }, 300000);
    </script>
</body>
</html>
)rawliteral";

#endif