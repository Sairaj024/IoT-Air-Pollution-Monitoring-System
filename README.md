# 🌍 IoT Air Pollution Monitoring System

An **IoT-based real-time air quality monitoring system** using **NodeMCU (ESP8266)** and **MQ135 gas sensor**, designed to detect harmful gases such as **Ammonia (NH3)**, **Benzene (C6H6)**, and **Carbon Dioxide (CO2)**.  
The data is uploaded to **ThingSpeak Cloud** for visualization, while a **buzzer alert** notifies users when pollution levels exceed safe thresholds.

---

## ⚙️ Features

- 🌫️ Real-time monitoring of multiple gases (NH3, C6H6, CO2)
- ☁️ Cloud integration using ThingSpeak for live data visualization
- 🔔 Buzzer alerts when air quality crosses dangerous levels
- 📊 Graphical dashboard to view pollution trends
- 💾 Data logging for future analysis
- ⚡ Built with NodeMCU (ESP8266) and MQ135 gas sensor

---

## 🧠 Project Overview

This project addresses the growing need for **environmental monitoring** in urban areas.  
By leveraging IoT technology, this system provides **continuous real-time air quality data** that can be accessed from anywhere, helping citizens and authorities make informed decisions about air pollution control.

---

## 🧩 Components Used

| Component | Description |
|------------|-------------|
| **NodeMCU (ESP8266)** | Microcontroller with Wi-Fi support |
| **MQ135 Gas Sensor** | Detects harmful gases like NH3, Benzene, CO2 |
| **Buzzer** | Alerts when air quality is poor |
| **Jumper Wires** | For circuit connections |
| **Breadboard / PCB** | Mounting components |
| **ThingSpeak** | Cloud platform for IoT data visualization |

---

## 🧾 Working Principle

1. The MQ135 sensor continuously measures gas concentration in PPM.  
2. Data is sent to the **NodeMCU**, which processes and uploads it to **ThingSpeak** via Wi-Fi.  
3. The **ThingSpeak dashboard** displays live graphs of Ammonia, Benzene, and CO2.  
4. If the gas level exceeds a predefined threshold, the **buzzer** is activated.  

---

## 🖥️ Hardware Setup

<p align="center">
  <img src="WhatsApp%20Image%202025-10-12%20at%2009.46.48.jpeg" width="500" />
</p>

---

## 📈 Results

- Real-time readings are displayed on the **ThingSpeak dashboard**  
- Example dashboard graphs:
  
<p align="center">
  <img src="https://thingspeak.com/channels/YOUR_CHANNEL_ID/charts/1" width="400" />  
  <img src="https://thingspeak.com/channels/YOUR_CHANNEL_ID/charts/2" width="400" />  
</p>

*(Replace `YOUR_CHANNEL_ID` with your actual ThingSpeak channel ID.)*

---

## 🧪 Testing

| Parameter | Threshold (PPM) | Status |
|------------|-----------------|--------|
| Ammonia (NH3) | >250 | Buzzer ON |
| Benzene (C6H6) | >700 | Buzzer ON |
| CO2 | >7000 | Buzzer ON |

---

## 🚀 How to Run

1. Open `MB0111.ino` in Arduino IDE  
2. Install libraries:
   - `ESP8266WiFi.h`
   - `ThingSpeak.h`
3. Connect your **NodeMCU (ESP8266)** via USB  
4. Enter your **Wi-Fi SSID** and **Password** in the code  
5. Upload the code to the NodeMCU  
6. Open **Serial Monitor (9600 baud)** to view live readings  
7. Go to your **ThingSpeak channel** to view the dashboard graphs  

---

## 📚 Project Files

- `MB0111.ino` → Arduino Code  
- `Project_Synopsis_LY613_Final.pdf` → Project Synopsis  
- `FINAL_PBL_REPORT.pdf` → Final Report  
- `project_report.pptx` → Presentation Slides  
- `Ideation_&_Design.pdf` → Design Documentation  
- `Research_paper.pdf` → Research Report  

---

## 🧑‍💻 Developed By

**Sairaj Deshmukh**  
Final Year, Computer Science Engineering  
MIT ADT University, Pune  

📬 [GitHub Profile](https://github.com/Sairaj024)

---

## 🏁 Future Scope

- Integration with mobile apps for instant pollution alerts  
- AI-based analysis for air quality prediction  
- GPS-enabled location-based pollution mapping  
- Integration with government pollution control APIs  

---

## 🪪 License

This project is licensed under the **MIT License** — feel free to use, modify, and share with credit.

---

⭐ **If you like this project, don’t forget to star the repository!**
