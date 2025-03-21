#include <WiFi.h>
#include <PubSubClient.h>
#include <TinyGPS++.h>
#include <HTTPClient.h>

// WiFi Credentials
const char* ssid = "IEP";
const char* password = "12345678";

// MQTT Configuration
const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_topic = "esp32/location";
String client_id = "ESP32Client-" + String(random(0xffff), HEX);

// GPS Configuration
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

// WiFi and MQTT Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Function to connect to WiFi
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

// Function to reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(client_id.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Function to get location name using reverse geocoding
String getLocationName(double latitude, double longitude) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.positionstack.com/v1/reverse?access_key=YOUR_ACCESS_KEY&query=" + String(latitude, 6) + "," + String(longitude, 6);
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      // Parse the JSON payload to extract the location name
      // This is a placeholder; you'll need to parse the JSON response appropriately
      String locationName = "Parsed Location Name";
      return locationName;
    } else {
      Serial.println("Error on HTTP request");
      return "Unknown";
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
    return "Unknown";
  }
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
    if (gps.location.isUpdated()) {
      double latitude = gps.location.lat();
      double longitude = gps.location.lng();
      String locationName = getLocationName(latitude, longitude);

      String payload = "{";
      payload += "\"latitude\": " + String(latitude, 6) + ",";
      payload += "\"longitude\": " + String(longitude, 6) + ",";
      payload += "\"location\": \"" + locationName + "\"";
      payload += "}";

      client.publish(mqtt_topic, payload.c_str());
      Serial.println("Published: " + payload);
    }
  }
}
