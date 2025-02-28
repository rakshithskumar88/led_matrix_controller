# ESP8266 LED Matrix Controller

A web-controlled LED matrix system using ESP8266 with touch sensor input and buzzer feedback.

## Hardware Requirements

- ESP8266 (NodeMCU)
- LED Matrix (6 rows):
  - Blue rows (D1, D2)
  - Red rows (D3, D4)
  - Green rows (D5, D6)
- Touch Sensor (SD2)
- Buzzer (SD0)

## Software Requirements

- PlatformIO IDE
- Arduino framework
- ESP8266 board support
- Required libraries:
  - ESP8266WiFi
  - ESP8266WebServer
  - ArduinoJson
  - LittleFS

## Setup Instructions

1. Clone this repository
2. Open the project in PlatformIO IDE
3. Update WiFi credentials in `led_matrix_controller.ino`:
   ```cpp
   const char* ssid = "Matrix";      // Your WiFi SSID
   const char* password = "matrix123"; // Your WiFi password
   ```

4. Build and Upload:
   - The main program will be uploaded automatically
   - The filesystem (containing web interface files) will be built and uploaded automatically

## Features

1. LED Patterns:
   - 20 predefined patterns with creative animations
   - Custom pattern creation through web interface
   - Pattern switching via touch sensor
   - Visual feedback through LED animations
   - Audio feedback through buzzer

2. Web Interface:
   - Pattern Selector tab
     - List of all available patterns
     - Radio button selection
   - Pattern Creator tab
     - Create custom patterns stage by stage
     - Circular brightness sliders for each LED row
     - Save and apply new patterns

## Usage

1. Power up the ESP8266
2. Connect to the configured WiFi network
3. Access the web interface at the ESP8266's IP address (shown in Serial Monitor)
4. Use touch sensor to cycle through patterns
5. Create custom patterns through the web interface

## File Structure

```
├── led_matrix_controller.ino  # Main Arduino code
├── platformio.ini             # PlatformIO configuration
├── data/                      # Web interface files
│   ├── index.html            # Main HTML file
│   ├── style.css             # CSS styles
│   └── script.js             # JavaScript code
└── scripts/
    └── build_littlefs.py     # LittleFS build script
```

## Troubleshooting

If the web interface shows "File not found":
1. Ensure the data files are properly uploaded to LittleFS
2. Check Serial Monitor for filesystem mounting status
3. Verify that all required files exist in the data directory

## LED Pattern Names

1. Aurora Cascade
2. Neon Pulse
3. Digital Rain
4. Mystic Waves
5. Rainbow Flow
6. Starlight Twinkle
7. Ocean Waves
8. Fire Dance
9. Matrix Code
10. Heartbeat
11. Color Symphony
12. Binary Counter
13. Gentle Breeze
14. Northern Lights
15. Cyber Pulse
16. Rainbow Chase
17. Morse Code SOS
18. Fibonacci Sequence
19. Color Meditation
20. Quantum Entanglement
