# IoT Air Pollution Monitoring System

An IoT-based air pollution monitoring system that collects environmental data using an ESP8266 and multiple sensors, processes the data through a Flask backend, stores historical readings, and presents the results through a web dashboard.

## Features

- Real-time air quality monitoring
- PM2.5 monitoring
- AQI calculation
- MQ135 gas-level monitoring
- Temperature and humidity monitoring using DHT11
- GPS-based location tracking
- Interactive map-based location display
- Real-time AQI trend chart
- Historical sensor-data logging
- SQLite database storage
- CSV data export
- AQI trend forecasting
- Public roadside AQI display
- Buzzer alert for high pollution levels

## Hardware

- ESP8266 NodeMCU
- MQ135 Gas Sensor
- DHT11 Temperature & Humidity Sensor
- PM2.5 / Dust Sensor
- GPS Module
- Buzzer
- Connecting wires and supporting components

## Software Technologies

- C++ / Arduino
- ESP8266 Wi-Fi
- Python
- Flask
- SQLite
- Pandas
- HTML
- CSS
- JavaScript
- Chart.js
- Leaflet.js
- OpenStreetMap

## System Architecture

```text
Sensors
   ↓
ESP8266 NodeMCU
   ↓
Wi-Fi
   ↓
Flask Backend
   ↓
SQLite Database
   ↓
Web Dashboard
   ↓
Monitoring / Trends / Alerts / Location
