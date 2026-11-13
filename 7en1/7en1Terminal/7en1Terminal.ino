#include <Arduino.h>

// --- Pines del módulo HW-097 (RS485) en ESP32-C3 ---
#define RX_PIN 0  // Conectado a RO
#define TX_PIN 1  // Conectado a DI
#define DE_RE 2   // Conectado a DE y RE

// Usamos Serial1 por hardware en lugar de SoftwareSerial
#define mod Serial1

// Modbus RTU tramas de solicitud para sensor 7 en 1
const byte humed[] = { 0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0B };
const byte tempe[] = { 0x01, 0x03, 0x00, 0x07, 0x00, 0x01, 0x35, 0xCB };
const byte condu[] = { 0x01, 0x03, 0x00, 0x08, 0x00, 0x01, 0x05, 0xC8 };
const byte pH[] = { 0x01, 0x03, 0x00, 0x09, 0x00, 0x01, 0x54, 0x08 };
const byte nitro[] = { 0x01, 0x03, 0x00, 0x04, 0x00, 0x01, 0xC5, 0xCB };
const byte phos[] = { 0x01, 0x03, 0x00, 0x05, 0x00, 0x01, 0x94, 0x0B };
const byte pota[] = { 0x01, 0x03, 0x00, 0x12, 0x00, 0x01, 0x24, 0x0F };

byte trama[7];  // Variable para lectura de trama de respuesta del sensor

// Función única y limpia para consultar cualquier parámetro Modbus
int leerParametro(const char* etiqueta, const byte* solicitud, size_t len) {
  // Limpiar cualquier basura previa en el buffer de recepción
  while (mod.available()) {
    mod.read();
  }

  // 1. MODO TRANSMISIÓN
  digitalWrite(DE_RE, HIGH);
  delay(10);

  Serial.print("TX-");
  Serial.print(etiqueta);
  Serial.print(": ");
  for (size_t i = 0; i < len; i++) {
    mod.write(solicitud[i]);
    Serial.print("0x");
    if (solicitud[i] < 16) Serial.print("0");
    Serial.print(solicitud[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  mod.flush();  // Espera a que terminen de salir todos los bytes

  // 2. MODO RECEPCIÓN
  digitalWrite(DE_RE, LOW);
  delay(200);

  // 3. LEER RESPUESTA
  memset(trama, 0, sizeof(trama));  // Reinicia el arreglo para evitar lecturas viejas
  Serial.print("RX-");
  Serial.print(etiqueta);
  Serial.print(": ");

  if (mod.available() >= 7) {
    for (int i = 0; i < 7; i++) {
      trama[i] = mod.read();
      if (trama[i] < 0x10) Serial.print("0");
      Serial.print(trama[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    int valor = (trama[3] << 8) | trama[4];
    return valor;
  } else {
    Serial.println("Sin respuesta o trama incompleta");
    return -1;  // Indica error de lectura
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);  // Pausa breve para estabilizar el puerto USB del ESP32-C3

  // Inicialización de Serial1 en los pines asignados
  mod.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW);

  Serial.println("--- Iniciando lectura Sensor 7 en 1 (ESP32-C3) ---");
}

void loop() {
  int val1H = leerParametro("Hum", humed, sizeof(humed));
  delay(300);
  int val2T = leerParametro("Temp", tempe, sizeof(tempe));
  delay(300);
  int val3C = leerParametro("Cond", condu, sizeof(condu));
  delay(300);
  int val4pH = leerParametro("pH", pH, sizeof(pH));
  delay(300);
  int val5N = leerParametro("Nit", nitro, sizeof(nitro));
  delay(300);
  int val6F = leerParametro("Phos", phos, sizeof(phos));
  delay(300);
  int val7P = leerParametro("Pot", pota, sizeof(pota));

  Serial.println("========================================");
  Serial.print("Humedad:       ");
  Serial.print(val1H / 10.0);
  Serial.println(" %RH");
  Serial.print("Temperatura:   ");
  Serial.print(val2T / 100.0);
  Serial.println(" C");
  Serial.print("Conductividad: ");
  Serial.print(val3C);
  Serial.println(" uS/cm");
  Serial.print("pH:            ");
  Serial.print(val4pH / 10.0);
  Serial.println();
  Serial.print("Nitrogeno:     ");
  Serial.print(val5N);
  Serial.println(" mg/kg");
  Serial.print("Fosforo:       ");
  Serial.print(val6F);
  Serial.println(" mg/kg");
  Serial.print("Potasio:       ");
  Serial.print(val7P);
  Serial.println(" mg/kg");
  Serial.println("========================================\n");

  delay(3000);  // Espera 3 segundos antes del siguiente ciclo
}
