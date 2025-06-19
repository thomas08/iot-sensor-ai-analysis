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

## 📡 **MQTT Configuration**

```javascript
// MQTT Trigger Settings
{
  "host": "www.sensesiot.net",
  "port": 3057,
  "username": "YOUR_UID",
  "password": "",
  "topic": "data/YOUR_DEVICEKEY/+",
  "qos": 0
}
```

## 🔧 **Code Nodes**

### **1. Code Node (Data Processing)**

```javascript
// Code Node - แปลงข้อมูล MQTT เป็นรูปแบบที่ AI เข้าใจ
try {
  const input = $input.first().json;
  const rawValue = input.message || input.payload || input.value || 0;
  const topic = input.topic || 'data/qwCOowlTnOYNSLVmZVMW/1';
  const timestamp = new Date().toLocaleString('th-TH');
  
  console.log('📡 Raw value from MQTT:', rawValue);
  
  // แปลงค่าเป็นตัวเลข
  let value = 0;
  if (rawValue !== null && rawValue !== undefined && rawValue !== '') {
    const parsedValue = parseFloat(rawValue);
    if (!isNaN(parsedValue)) {
      value = parsedValue;
    }
  }
  
  // แยกประเภท sensor
  const topicParts = topic.split('/');
  const deviceKey = topicParts[1] || 'qwCOowlTnOYNSLVmZVMW';
  let slot = topicParts[2] || '1';
  
  // กำหนดประเภท sensor
  let sensorName, sensorType, unit, icon, assessment;
  
  switch(slot) {
    case '1':
      sensorName = 'อุณหภูมิ';
      sensorType = 'temperature';
      unit = '°C';
      icon = '🌡️';
      if (value < 16) assessment = 'เย็นมาก';
      else if (value < 20) assessment = 'เย็น';
      else if (value < 25) assessment = 'สบาย';
      else if (value < 30) assessment = 'อบอุ่น';
      else if (value < 35) assessment = 'ร้อน';
      else assessment = 'ร้อนมาก';
      break;
      
    case '2':
      sensorName = 'ความชื้น';
      sensorType = 'humidity';
      unit = '%';
      icon = '💧';
      if (value < 30) assessment = 'แห้งมาก';
      else if (value < 40) assessment = 'แห้ง';
      else if (value < 60) assessment = 'เหมาะสม';
      else if (value < 80) assessment = 'ชื้น';
      else assessment = 'ชื้นมาก';
      break;
      
    default:
      sensorName = 'Unknown';
      sensorType = 'unknown';
      unit = '';
      icon = '📊';
      assessment = 'ไม่ทราบ';
  }
  
  console.log(`${icon} ${sensorName}: ${value}${unit} (${assessment})`);
  
  // สร้าง prompt สำหรับ AI
  const message = `${icon} ได้รับข้อมูล${sensorName}ใหม่!

📊 รายละเอียด:
• ${sensorName}: ${value}${unit}
• ประเมินเบื้องต้น: ${assessment}
• เวลา: ${timestamp}
• อุปกรณ์: ${deviceKey}

โปรดวิเคราะห์ข้อมูล${sensorName} ${value}${unit} และให้ความเห็น:
1. ค่านี้เป็นอย่างไร (ปกติ/สูง/ต่ำ)
2. เหมาะสมสำหรับสภาพแวดล้อมหรือไม่  
3. คำแนะนำหรือข้อควรระวัง
4. ผลกระทบต่อสุขภาพ

ตอบเป็นภาษาไทยแบบกระชับ`;

  return [{
    json: {
      message: message,
      sensor_data: {
        name: sensorName,
        type: sensorType,
        value: value,
        unit: unit,
        icon: icon,
        slot: slot,
        assessment: assessment,
        timestamp: timestamp,
        topic: topic,
        device_key: deviceKey
      }
    }
  }];
  
} catch (error) {
  console.log('❌ Error:', error.message);
  return [{
    json: {
      message: "เกิดข้อผิดพลาดในการประมวลผลข้อมูล",
      sensor_data: {
        name: "Error",
        value: 0,
        unit: "",
        icon: "❌"
      }
    }
  }];
}
```

### **2. Code1 Node (Display Results)**

```javascript
// Code1 Node - แสดงผลรายงานจาก AI
try {
  const input = $input.first().json;
  const aiResponse = input.response || 'ไม่มีการตอบกลับจาก AI';
  const sensor = input.sensor_data;

  if (sensor) {
    console.log('========================================');
    console.log('🤖 รายงานจาก AI');
    console.log('========================================');
    console.log(`📊 เซนเซอร์: ${sensor.name}`);
    console.log(`🔢 ค่า: ${sensor.value}${sensor.unit}`);
    console.log(`📈 สถานะ: ${sensor.assessment}`);
    console.log(`🕐 เวลา: ${sensor.timestamp}`);
    console.log('');
    console.log('🤖 AI Analysis:');
    console.log(aiResponse);
    console.log('========================================');
  } else {
    console.log('❌ ไม่พบข้อมูล sensor');
  }

  return [{
    json: {
      success: true,
      report: {
        sensor: sensor?.name,
        value: `${sensor?.value}${sensor?.unit}`,
        assessment: sensor?.assessment,
        ai_analysis: aiResponse,
        timestamp: sensor?.timestamp
      }
    }
  }];
  
} catch (error) {
  console.log('❌ Error in display:', error.message);
  return [{ json: { success: false, error: error.message } }];
}
```

## 🤖 **AI Agent Configuration**

```javascript
// AI Agent Settings
{
  "model": "llama3.1", // หรือ gemma2
  "baseURL": "http://localhost:11434",
  "temperature": 0.7,
  "maxTokens": 400,
  "systemPrompt": "คุณเป็นผู้เชี่ยวชาญด้านการวิเคราะห์ข้อมูลสภาพแวดล้อม ให้คำแนะนำเป็นภาษาไทยแบบเข้าใจง่าย"
}
```

## 🔌 **ESP32 Code (Arduino)**

```cpp
// ESP32 SHTC3 + Sensesiot
#include <Sensesiot.h>
#include <Wire.h>
#include <Adafruit_SHTC3.h>
#include <Adafruit_Sensor.h>

const char key[] = "YOUR_SENSESIOT_DEVICEKEY";
const char wifissid[] = "YOUR_WIFI_SSID";
const char wifipw[] = "YOUR_WIFI_PASSWORD";

SensesiotClient sensesProtocol(key);
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  if (!shtc3.begin()) {
    Serial.println("SHTC3 sensor failed!");
    while (1) delay(1000);
  }
  
  sensesProtocol.begin(wifissid, wifipw);
  sensesProtocol.waitUntilReady();
  Serial.println("Connected to Sensesiot");
}

void loop() {
  if (!sensesProtocol.ready()) {
    sensesProtocol.begin(wifissid, wifipw);
    sensesProtocol.waitUntilReady();
  }
  
  sensesProtocol.loop();
  
  sensors_event_t humidity_event, temp_event;
  if (shtc3.getEvent(&humidity_event, &temp_event)) {
    sensesProtocol.setData(1, temp_event.temperature);    // อุณหภูมิ
    sensesProtocol.setData(2, humidity_event.relative_humidity); // ความชื้น
    
    Serial.println("Data sent: T=" + String(temp_event.temperature) + 
                   "°C, H=" + String(humidity_event.relative_humidity) + "%");
  }
  
  delay(5000); // ส่งทุก 5 วินาที
}
```

## 🚀 **Installation & Setup**

### **1. Prerequisites**
```bash
# Install n8n
npm install -g n8n

# Install Ollama
curl -fsSL https://ollama.ai/install.sh | sh
ollama pull llama3.1
```

### **2. n8n Workflow Setup**
1. Import JSON workflow file
2. Configure MQTT credentials
3. Set Ollama model settings
4. Test with sample data

### **3. Hardware Setup**
```
ESP32 Wiring:
VCC  → 3.3V
GND  → GND
SDA  → GPIO21
SCL  → GPIO22
```

## 📊 **Features**

- ✅ **Real-time Sensor Monitoring**
- ✅ **AI-powered Analysis** 
- ✅ **Thai Language Support**
- ✅ **MQTT Integration**
- ✅ **Error Handling**
- ✅ **Debug Logging**
- ✅ **Chat Interface**
- ✅ **Scalable Architecture**

## 📈 **Sample Output**

```
========================================
🤖 รายงานจาก AI
========================================
📊 เซนเซอร์: อุณหภูมิ
🔢 ค่า: 25.7°C
📈 สถานะ: สบาย
🕐 เวลา: 19/6/2568, 15:30:25

🤖 AI Analysis:
วิเคราะห์ข้อมูลอุณหภูมิ 25.7°C:

1. ค่าปกติ: อยู่ในช่วงปกติ (น่าสวัสดี)
2. เหมาะสำหรับสภาพแวดล้อม: เหมาะสำหรับการอยู่อาศัย
3. คำแนะนำ: ไม่จำเป็นต้องใช้เครื่องปรับอากาศ 
4. ผลกระทบต่อสุขภาพ: ไม่มีผลกระทบ มีสภาพแวดล้อมดี

ขอให้ปฏิบัติตามคำแนะนำ!
========================================
```

## 🔧 **Configuration Files**

### **n8n Workflow JSON**
```json
{
  "name": "IoT Sensor AI Analysis",
  "nodes": [
    // MQTT Trigger Node
    // Code Processing Node  
    // AI Agent Node
    // Display Results Node
  ],
  "connections": {
    // Node connections
  }
}
```

### **Environment Variables**
```env
N8N_HOST=localhost
N8N_PORT=5678
OLLAMA_HOST=localhost:11434
MQTT_HOST=www.sensesiot.net
MQTT_PORT=3057
```

## 📝 **Usage**

1. **Start Services:**
   ```bash
   ollama serve
   n8n start
   ```

2. **Deploy ESP32 Code**
3. **Activate n8n Workflow** 
4. **Monitor Real-time Data**

## 🤝 **Contributing**

1. Fork the repository
2. Create feature branch
3. Commit changes
4. Submit pull request

## 📄 **License**

MIT License - See LICENSE file for details

## 📞 **Support**

- GitHub Issues
- Documentation Wiki
- Community Forum

---

**🌟 IoT + AI + Thai Language = Smart Environmental Monitoring! 🌟**
