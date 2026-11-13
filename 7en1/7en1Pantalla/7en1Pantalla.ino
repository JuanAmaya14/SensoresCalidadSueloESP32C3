#include <Arduino.h>
#include <U8g2lib.h>

// --- Pines de la pantalla GMG12864-06D ---
#define PIN_CS   10  // Pin 1 (CS)
#define PIN_RSE  5   // Pin 2 (RSE)
#define PIN_RS   4   // Pin 3 (RS)
#define PIN_SCL  6   // Pin 4 (SCL)
#define PIN_SI   7   // Pin 5 (SI)

// --- Pines del módulo HW-097 (RS485) en ESP32-C3 ---
#define RX_PIN 0  // Conectado a RO
#define TX_PIN 1  // Conectado a DI
#define DE_RE  2  // Conectado a DE y RE puenteados juntos

// Constructor ST7565 validado para tu pantalla
U8G2_ST7565_ERC12864_ALT_F_4W_SW_SPI u8g2(U8G2_R0, PIN_SCL, PIN_SI, PIN_CS, PIN_RS, PIN_RSE);

// Usamos Serial1 por hardware
#define mod Serial1

// Modbus RTU tramas de solicitud para sensor 7 en 1
const byte humed[] = { 0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0B };
const byte tempe[] = { 0x01, 0x03, 0x00, 0x07, 0x00, 0x01, 0x35, 0xCB };
const byte condu[] = { 0x01, 0x03, 0x00, 0x08, 0x00, 0x01, 0x05, 0xC8 };
const byte pH[]    = { 0x01, 0x03, 0x00, 0x09, 0x00, 0x01, 0x54, 0x08 };
const byte nitro[] = { 0x01, 0x03, 0x00, 0x04, 0x00, 0x01, 0xC5, 0xCB };
const byte phos[]  = { 0x01, 0x03, 0x00, 0x05, 0x00, 0x01, 0x94, 0x0B };
const byte pota[]  = { 0x01, 0x03, 0x00, 0x12, 0x00, 0x01, 0x24, 0x0F };

byte trama[7];  // Variable para lectura de trama de respuesta del sensor

int leerParametro(const char* etiqueta, const byte* solicitud, size_t len) {
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
  mod.flush();

  // 2. MODO RECEPCIÓN
  digitalWrite(DE_RE, LOW);
  delay(200);

  // 3. LEER RESPUESTA
  memset(trama, 0, sizeof(trama));
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
    return -1;
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Inicialización de pantalla
  u8g2.begin();
  u8g2.setContrast(60);

  // Mensaje inicial en pantalla mientras hace la primera lectura
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.setCursor(15, 30);
    u8g2.print("Iniciando sensor");
    u8g2.setCursor(35, 45);
    u8g2.print("7 en 1...");
  } while (u8g2.nextPage());

  // Inicialización de Serial1 para RS485
  mod.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW);

  Serial.println("--- Iniciando lectura Sensor 7 en 1 (ESP32-C3) ---");
}

void loop() {
  int val1H  = leerParametro("Hum", humed, sizeof(humed));
  delay(300);
  int val2T  = leerParametro("Temp", tempe, sizeof(tempe));
  delay(300);
  int val3C  = leerParametro("Cond", condu, sizeof(condu));
  delay(300);
  int val4pH = leerParametro("pH", pH, sizeof(pH));
  delay(300);
  int val5N  = leerParametro("Nit", nitro, sizeof(nitro));
  delay(300);
  int val6F  = leerParametro("Phos", phos, sizeof(phos));
  delay(300);
  int val7P  = leerParametro("Pot", pota, sizeof(pota));

  // Conversión de escalas (Nota: si tu pH da 700, cambia / 10.0 por / 100.0)
  float hum  = val1H / 10.0;
  float temp = val2T / 100.0;
  float ph   = val4pH / 100.0;

  // --- MOSTRAR EN MONITOR SERIAL ---
  Serial.println("========================================");
  Serial.print("Humedad:       "); Serial.print(hum);   Serial.println(" %RH");
  Serial.print("Temperatura:   "); Serial.print(temp);  Serial.println(" C");
  Serial.print("Conductividad: "); Serial.print(val3C); Serial.println(" uS/cm");
  Serial.print("pH:            "); Serial.println(ph);
  Serial.print("Nitrogeno:     "); Serial.print(val5N); Serial.println(" mg/kg");
  Serial.print("Fosforo:       "); Serial.print(val6F); Serial.println(" mg/kg");
  Serial.print("Potasio:       "); Serial.print(val7P); Serial.println(" mg/kg");
  Serial.println("========================================\n");

  // --- MOSTRAR EN PANTALLA LCD ---
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_tf);

    // Título y línea divisoria
    u8g2.setCursor(22, 10);
    u8g2.print("SENSOR 7 EN 1");
    u8g2.drawHLine(0, 12, 128);

    // Columna Izquierda (NPK + Humedad)
    u8g2.setCursor(2, 25);
    u8g2.print("N: "); u8g2.print(val5N);

    u8g2.setCursor(2, 37);
    u8g2.print("P: "); u8g2.print(val6F);

    u8g2.setCursor(2, 49);
    u8g2.print("K: "); u8g2.print(val7P);

    u8g2.setCursor(2, 62);
    u8g2.print("H: "); u8g2.print(hum, 1); u8g2.print("%");

    // Columna Derecha (Temperatura, Conductividad, pH)
    u8g2.setCursor(66, 25);
    u8g2.print("T: "); u8g2.print(temp, 1); u8g2.print("C");

    u8g2.setCursor(66, 37);
    u8g2.print("EC:"); u8g2.print(val3C);

    u8g2.setCursor(66, 49);
    u8g2.print("pH:"); u8g2.print(ph, 2);

  } while (u8g2.nextPage());

  delay(3000);
}
