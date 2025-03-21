# ESP32 GPS Tracker with MQTT & Web Server

## 📌 Project Overview
This project utilizes an **ESP32** to receive GPS coordinates, publish them via **MQTT**, and display the live location on a web page. The ESP32 acts as both an **MQTT subscriber** and a **web server**, allowing real-time GPS tracking with Google Maps integration.

---

## ⚙️ Features
- **GPS Data Collection**: Fetches real-time latitude and longitude via **TinyGPS++**.
- **MQTT Communication**: Publishes GPS data to an MQTT broker and subscribes to receive updates.
- **Web Dashboard**: Hosts a responsive webpage displaying the location with **Google Maps**.
- **Auto Refresh**: Updates the displayed coordinates every **5 seconds**.
- **WiFi Connectivity**: Connects to the internet to fetch location details via **reverse geocoding**.

---

## 🛠️ Hardware Requirements
- **ESP32 Development Board**
- **GPS Module (e.g., NEO-6M, NEO-M8N)**
- **Jumper Wires**
- **Power Source (5V/3.3V)**

---

## 📝 Software & Libraries
Ensure you have these installed in the **Arduino IDE**:
- **WiFi.h** - ESP32 WiFi library
- **PubSubClient.h** - MQTT client library
- **TinyGPS++.h** - GPS data parsing
- **ArduinoJson.h** - JSON parsing for MQTT messages
- **HTTPClient.h** - API requests for reverse geocoding

---

## 🚀 Setup & Usage
### 1️⃣ Wiring Connections (ESP32 & GPS Module)
| GPS Module Pin | ESP32 Pin |
|---------------|----------|
| VCC | 3.3V |
| GND | GND |
| TX | GPIO 16 |
| RX | GPIO 17 |

### 2️⃣ Code Setup
- Open the **ESP32 Publisher** code and update:
  ```cpp
  const char* ssid = "YOUR_WIFI_SSID";
  const char* password = "YOUR_WIFI_PASSWORD";
  const char* mqtt_server = "test.mosquitto.org";
  ```
- If using **reverse geocoding**, replace:
  ```cpp
  String url = "http://api.positionstack.com/v1/reverse?access_key=YOUR_ACCESS_KEY&query=" + String(latitude, 6) + "," + String(longitude, 6);
  ```
  with your **PositionStack API key**.
- Upload the sketch and monitor via **Serial Monitor**.

### 3️⃣ Web Server (ESP32 Subscriber)
- The subscriber receives GPS data and displays it in a **webpage**.
- Connect to the same WiFi and enter the **ESP32's IP address** in a browser to view the map.

---

## 📡 MQTT Data Format
Each GPS update is sent as a **JSON payload**:
```json
{
  "latitude": 12.345678,
  "longitude": 98.765432,
  "location": "Some Place"
}
```

---

## 🔧 Troubleshooting
### 🔹 No GPS Data?
- Ensure **GPS module** is receiving signals (place it outdoors).
- Use `Serial.print(gpsSerial.read());` to debug GPS output.

### 🔹 MQTT Not Connecting?
- Check WiFi credentials & broker status.
- Try `mqtt_client.setServer("broker.hivemq.com", 1883);` as an alternative.

### 🔹 Web Page Not Loading?
- Find the **ESP32 IP Address** using:
  ```cpp
  Serial.println(WiFi.localIP());
  ```
- Ensure **port 80** is not blocked by firewall.

---

## 📌 Future Enhancements
✅ Add **HTTPS encryption** for MQTT communication.  
✅ Implement **Firebase** for cloud-based tracking.  
✅ Use **OLED display** to show location data locally.  
✅ Improve **reverse geocoding** for better place names.

---

## 🏆 Credits
**Developed by Praveena** 💡✨  
Built with **ESP32, MQTT, and Web Technologies**. 🚀

