#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <LittleFS.h>

// WiFi Credentials
const char* ssid = "Matrix";           // Replace with your WiFi SSID
const char* password = "matrix123";     // Replace with your WiFi password

// Pin Definitions
const int BLUE1_PIN = D1;   // First blue row
const int BLUE2_PIN = D2;   // Second blue row
const int RED1_PIN = D3;    // First red row
const int RED2_PIN = D4;    // Second red row
const int GREEN1_PIN = D5;  // First green row
const int GREEN2_PIN = D6;  // Second green row
const int TOUCH_PIN = 9;    // SD2 pin for touch sensor
const int BUZZER_PIN = 15;  // SD0 pin for buzzer

// Constants
const int MAX_STAGES = 10;
const int NUM_ROWS = 6;
const int MAX_PATTERNS = 30;  // Maximum number of patterns (20 predefined + space for custom)
const int STAGE_DURATION = 500;  // Duration of each stage in milliseconds
const int DEBOUNCE_DELAY = 50;   // Debounce time for touch sensor in milliseconds

// Pattern Structure
struct Stage {
    uint8_t rowBrightness[NUM_ROWS];
};

struct Pattern {
    char name[32];
    int numStages;
    Stage stages[MAX_STAGES];
};

// Global Variables
Pattern patterns[MAX_PATTERNS];
int currentPatternIndex = 0;
int currentStageIndex = 0;
unsigned long lastTouchTime = 0;
unsigned long lastPatternUpdate = 0;
bool touchState = false;
bool lastTouchState = false;

// Web Server
ESP8266WebServer server(80);

// Predefined Patterns
void initializePredefinedPatterns() {
    // Pattern 1: "Aurora Cascade"
    strcpy(patterns[0].name, "Aurora Cascade");
    patterns[0].numStages = 4;
    // Stage 1
    patterns[0].stages[0] = {.rowBrightness = {255, 0, 0, 0, 0, 0}};
    patterns[0].stages[1] = {.rowBrightness = {128, 255, 0, 0, 0, 0}};
    patterns[0].stages[2] = {.rowBrightness = {0, 128, 255, 0, 0, 0}};
    patterns[0].stages[3] = {.rowBrightness = {0, 0, 128, 255, 0, 0}};

    // Pattern 2: "Neon Pulse"
    strcpy(patterns[1].name, "Neon Pulse");
    patterns[1].numStages = 3;
    patterns[1].stages[0] = {.rowBrightness = {255, 255, 0, 0, 0, 0}};
    patterns[1].stages[1] = {.rowBrightness = {0, 0, 255, 255, 0, 0}};
    patterns[1].stages[2] = {.rowBrightness = {0, 0, 0, 0, 255, 255}};

    // Pattern 3: "Digital Rain"
    strcpy(patterns[2].name, "Digital Rain");
    patterns[2].numStages = 6;
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            patterns[2].stages[i].rowBrightness[j] = (i == j) ? 255 : 0;
        }
    }

    // Pattern 4: "Mystic Waves"
    strcpy(patterns[3].name, "Mystic Waves");
    patterns[3].numStages = 4;
    patterns[3].stages[0] = {.rowBrightness = {128, 255, 128, 0, 0, 0}};
    patterns[3].stages[1] = {.rowBrightness = {0, 128, 255, 128, 0, 0}};
    patterns[3].stages[2] = {.rowBrightness = {0, 0, 128, 255, 128, 0}};
    patterns[3].stages[3] = {.rowBrightness = {0, 0, 0, 128, 255, 128}};

    // Pattern 5: "Rainbow Flow"
    strcpy(patterns[4].name, "Rainbow Flow");
    patterns[4].numStages = 6;
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            patterns[4].stages[i].rowBrightness[j] = ((i + j) % 6 == 0) ? 255 : 0;
        }
    }

    // Pattern 6: "Starlight Twinkle"
    strcpy(patterns[5].name, "Starlight Twinkle");
    patterns[5].numStages = 4;
    patterns[5].stages[0] = {.rowBrightness = {255, 128, 64, 32, 16, 8}};
    patterns[5].stages[1] = {.rowBrightness = {128, 255, 128, 64, 32, 16}};
    patterns[5].stages[2] = {.rowBrightness = {64, 128, 255, 128, 64, 32}};
    patterns[5].stages[3] = {.rowBrightness = {32, 64, 128, 255, 128, 64}};

    // Pattern 7: "Ocean Waves"
    strcpy(patterns[6].name, "Ocean Waves");
    patterns[6].numStages = 5;
    patterns[6].stages[0] = {.rowBrightness = {255, 192, 0, 0, 0, 0}};
    patterns[6].stages[1] = {.rowBrightness = {192, 255, 192, 0, 0, 0}};
    patterns[6].stages[2] = {.rowBrightness = {128, 192, 255, 192, 0, 0}};
    patterns[6].stages[3] = {.rowBrightness = {64, 128, 192, 255, 192, 0}};
    patterns[6].stages[4] = {.rowBrightness = {32, 64, 128, 192, 255, 192}};

    // Pattern 8: "Fire Dance"
    strcpy(patterns[7].name, "Fire Dance");
    patterns[7].numStages = 4;
    patterns[7].stages[0] = {.rowBrightness = {0, 0, 255, 128, 0, 0}};
    patterns[7].stages[1] = {.rowBrightness = {0, 0, 128, 255, 64, 0}};
    patterns[7].stages[2] = {.rowBrightness = {0, 0, 64, 128, 255, 128}};
    patterns[7].stages[3] = {.rowBrightness = {0, 0, 255, 64, 128, 255}};

    // Pattern 9: "Matrix Code"
    strcpy(patterns[8].name, "Matrix Code");
    patterns[8].numStages = 6;
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            patterns[8].stages[i].rowBrightness[j] = (j <= i) ? random(64, 255) : 0;
        }
    }

    // Pattern 10: "Heartbeat"
    strcpy(patterns[9].name, "Heartbeat");
    patterns[9].numStages = 4;
    patterns[9].stages[0] = {.rowBrightness = {0, 0, 255, 255, 0, 0}};
    patterns[9].stages[1] = {.rowBrightness = {0, 0, 128, 128, 0, 0}};
    patterns[9].stages[2] = {.rowBrightness = {0, 0, 255, 255, 0, 0}};
    patterns[9].stages[3] = {.rowBrightness = {0, 0, 0, 0, 0, 0}};

    // Pattern 11: "Color Symphony"
    strcpy(patterns[10].name, "Color Symphony");
    patterns[10].numStages = 6;
    patterns[10].stages[0] = {.rowBrightness = {255, 255, 0, 0, 0, 0}};
    patterns[10].stages[1] = {.rowBrightness = {128, 128, 255, 255, 0, 0}};
    patterns[10].stages[2] = {.rowBrightness = {64, 64, 128, 128, 255, 255}};
    patterns[10].stages[3] = {.rowBrightness = {0, 0, 64, 64, 128, 128}};
    patterns[10].stages[4] = {.rowBrightness = {0, 0, 0, 0, 64, 64}};
    patterns[10].stages[5] = {.rowBrightness = {0, 0, 0, 0, 255, 255}};

    // Pattern 12: "Binary Counter"
    strcpy(patterns[11].name, "Binary Counter");
    patterns[11].numStages = 8;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 6; j++) {
            patterns[11].stages[i].rowBrightness[j] = (i & (1 << j)) ? 255 : 0;
        }
    }

    // Pattern 13: "Gentle Breeze"
    strcpy(patterns[12].name, "Gentle Breeze");
    patterns[12].numStages = 5;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 6; j++) {
            patterns[12].stages[i].rowBrightness[j] = 128 + 127 * sin(2 * PI * (i + j) / 10.0);
        }
    }

    // Pattern 14: "Northern Lights"
    strcpy(patterns[13].name, "Northern Lights");
    patterns[13].numStages = 6;
    patterns[13].stages[0] = {.rowBrightness = {255, 128, 0, 0, 64, 32}};
    patterns[13].stages[1] = {.rowBrightness = {128, 255, 64, 0, 32, 64}};
    patterns[13].stages[2] = {.rowBrightness = {64, 128, 255, 64, 0, 128}};
    patterns[13].stages[3] = {.rowBrightness = {32, 64, 128, 255, 64, 0}};
    patterns[13].stages[4] = {.rowBrightness = {0, 32, 64, 128, 255, 64}};
    patterns[13].stages[5] = {.rowBrightness = {64, 0, 32, 64, 128, 255}};

    // Pattern 15: "Cyber Pulse"
    strcpy(patterns[14].name, "Cyber Pulse");
    patterns[14].numStages = 4;
    patterns[14].stages[0] = {.rowBrightness = {255, 0, 255, 0, 255, 0}};
    patterns[14].stages[1] = {.rowBrightness = {0, 255, 0, 255, 0, 255}};
    patterns[14].stages[2] = {.rowBrightness = {128, 128, 128, 128, 128, 128}};
    patterns[14].stages[3] = {.rowBrightness = {0, 0, 0, 0, 0, 0}};

    // Pattern 16: "Rainbow Chase"
    strcpy(patterns[15].name, "Rainbow Chase");
    patterns[15].numStages = 6;
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            patterns[15].stages[i].rowBrightness[j] = ((i + j) % 3 == 0) ? 255 : 0;
        }
    }

    // Pattern 17: "Morse Code SOS"
    strcpy(patterns[16].name, "Morse Code SOS");
    patterns[16].numStages = 8;
    // S = ... (3 short)
    patterns[16].stages[0] = {.rowBrightness = {255, 255, 255, 255, 255, 255}};
    patterns[16].stages[1] = {.rowBrightness = {0, 0, 0, 0, 0, 0}};
    // O = --- (3 long)
    patterns[16].stages[2] = {.rowBrightness = {255, 255, 255, 255, 255, 255}};
    patterns[16].stages[3] = {.rowBrightness = {255, 255, 255, 255, 255, 255}};
    patterns[16].stages[4] = {.rowBrightness = {0, 0, 0, 0, 0, 0}};
    // S = ... (3 short)
    patterns[16].stages[5] = {.rowBrightness = {255, 255, 255, 255, 255, 255}};
    patterns[16].stages[6] = {.rowBrightness = {0, 0, 0, 0, 0, 0}};
    patterns[16].stages[7] = {.rowBrightness = {0, 0, 0, 0, 0, 0}};

    // Pattern 18: "Fibonacci Sequence"
    strcpy(patterns[17].name, "Fibonacci Sequence");
    patterns[17].numStages = 6;
    int fib[6] = {1, 1, 2, 3, 5, 8};
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) {
            patterns[17].stages[i].rowBrightness[j] = (j < fib[i]) ? 255 : 0;
        }
    }

    // Pattern 19: "Color Meditation"
    strcpy(patterns[18].name, "Color Meditation");
    patterns[18].numStages = 5;
    patterns[18].stages[0] = {.rowBrightness = {255, 255, 0, 0, 0, 0}};
    patterns[18].stages[1] = {.rowBrightness = {128, 128, 255, 255, 0, 0}};
    patterns[18].stages[2] = {.rowBrightness = {64, 64, 128, 128, 255, 255}};
    patterns[18].stages[3] = {.rowBrightness = {32, 32, 64, 64, 128, 128}};
    patterns[18].stages[4] = {.rowBrightness = {16, 16, 32, 32, 64, 64}};

    // Pattern 20: "Quantum Entanglement"
    strcpy(patterns[19].name, "Quantum Entanglement");
    patterns[19].numStages = 4;
    patterns[19].stages[0] = {.rowBrightness = {255, 0, 0, 255, 0, 255}};
    patterns[19].stages[1] = {.rowBrightness = {0, 255, 255, 0, 255, 0}};
    patterns[19].stages[2] = {.rowBrightness = {255, 255, 0, 0, 255, 255}};
    patterns[19].stages[3] = {.rowBrightness = {0, 0, 255, 255, 0, 0}};
}

// Core Functions
void updateLEDs(const uint8_t* brightness) {
    analogWrite(BLUE1_PIN, brightness[0]);
    analogWrite(BLUE2_PIN, brightness[1]);
    analogWrite(RED1_PIN, brightness[2]);
    analogWrite(RED2_PIN, brightness[3]);
    analogWrite(GREEN1_PIN, brightness[4]);
    analogWrite(GREEN2_PIN, brightness[5]);
}


void runCurrentPattern() {
    if (millis() - lastPatternUpdate >= STAGE_DURATION) {
        Pattern& currentPattern = patterns[currentPatternIndex];
        updateLEDs(currentPattern.stages[currentStageIndex].rowBrightness);
        currentStageIndex = (currentStageIndex + 1) % currentPattern.numStages;
        lastPatternUpdate = millis();
    }
}

void handleTouch() {
    static unsigned long lastDebounceTime = 0;
    static bool lastReading = false;
    static int consecutiveReadings = 0;
    const int REQUIRED_CONSECUTIVE = 3;  // Number of consistent readings required
    
    bool reading = digitalRead(TOUCH_PIN);
    unsigned long currentTime = millis();
    
    // If the reading has changed due to noise or touch
    if (reading != lastReading) {
        lastDebounceTime = currentTime;
        consecutiveReadings = 0;
    } else if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
        // If the reading has been consistent for DEBOUNCE_DELAY
        consecutiveReadings++;
        
        // If we have enough consecutive consistent readings
        if (consecutiveReadings >= REQUIRED_CONSECUTIVE) {
            // If the touch state has actually changed
            if (reading != touchState) {
                touchState = reading;
                
                if (touchState == HIGH) {
                    Serial.println("Touch sensor activated");
                    beepBuzzer();  // Provide immediate feedback
                    nextPattern(); // Switch to next pattern
                    
                    // Reset consecutive readings to prevent multiple triggers
                    consecutiveReadings = 0;
                }
            }
        }
    }
    
    lastReading = reading;
}

void beepBuzzer() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);  // Short beep duration
    digitalWrite(BUZZER_PIN, LOW);
}

void flashLEDs() {
    // Flash all LEDs three times
    for(int i = 0; i < 3; i++) {
        // Turn all LEDs on
        analogWrite(BLUE1_PIN, 255);
        analogWrite(BLUE2_PIN, 255);
        analogWrite(RED1_PIN, 255);
        analogWrite(RED2_PIN, 255);
        analogWrite(GREEN1_PIN, 255);
        analogWrite(GREEN2_PIN, 255);
        delay(200);
        
        // Turn all LEDs off
        analogWrite(BLUE1_PIN, 0);
        analogWrite(BLUE2_PIN, 0);
        analogWrite(RED1_PIN, 0);
        analogWrite(RED2_PIN, 0);
        analogWrite(GREEN1_PIN, 0);
        analogWrite(GREEN2_PIN, 0);
        delay(200);
    }
}

void nextPattern() {
    int previousPattern = currentPatternIndex;
    currentPatternIndex = (currentPatternIndex + 1) % MAX_PATTERNS;
    
    // Skip empty custom pattern slots
    while (currentPatternIndex >= 20 && patterns[currentPatternIndex].numStages == 0) {
        currentPatternIndex = (currentPatternIndex + 1) % MAX_PATTERNS;
        
        // If we've gone through all patterns and back to where we started
        if (currentPatternIndex == previousPattern) {
            break;
        }
    }
    
    currentStageIndex = 0;
    Serial.printf("Switching to pattern: %s (index: %d)\n", 
                 patterns[currentPatternIndex].name, 
                 currentPatternIndex);
    
    beepBuzzer();
}

// Web Server Handlers
void handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        Serial.println("ERROR: Failed to open index.html");
        server.send(404, "text/plain", "File not found");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

void handleSelectPattern() {
    if (!server.hasArg("pattern")) {
        Serial.println("ERROR: No pattern index provided");
        server.send(400, "application/json", "{\"error\": \"No pattern specified\"}");
        return;
    }

    int patternIndex = server.arg("pattern").toInt();
    if (patternIndex < 0 || patternIndex >= MAX_PATTERNS) {
        Serial.printf("ERROR: Invalid pattern index: %d\n", patternIndex);
        server.send(400, "application/json", "{\"error\": \"Invalid pattern index\"}");
        return;
    }

    // Check if pattern at index exists
    if (patternIndex >= 20 && patterns[patternIndex].numStages == 0) {
        Serial.printf("ERROR: No pattern exists at index: %d\n", patternIndex);
        server.send(404, "application/json", "{\"error\": \"Pattern not found\"}");
        return;
    }

    currentPatternIndex = patternIndex;
    currentStageIndex = 0;
    beepBuzzer();
    
    Serial.printf("Pattern selected successfully: %s (index: %d)\n", 
                 patterns[patternIndex].name, patternIndex);
    
    server.send(200, "application/json", 
               "{\"message\": \"Pattern selected successfully\", "
               "\"name\": \"" + String(patterns[patternIndex].name) + "\"}");
}

void handleSavePattern() {
    if (!server.hasArg("plain")) {
        Serial.println("ERROR: No data received in save pattern request");
        server.send(400, "application/json", "{\"error\": \"No data received\"}");
        return;
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (error) {
        Serial.print("ERROR: JSON parsing failed: ");
        Serial.println(error.c_str());
        server.send(400, "application/json", 
                   "{\"error\": \"Invalid JSON: " + String(error.c_str()) + "\"}");
        return;
    }

    // Validate required fields
    if (!doc.containsKey("name") || !doc.containsKey("stages")) {
        Serial.println("ERROR: Missing required fields in pattern data");
        server.send(400, "application/json", 
                   "{\"error\": \"Missing required fields: name and stages\"}");
        return;
    }

    JsonArray stages = doc["stages"];
    if (stages.size() == 0 || stages.size() > MAX_STAGES) {
        Serial.printf("ERROR: Invalid number of stages: %d\n", stages.size());
        server.send(400, "application/json", 
                   "{\"error\": \"Invalid number of stages (1-" + String(MAX_STAGES) + ")\"}");
        return;
    }

    // Find next available pattern slot
    int newPatternIndex = -1;
    for (int i = 20; i < MAX_PATTERNS; i++) {
        if (patterns[i].numStages == 0) {
            newPatternIndex = i;
            break;
        }
    }

    if (newPatternIndex == -1) {
        Serial.println("ERROR: No space available for new patterns");
        server.send(507, "application/json", "{\"error\": \"No space for new patterns\"}");
        return;
    }

    // Save new pattern
    Pattern& newPattern = patterns[newPatternIndex];
    const char* patternName = doc["name"] | "Custom Pattern";
    strlcpy(newPattern.name, patternName, sizeof(newPattern.name));
    newPattern.numStages = stages.size();

    // Validate and save stage data
    for (int i = 0; i < newPattern.numStages; i++) {
        JsonArray brightness = stages[i];
        if (brightness.size() != NUM_ROWS) {
            Serial.printf("ERROR: Invalid number of brightness values in stage %d\n", i);
            server.send(400, "application/json", 
                       "{\"error\": \"Invalid number of brightness values in stage " + 
                       String(i) + "\"}");
            // Reset pattern data
            newPattern.numStages = 0;
            return;
        }

        for (int j = 0; j < NUM_ROWS; j++) {
            int value = brightness[j] | 0;
            if (value < 0 || value > 255) {
                Serial.printf("ERROR: Invalid brightness value %d in stage %d, row %d\n", 
                            value, i, j);
                server.send(400, "application/json", 
                           "{\"error\": \"Invalid brightness value in stage " + 
                           String(i) + ", row " + String(j) + "\"}");
                // Reset pattern data
                newPattern.numStages = 0;
                return;
            }
            newPattern.stages[i].rowBrightness[j] = value;
        }
    }

    Serial.printf("Pattern saved successfully: %s (index: %d)\n", patternName, newPatternIndex);
    
    flashLEDs();
    beepBuzzer();
    
    server.send(201, "application/json", 
               "{\"message\": \"Pattern saved successfully\", "
               "\"index\": " + String(newPatternIndex) + ", "
               "\"name\": \"" + String(patternName) + "\"}");
}

void setupWebServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/select", HTTP_GET, handleSelectPattern);
    server.on("/save", HTTP_POST, handleSavePattern);
    
    // Serve static files
    server.serveStatic("/", LittleFS, "/");
}

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    Serial.println("\nInitializing LED Matrix Controller...");
    
    // Initialize pins
    Serial.println("Configuring GPIO pins...");
    pinMode(BLUE1_PIN, OUTPUT);
    pinMode(BLUE2_PIN, OUTPUT);
    pinMode(RED1_PIN, OUTPUT);
    pinMode(RED2_PIN, OUTPUT);
    pinMode(GREEN1_PIN, OUTPUT);
    pinMode(GREEN2_PIN, OUTPUT);
    pinMode(TOUCH_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);  // Ensure buzzer is off initially
    
    // Test all LEDs and buzzer
    Serial.println("Testing hardware components...");
    flashLEDs();
    beepBuzzer();
    
    // Initialize patterns
    Serial.println("Loading predefined patterns...");
    initializePredefinedPatterns();
    
    // Initialize WiFi with improved feedback
    Serial.println("Connecting to WiFi...");
    WiFi.mode(WIFI_STA);  // Set WiFi to station mode
    WiFi.begin(ssid, password);
    
    int wifiTimeout = 0;
    bool connected = false;
    
    // Try to connect for 20 seconds (40 attempts, 500ms each)
    while (wifiTimeout < 40) {
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            break;
        }
        delay(500);
        Serial.print(".");
        // Flash blue LED to indicate connection attempt
        analogWrite(BLUE1_PIN, (wifiTimeout % 2) * 255);
        wifiTimeout++;
    }
    
    if (!connected) {
        Serial.println("\nWiFi connection failed! Check credentials or WiFi signal.");
        // Flash red LEDs to indicate WiFi failure
        for(int i = 0; i < 3; i++) {
            analogWrite(RED1_PIN, 255);
            analogWrite(RED2_PIN, 255);
            delay(500);
            analogWrite(RED1_PIN, 0);
            analogWrite(RED2_PIN, 0);
            delay(500);
        }
    } else {
        Serial.println("\nConnected to WiFi successfully!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        
        // Flash green LEDs to indicate successful connection
        for(int i = 0; i < 2; i++) {
            analogWrite(GREEN1_PIN, 255);
            analogWrite(GREEN2_PIN, 255);
            delay(200);
            analogWrite(GREEN1_PIN, 0);
            analogWrite(GREEN2_PIN, 0);
            delay(200);
        }
    }
    
    // Initialize file system with error handling
    Serial.println("Mounting file system...");
    if (!LittleFS.begin()) {
        Serial.println("ERROR: Failed to mount file system!");
        // Flash all LEDs to indicate file system error
        for(int i = 0; i < 3; i++) {
            uint8_t allOn[6] = {255, 255, 255, 255, 255, 255};
            updateLEDs(allOn);
            delay(200);
            uint8_t allOff[6] = {0, 0, 0, 0, 0, 0};
            updateLEDs(allOff);
            delay(200);
        }
    } else {
        Serial.println("File system mounted successfully!");
    }
    
    // Setup web server routes
    setupWebServer();
    
    server.begin();
}

void loop() {
    server.handleClient();
    handleTouch();
    runCurrentPattern();
}
