🌍 IoT Air Pollution Monitoring System

An IoT-based real-time air pollution monitoring system that collects environmental data using an ESP8266 NodeMCU and multiple sensors, processes the data through a Flask backend, stores historical readings using SQLite, and presents the results through an interactive web dashboard.

The system is designed to monitor air-quality parameters, visualize pollution trends, track the monitoring location, store historical sensor readings, and provide alerts when pollution levels become high.

---

🚀 Features

- 📊 Real-time air quality monitoring
- 🌫️ PM2.5 monitoring
- 📈 AQI calculation and monitoring
- 🧪 MQ135 gas-level monitoring
- 🌡️ Temperature and humidity monitoring using DHT11
- 📍 GPS-based location tracking
- 🗺️ Interactive map-based location display
- 📉 Real-time AQI trend visualization
- 🗄️ Historical sensor-data logging
- 💾 SQLite database storage
- 📁 CSV data export
- 🔮 AQI trend forecasting
- 🚦 Public roadside AQI display
- 🔔 Buzzer alert for high pollution levels
- 🌐 Web-based monitoring dashboard

---

🛠️ Hardware

- ESP8266 NodeMCU
- MQ135 Gas Sensor
- DHT11 Temperature & Humidity Sensor
- PM2.5 / Dust Sensor
- GPS Module
- Buzzer
- Connecting wires
- Supporting electronic components

---

💻 Software Technologies

Embedded / IoT
- C++
- Arduino IDE
- ESP8266 Wi-Fi

Backend
- Python
- Flask
- SQLite
- Pandas
- Requests

Frontend
- HTML
- CSS
- JavaScript
- Chart.js
- Leaflet.js
- OpenStreetMap

Development Tools
- Git
- GitHub
- VS Code / Arduino IDE

---

🏗️ System Architecture

```text
┌─────────────────────────────┐
│          Sensors            │
│                             │
│  MQ135 │ PM2.5 │ DHT11     │
│         │ GPS   │ Buzzer   │
└──────────────┬──────────────┘
               │
               ▼
┌─────────────────────────────┐
│      ESP8266 NodeMCU        │
│                             │
│   Sensor Data Collection    │
│   Processing & Wi-Fi        │
└──────────────┬──────────────┘
               │
               │ Wi-Fi
               ▼
┌─────────────────────────────┐
│       Flask Backend         │
│                             │
│  Data Retrieval             │
│  AQI Processing             │
│  API Endpoints              │
└──────────────┬──────────────┘
               │
               ▼
┌─────────────────────────────┐
│       SQLite Database       │
│                             │
│  Current & Historical Data  │
└──────────────┬──────────────┘
               │
               ▼
┌─────────────────────────────┐
│       Web Dashboard         │
│                             │
│  AQI │ Trends │ Maps        │
│  Logs │ Forecast │ Alerts   │
└─────────────────────────────┘
