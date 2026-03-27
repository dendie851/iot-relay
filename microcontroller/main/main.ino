#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>

// --- Configuration ---
#define RELAY1 18             // GPIO Pin for Relay
const int RESET_BTN_PIN = 0;  // BOOT button to reset config

Preferences pref;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
AsyncWebServer server(80);

String ssid, pass, mqtt_ip;

// --- MQTT Callback: Receives ON/OFF commands ---
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived ["); Serial.print(topic); Serial.print("] ");
    
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
        Serial.println("JSON Parse Failed");
        return;
    }

    int status = doc["status"]; // Extracts 0 or 1
    
    // Strict check for the correct topic
    if (String(topic) == "-/dev1/relay1") {
        digitalWrite(RELAY1, status);
        Serial.println(status == 1 ? "-> RELAY1 ON" : "-> RELAY1 OFF");
    }
}

// --- Setup Mode (AP) ---
void startAP() {
    WiFi.mode(WIFI_AP);
    // Updated Name as requested
    WiFi.softAP("ESP32_Dev1_Relay1", "12345678"); 
    Serial.println("AP Mode: Connect to 'ESP32_Dev1_Relay1' to configure device.");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = "<html><body style='font-family:sans-serif; text-align:center; padding:50px;'>";
        html += "<h2>Device 1 Relay 1 Setup</h2><form action='/save' method='POST'>";
        html += "WiFi SSID:<br><input type='text' name='s' required><br><br>";
        html += "WiFi Password:<br><input type='password' name='p' required><br><br>";
        html += "MQTT Server IP:<br><input type='text' name='m' placeholder='192.168.100.35' required><br><br>";
        html += "<button type='submit' style='padding:10px 20px;'>SAVE & CONNECT</button></form></body></html>";
        request->send(200, "text/html", html);
    });

    server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
        pref.begin("config", false);
        pref.putString("ssid", request->arg("s"));
        pref.putString("pass", request->arg("p"));
        pref.putString("mqtt", request->arg("m"));
        pref.end();
        request->send(200, "text/plain", "Configuration Saved. ESP32 is restarting...");
        delay(2000); ESP.restart();
    });
    server.begin();
}

void setup() {
    Serial.begin(115200);
    pinMode(RESET_BTN_PIN, INPUT_PULLUP);
    pinMode(RELAY1, OUTPUT); 
    digitalWrite(RELAY1, LOW); // Ensure relay is OFF on boot

    pref.begin("config", true);
    ssid = pref.getString("ssid", "");
    pass = pref.getString("pass", "");
    mqtt_ip = pref.getString("mqtt", "");
    pref.end();

    if (ssid == "" || mqtt_ip == "") {
        startAP();
    } else {
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.print("Connecting to WiFi");
        unsigned long startWait = millis();
        // 15 seconds timeout for WiFi
        while (WiFi.status() != WL_CONNECTED && (millis() - startWait < 15000)) { 
            delay(500); 
            Serial.print("."); 
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi Connected!");
            mqttClient.setServer(mqtt_ip.c_str(), 1883);
            mqttClient.setCallback(callback);
        } else {
            Serial.println("\nWiFi Connection Failed. Entering Setup Mode...");
            startAP();
        }
    }
}

void loop() {
    // Factory Reset: Hold BOOT button for 3 seconds to clear WiFi/MQTT settings
    if (digitalRead(RESET_BTN_PIN) == LOW) {
        static unsigned long hold = 0;
        if (hold == 0) hold = millis();
        if (millis() - hold > 3000) {
            pref.begin("config", false); pref.clear(); pref.end();
            Serial.println("Memory Cleared. Restarting...");
            ESP.restart();
        }
    } else { hold = 0; }

    if (WiFi.status() == WL_CONNECTED) {
        if (!mqttClient.connected()) {
            static unsigned long lastRetry = 0;
            if (millis() - lastRetry > 5000) {
                lastRetry = millis();
                // Unique Client ID
                String clientId = "ESP32_Dev1_R1_" + String(random(0xffff), HEX);
                if (mqttClient.connect(clientId.c_str())) {
                    Serial.println("MQTT Connected!");
                    mqttClient.subscribe("-/dev1/relay1");
                }
            }
        }
        mqttClient.loop();
    }
}