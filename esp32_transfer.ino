#include <WiFi.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>

// WiFi credentials
const char* ssid = "Quàng";
const char* password = "quangcute";

// MQTT broker details
const char* mqtt_server = "192.168.213.184";
const int mqtt_port = 1883;
const char* mqtt_topic = "opencv/esp32";

#define led 2

// Buffer to hold the received message
char txt[100]; // Adjust the size as needed

// Initialize Serial communication
HardwareSerial MySerial(1);  // Using Serial1 (TX=17, RX=16)

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// WiFi connection function
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback when a message is received
void callback(char* topic, byte* message, unsigned int length) {
  
  memset(txt, 0, sizeof(txt));  // Clear the buffer

  for (unsigned int i = 0; i < length && i < sizeof(txt) - 1; i++) {
    txt[i] = (char)message[i];
  }
  Serial.println(txt);

  // Send message to Arduino via Serial as a char array
  MySerial.println(txt);  // Send the message via Serial to Arduino
}

// MQTT broker connection function
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe(mqtt_topic);
      digitalWrite(led, HIGH);
    } else {
      Serial.print(client.state());
      delay(2000);
      digitalWrite(led, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);  // Debugging via USB-Serial
  MySerial.begin(115200, SERIAL_8N1, 16, 17);  // Initialize Serial1 with TX=17, RX=16

  // Connect to WiFi
  setup_wifi();

  // Set up MQTT server and callback
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  pinMode(led, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Handle MQTT tasks
}
