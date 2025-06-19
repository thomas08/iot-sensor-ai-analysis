# 🌡️ IoT Sensor AI Analysis System

ระบบวิเคราะห์ข้อมูล Sensor แบบอัตโนมัติด้วย AI โดยใช้ n8n + Ollama + MQTT

## 🏗️ **Architecture**

```
ESP32 (SHTC3) → MQTT → Sensesiot Platform → n8n → Ollama AI → Report
```

### **Workflow Components:**
1. **MQTT Trigger** - รับข้อมูลจาก Sensesiot
2. **Code Node** - ประมวลผลและแปลงข้อมูล  
3. **AI Agent** - วิเคราะห์ด้วย Ollama
4. **Code1 Node** - แสดงผลรายงาน
5. **Chat Interface** - รองรับการสื่อสารแบบ manual



🚀 Installation & Setup
1. Prerequisites
bash# Install n8n
npm install -g n8n

# Install Ollama
curl -fsSL https://ollama.ai/install.sh | sh
ollama pull llama3.1
2. n8n Workflow Setup

Import JSON workflow file
Configure MQTT credentials
Set Ollama model settings
Test with sample data

3. Hardware Setup
ESP32 Wiring:
VCC  → 3.3V
GND  → GND
SDA  → GPIO21
SCL  → GPIO22
