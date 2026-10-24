#include <ModbusMaster.h>

#define RE 8
#define DE 7
#define RX 4  // RO
#define TX 5  // DI

ModbusMaster node;

void preTransmission() {
  digitalWrite(RE, HIGH);
  digitalWrite(DE, HIGH);
}

void postTransmission() {
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  Serial1.begin(9600, SERIAL_8N1, RX, TX);

  node.begin(1, Serial1);
  
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("Leyendo sensor RK500-01...");
}

void loop() {
  uint8_t result;
  
  result = node.readHoldingRegisters(0, 1);
  
  if (result == node.ku8MBSuccess) {
    int16_t rawData = node.getResponseBuffer(0);
    
    float temperatura = rawData / 10.0;
    
    Serial.print("Valor Raw: ");
    Serial.print(rawData);
    Serial.print("  ->  Temperatura: ");
    Serial.print(temperatura, 1);
    Serial.println(" °C");
  } else {
    Serial.print("Error de Comunicación Modbus: 0x");
    Serial.println(result, HEX);
  }

  delay(2000);
}
