# 💡 ESP32 LDR Smart LED Controller

[![ESP32](https://img.shields.io/badge/ESP32-ESP32--WROOM--32-blue)](https://www.espressif.com)
[![License](https://img.shields.io/badge/License-MIT-yellow)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Arduino%20IDE-green)](https://www.arduino.cc)

> An intelligent LED control system that responds to ambient light with a beautiful animated web dashboard.

## ✨ Features

- 🌞 **LDR Light Sensor** - Real-time ambient light detection (0-4095 range)
- 🤖 **Auto Mode** - 3-tier automatic LED control based on light thresholds
- 🎮 **Manual Mode** - Direct individual LED control via web buttons
- 🌊 **Water Wave Animation** - Stunning HTML5 Canvas visualization of LDR data
- 📊 **Live Updates** - AJAX-powered sensor refresh every second
- 💡 **3-Channel Output** - Control up to 3 LEDs independently
- 📱 **Responsive Design** - Works on desktop, tablet, and mobile
- 🔄 **Mode Toggle** - Seamless switch between Auto and Manual modes
- 🎨 **Visual Feedback** - LED buttons change color based on state

## 🎯 Auto Mode Logic

| LDR Value | Condition | LED 1 | LED 2 | LED 3 |
|-----------|-----------|-------|-------|-------|
| ≤ 1000 | Very Dark | 🟢 ON | 🔴 OFF | 🔴 OFF |
| ≤ 2000 | Dim | 🔴 OFF | 🟢 ON | 🔴 OFF |
| > 2000 | Bright | 🔴 OFF | 🔴 OFF | 🟢 ON |
