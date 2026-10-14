#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "index.h"

// Your Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);
WebSocketsServer webSocket(81);

// --- PIN DEFINITIONS ---
const int dacPin = 25;
const int adcPin1 = 34; // CH1: Logic Level
const int adcPin2 = 35; // CH2: High Voltage
const int adcPin3 = 32; // CH3: AC / Audio
const int ledPin = 2;   // Data TX Indicator LED

unsigned long lastTime = 0;
bool ledState = false;  // Used to blink the LED

void setup() {
  Serial.begin(115200);
  
  // --- SETUP PINS ---
  pinMode(adcPin1, INPUT);
  pinMode(adcPin2, INPUT);
  pinMode(adcPin3, INPUT);
  pinMode(ledPin, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP Address: "); Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/html", MAIN_page);
  });
  
  server.begin();
  webSocket.begin();
}

void loop() {
  server.handleClient();
  webSocket.loop(); 

  // Throttle to 20ms (50 FPS) for Wi-Fi stability
  if (millis() - lastTime > 20) {
    
    // -- GENERATE 2Hz SINE WAVE ON PIN 25 --
    // To prevent Op-Amp clipping, we limit the peak to ~3.0V (DAC value 232).
    // The wave oscillates around a center of 116, with an amplitude of 116.
    // 2Hz angular frequency math: 2 * PI * 2.0 / 1000.0 = 0.012566
    int dacValue = 116 + 116 * sin(millis() * 0.012566);
    dacWrite(dacPin, dacValue);

    // -- CAPTURE RAW DATA (Smoothing Removed for Sharp Edges!) --
    int rawADC1 = analogRead(adcPin1);
    int voltage1_mV = (rawADC1 * 3300) / 4095;

    int rawADC2 = analogRead(adcPin2);
    int voltage2_mV = (rawADC2 * 3300) / 4095;

    int rawADC3 = analogRead(adcPin3);
    int voltage3_mV = (rawADC3 * 3300) / 4095;

    // -- TOGGLE LED TO SHOW DATA IS TRANSMITTING --
    ledState = !ledState;
    digitalWrite(ledPin, ledState);

    // -- SEND PURE HARDWARE DATA TO BROWSER --
    // Format: "CH1,CH2,CH3"
    String dataString = String(voltage1_mV) + "," + 
                        String(voltage2_mV) + "," + 
                        String(voltage3_mV);
                        
    webSocket.broadcastTXT(dataString); 

    lastTime = millis();
  }
}