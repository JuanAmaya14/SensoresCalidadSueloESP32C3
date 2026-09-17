#define RE 8
#define DE 7

// Definimos los pines para el puerto Serial Modbus (puedes cambiarlos si lo necesitas)
#define RX_PIN 4  //RO
#define TX_PIN 5  //DI

const byte ph[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A };
byte values[11];

// Creamos un objeto de tipo HardwareSerial en el puerto 1
HardwareSerial mod(1);

void setup() {
  Serial.begin(9600);  // Monitor Serial de la PC (USB)

  // Inicializamos el puerto del sensor con sus pines TX y RX
  mod.begin(4800, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  Serial.println("PH Meter Inicializado en ESP32-C3...");
}

void loop() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  // Limpiamos el buffer de lectura antes de enviar
  while (mod.available() > 0) {
    mod.read();
  }

  if (mod.write(ph, sizeof(ph)) == 8) {
    // Esperamos un instante a que se terminen de transmitir los datos físicos antes de apagar el transmisor RS485
    mod.flush();

    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    // Pequeño delay para que el sensor responda
    delay(50);

    int availableBytes = mod.available();
    if (availableBytes > 0) {
      for (byte i = 0; i < 11; i++) {
        values[i] = mod.read();
        Serial.print(values[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      float soil_ph = (float)values[4] / 10.0;
      Serial.print("Soil pH: ");
      Serial.println(soil_ph, 1);
    } else {
      Serial.println("Error: No se recibieron datos del sensor.");
    }
  }

  delay(3000);
}