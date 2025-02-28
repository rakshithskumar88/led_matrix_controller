# ESP8266 LED Matrix Controller

An interactive LED matrix controller using ESP8266 with touch sensor control and web interface.

## Features

- 20 predefined LED patterns with creative animations
- Touch sensor control with buzzer feedback
- Web interface with two main features:
  - Pattern Selector: Choose from predefined patterns
  - Pattern Creator: Create custom patterns with intuitive controls
- Real-time pattern switching
- Visual and audio feedback

## Hardware Requirements

- ESP8266 (NodeMCU)
- 6-row LED Matrix:
  - Rows 1-2: Blue LEDs
  - Rows 3-4: Red LEDs
  - Rows 5-6: Green LEDs
- Touch Sensor (connected to SD2)
- Buzzer (connected to SD0)

## Pin Configuration

- D1 -> Blue1 (First blue row)
- D2 -> Blue2 (Second blue row)
- D3 -> Red1 (First red row)
- D4 -> Red2 (Second red row)
- D5 -> Green1 (First green row)
- D6 -> Green2 (Second green row)
- SD2 -> Touch Sensor
- SD0 -> Buzzer

## Setup Instructions

1. Clone this repository
2. Open `led_matrix_controller.ino` in Arduino IDE
3. Install required libraries:
   - ESP8266WiFi
   - ESP8266WebServer
   - ArduinoJson
   - LittleFS
4. Update WiFi credentials in the sketch
5. Upload the sketch to your ESP8266
6. Upload the contents of the `data` folder using "ESP8266 Sketch Data Upload" tool

## Usage

1. Power up the ESP8266
2. Connect to the same WiFi network
3. Access the web interface using ESP8266's IP address
4. Use either:
   - Touch sensor to cycle through patterns
   - Web interface to select or create patterns

## Project Structure

- `led_matrix_controller.ino`: Main Arduino sketch
- `data/`
  - `index.html`: Web interface
  - `style.css`: CSS styling
  - `script.js`: JavaScript functionality

## Pattern List

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

## License

MIT License
