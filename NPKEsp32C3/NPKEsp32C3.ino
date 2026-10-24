#define RE 8      // Pin RE
#define DE 7      // Pin DE
#define RX_PIN 4  // Pin RO
#define TX_PIN 5  // Pin DI)

//NOTA: USAR 12V PARA EL SENSOR NPK

const byte nitro[] = { 0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C };  // Trama - Nitrógeno
const byte phos[] = { 0x01, 0x03, 0x00, 0x1F, 0x00, 0x01, 0xB5, 0xCC };   // Trama - Fósforo
const byte pota[] = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xC0 };   // Trama - Potasio

byte trama[7];

void setup() {
  Serial.begin(9600);

  Serial1.begin(4800, SERIAL_8N1, RX_PIN, TX_PIN);

  delay(100);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  Serial.println("INICIANDO SENSOR NPK");

  delay(500);
}

void loop() {
  int val1N, val2F, val3P;

  delay(500);
  val1N = nitrogeno();

  delay(500);
  val2F = fosforo();

  delay(500);
  val3P = potasio();
  delay(500);

  Serial.println("\n=================================");
  Serial.println("      RESULTADOS NPK [mg/kg]     ");
  Serial.println("=================================");

  Serial.print("NITRÓGENO = ");
  Serial.print(val1N);
  Serial.println(" mg/kg");

  Serial.print("FÓSFORO   = ");
  Serial.print(val2F);
  Serial.println(" mg/kg");

  Serial.print("POTASIO   = ");
  Serial.print(val3P);
  Serial.println(" mg/kg");

  Serial.println("=================================\n");

  delay(2000);
}

int nitrogeno() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  Serial.print("TX-Nitro:  ");
  for (int i = 0; i < sizeof(nitro); i++) {
    Serial1.write(nitro[i]);
    Serial.print("0x");
    if (nitro[i] < 16) Serial.print("0");
    Serial.print(nitro[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial1.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(200);

  Serial.print("RX-Nitro:  ");
  if (Serial1.available() >= 7) {
    for (int i = 0; i < 7; i++) {
      trama[i] = Serial1.read();
    }
    delay(300);
    for (int i = 0; i < 7; i++) {
      if (trama[i] < 0x10) Serial.print("0");
      Serial.print(trama[i], HEX);
      Serial.print(" ");
    }
  }

  byte highByte = trama[3];
  byte lowByte = trama[4];
  int valor = (highByte << 8) | lowByte;

  Serial.println();
  Serial.print("Valor decimal: ");
  Serial.println(valor);
  Serial.println();

  while (Serial1.available()) Serial1.read();
  return valor;
}

int fosforo() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  Serial.print("TX-Fosf:   ");
  for (int i = 0; i < sizeof(phos); i++) {
    Serial1.write(phos[i]);
    Serial.print("0x");
    if (phos[i] < 16) Serial.print("0");
    Serial.print(phos[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial1.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(200);

  Serial.print("RX-Fosf:   ");
  if (Serial1.available() >= 7) {
    for (int i = 0; i < 7; i++) {
      trama[i] = Serial1.read();
    }
    delay(300);
    for (int i = 0; i < 7; i++) {
      if (trama[i] < 0x10) Serial.print("0");
      Serial.print(trama[i], HEX);
      Serial.print(" ");
    }
  }
  Serial.println();

  byte highByte = trama[3];
  byte lowByte = trama[4];
  int valor = (highByte << 8) | lowByte;

  Serial.print("Valor decimal: ");
  Serial.println(valor);
  Serial.println();

  while (Serial1.available()) Serial1.read();
  return valor;
}

int potasio() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  Serial.print("TX-Pota:   ");
  for (int i = 0; i < sizeof(pota); i++) {
    Serial1.write(pota[i]);
    Serial.print("0x");
    if (pota[i] < 16) Serial.print("0");
    Serial.print(pota[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial1.flush();

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(200);

  Serial.print("RX-Pota:   ");
  if (Serial1.available() >= 7) {
    for (int i = 0; i < 7; i++) {
      trama[i] = Serial1.read();
    }
    delay(300);
    for (int i = 0; i < 7; i++) {
      if (trama[i] < 0x10) Serial.print("0");
      Serial.print(trama[i], HEX);
      Serial.print(" ");
    }
  }
  Serial.println();

  byte highByte = trama[3];
  byte lowByte = trama[4];
  int valor = (highByte << 8) | lowByte;

  Serial.print("Valor decimal: ");
  Serial.println(valor);
  Serial.println();

  while (Serial1.available()) Serial1.read();
  return valor;
}