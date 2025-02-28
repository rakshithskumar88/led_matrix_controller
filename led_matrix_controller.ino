#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <LittleFS.h>

// WiFi Credentials
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

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

void beepBuzzer() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
}

void flashLEDs() {
    for(int i = 0; i < 3; i++) {
        uint8_t allOn[6] = {255, 255, 255, 255, 255, 255};
        uint8_t allOff[6] = {0, 0, 0, 0, 0, 0};
        updateLEDs(allOn);
        delay(200);
        updateLEDs(allOff);
        delay(200);
    }
}

void nextPattern() {
    currentPatternIndex = (currentPatternIndex + 1) % MAX_PATTERNS;
    currentStageIndex = 0;
    beepBuzzer();
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
    bool reading = digitalRead(TOUCH_PIN);
    
    if (reading != lastTouchState) {
        lastTouchTime = millis();
    }
    
    if ((millis() - lastTouchTime) > DEBOUNCE_DELAY) {
        if (reading != touchState) {
            touchState = reading;
            if (touchState == HIGH) {
                nextPattern();
            }
        }
    }
    
    lastTouchState = reading;
}

// Web Server Handlers
void handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

void handleSelectPattern() {
    if (server.hasArg("pattern")) {
        int patternIndex = server.arg("pattern").toInt();
        if (patternIndex >= 0 && patternIndex < MAX_PATTERNS) {
            currentPatternIndex = patternIndex;
            currentStageIndex = 0;
            beepBuzzer();
            server.send(200, "text/plain", "Pattern selected");
        } else {
            server.send(400, "text/plain", "Invalid pattern index");
        }
    } else {
        server.send(400, "text/plain", "No pattern specified");
    }
}

void handleSavePattern() {
    if (server.hasArg("plain")) {
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, server.arg("plain"));
        
        if (error) {
            server.send(400, "text/plain", "Invalid JSON");
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
            server.send(400, "text/plain", "No space for new patterns");
            return;
        }

        // Save new pattern
        Pattern& newPattern = patterns[newPatternIndex];
        strlcpy(newPattern.name, doc["name"] | "Custom Pattern", sizeof(newPattern.name));
        JsonArray stages = doc["stages"];
        newPattern.numStages = min((int)stages.size(), MAX_STAGES);

        for (int i = 0; i < newPattern.numStages; i++) {
            JsonArray brightness = stages[i];
            for (int j = 0; j < NUM_ROWS; j++) {
                newPattern.stages[i].rowBrightness[j] = brightness[j] | 0;
            }
        }

        flashLEDs();
        beepBuzzer();
        server.send(200, "text/plain", "Pattern saved");
    } else {
        server.send(400, "text/plain", "No data received");
    }
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
    
    // Initialize pins
    pinMode(BLUE1_PIN, OUTPUT);
    pinMode(BLUE2_PIN, OUTPUT);
    pinMode(RED1_PIN, OUTPUT);
    pinMode(RED2_PIN, OUTPUT);
    pinMode(GREEN1_PIN, OUTPUT);
    pinMode(GREEN2_PIN, OUTPUT);
    pinMode(TOUCH_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    // Initialize patterns
    initializePredefinedPatterns();
    
    // Initialize WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi. IP: ");
    Serial.println(WiFi.localIP());
    
    // Initialize file system
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount file system");
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
