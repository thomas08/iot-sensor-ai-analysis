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

