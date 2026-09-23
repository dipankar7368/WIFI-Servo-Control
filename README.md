# ESP32-C3 Wi-Fi Servo Control 🤖

A simple IoT project that allows you to control an **SG90 servo motor wirelessly using an ESP32-C3 and a mobile phone**.

The ESP32-C3 creates its own Wi-Fi network and hosts a mobile-friendly web page. After connecting the phone to the ESP32-C3, the servo angle can be controlled directly from the browser.

## 📸 Project

![ESP32-C3 Servo](c:\Users\Dipankar Bhunia\Videos\YOUTUBE VIDEO DATA\IMG20260923204332.jpg.jpeg)

## 🔧 Components

- ESP32-C3 Super Mini
- SG90 Servo Motor
- External 5V power supply
- Jumper wires

## 🔌 Wiring

| SG90 | ESP32-C3 |
|---|---|
| Orange (Signal) | GPIO 3 |
| Red (VCC) | 5V |
| Brown (GND) | GND |

⚠️ Use a suitable external 5V supply for the servo and connect the grounds together.

## ⚙️ Working

```text
📱 Mobile
   ↓ Wi-Fi
ESP32-C3
   ↓ PWM
SG90 Servo
   ↓
Angle Control
```

The ESP32-C3 works as a Wi-Fi Access Point, so **internet is not required**.

Connect to:

```text
Wi-Fi: ESP32-Servo
Password: 12345678
```

Then open:

```text
192.168.4.1
```

## ✨ Features

- 📱 Mobile web control
- 📡 Wi-Fi control
- ⚙️ Servo angle control
- 🔄 Real-time movement
- 🌐 No internet required
- 💻 Arduino IDE compatible

## 👨‍💻 Author

**Dipankar Bhunia**

IoT • Electronics • Robotics • Embedded Systems
