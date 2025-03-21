#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi Credentials
const char* ssid = "impraveena";
const char* password = "abcdefgh";

// MQTT Configuration
const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_topic = "esp32/location";
String client_id = "ESP32Subscriber-" + String(random(0xffff), HEX);

// Global variables for GPS data (updated via MQTT callback)
String latitude_data = "Waiting...";
String longitude_data = "Waiting...";

// WiFi and MQTT objects
WiFiClient espClient;
PubSubClient client(espClient);

// Web server on port 80
WiFiServer server(80);

// MQTT Message Received Callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  // Parse the JSON payload
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }
  
  // Extract GPS values and update globals
  double latitude = doc["latitude"];
  double longitude = doc["longitude"];
  // Optionally, you can extract location name:
  // const char* location = doc["location"];
  
  latitude_data = String(latitude, 6);
  longitude_data = String(longitude, 6);
  
  Serial.print("Latitude: ");
  Serial.println(latitude_data);
  Serial.print("Longitude: ");
  Serial.println(longitude_data);
}

// Connect to WiFi
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
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to MQTT broker if disconnected
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(client_id.c_str())) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
      Serial.print("Subscribed to: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

// Function to display the webpage with GPS data
void displayWebPage(WiFiClient client) {
  String page = "<!DOCTYPE html><html lang='en'><head>";
  page += "<meta charset='UTF-8'>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += "<meta http-equiv='refresh' content='5'>";
  page += "<title>ESP32 GPS Tracker</title>";
  page += "<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>";
  page += "<style>";
  page += "* { margin: 0; padding: 0; box-sizing: border-box; }";
  page += "body { font-family: 'Poppins', sans-serif; background: linear-gradient(135deg, #0f2027, #203a43, #2c5364); color: #ffffff; text-align: center; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; padding: 20px; }";
  page += "h1 { font-size: 36px; font-weight: 600; margin-bottom: 20px; text-transform: uppercase; animation: glow 1.5s infinite alternate; }";
  page += "@keyframes glow { from { text-shadow: 0 0 10px rgba(255, 255, 255, 0.5); } to { text-shadow: 0 0 20px rgba(255, 255, 255, 1); } }";
  page += ".card { background: rgba(255, 255, 255, 0.1); padding: 30px; border-radius: 15px; width: 90%; max-width: 500px; box-shadow: 0 10px 30px rgba(0, 0, 0, 0.3); transition: transform 0.3s ease-in-out; }";
  page += ".card:hover { transform: translateY(-5px); }";
  page += ".data { font-size: 24px; font-weight: bold; margin: 15px 0; }";
  page += "iframe { width: 100%; height: 300px; border: none; border-radius: 12px; margin-top: 15px; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2); }";
  page += ".footer { margin-top: 20px; font-size: 14px; opacity: 0.8; }";
  page += "</style></head><body>";
  page += "<h1>Live GPS Tracking</h1>";
  page += "<div class='card'>";
  page += "<p class='data'>Latitude: <span>" + latitude_data + "</span></p>";
  page += "<p class='data'>Longitude: <span>" + longitude_data + "</span></p>";
  page += "<iframe src='https://maps.google.com/maps?q=" + latitude_data + "," + longitude_data + "&hl=en&z=14&output=embed'></iframe>";
  page += "</div>";
  page += "<p class='footer'> Designed by Praveena | Auto-refresh every 5 seconds</p>";
  page += "</body></html>";
  
  client.print(page);
  delay(100);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Start the web server
  server.begin();
  
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Check for incoming web client connections
  WiFiClient webClient = server.available();
  if (webClient) {
    displayWebPage(webClient);
    webClient.stop();
  }
}
