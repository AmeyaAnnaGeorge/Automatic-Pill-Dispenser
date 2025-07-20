# Automatic-Pill-Dispenser
An IoT-based pill dispenser using ESP8266, stepper motor, and real-time scheduling for automatic and manual pill delivery with notifications.
# 💊 Automatic Pill Dispenser

This project is designed to assist elderly patients by automatically dispensing pills at scheduled times and allowing manual overrides. The system sends real-time notifications to caregivers and confirms pill pickup using an IR sensor.

---

## 🚀 Features

- Automatic pill dispensing at predefined times
- Manual dispensing via push button
- Pill pickup detection using IR sensor
- Real-time scheduling via NTP (Network Time Protocol)
- Alerts via buzzer and LED
- Skips next scheduled dose if dispensed manually

---

## 🛠️ Tech Stack

- **Microcontroller**: NodeMCU ESP8266  
- **Motor**: NEMA 17 stepper with TMC2208 driver  
- **Sensors**: IR sensor, push button  
- **Alerts**: LED, Buzzer  
- **Cloud**: Telegram Bot API, NTP client for scheduling  
- **Platform**: Arduino IDE  

---

## 🔧 How It Works

1. The ESP8266 syncs time using NTP.
2. Pills are dispensed automatically at scheduled times using a stepper motor.
3. Users can manually dispense pills using a push button.
4. IR sensor checks if the pill is picked up within 10 seconds.
5. Notifications are sent via mail for every pill dispensed (manual or automatic).
6. If a pill is manually dispensed before the scheduled time, the next schedule is skipped.

---

## 📦 Project Structure

- `pill_dispenser.ino` – Main Arduino sketch
- `StepperLogic.ino` – Stepper motor control logic
- `README.md` – This file

## 📬 Contact

**Ameya Anna George**  
📧 ameyaanna.g@gmail.com  
📍 Kochi, Kerala  
🔗 [LinkedIn](https://www.linkedin.com/in/ameyaannageorge)
