#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "ESP32-C3-AP";
const char* password = "";

#define RE 8
#define DE 7

#define RX_PIN 4  // RO
#define TX_PIN 5  // DI

const byte ph[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A };
byte values[11];

HardwareSerial mod(1);
WebServer server(80);

float soil_ph = 0.0;

void autoCrearHTML() {
  File file = LittleFS.open("/index.html", "w");
  if (!file) {
    Serial.println("Error critico: No se pudo crear index.html en la memoria Flash.");
    return;
  }
  
  file.print("<!DOCTYPE html><html><head><meta charset='utf-8'>");
  file.print("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  file.print("<title>ESP32-C3 AJAX</title><style>");
  file.print("body { text-align: center; font-family: sans-serif; background: #f4f4f4; color: #333; }");
  file.print(".card { background: rgb(182, 192, 183); max-width: 400px; margin: 50px auto; padding: 30px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); }");
  file.print("h1 { color: #0066cc; } .valor { font-size: 3rem; font-weight: bold; color: #ff5500; }");
  file.print("</style><script>");
  file.print("setInterval(function() { fetch('/leerPH').then(res => res.text()).then(data => { document.getElementById('ph').innerText = data; }).catch(err => console.error(err)); }, 1000);");
  file.print("</script></head><body><div class='card'><h1>ESP32-C3 PH SOIL</h1><p>El valor del PH es:</p><div class='valor' id='ph'>...</div></div></body></html>");
  
  file.close();
  Serial.println("¡[EXITO] El archivo index.html ha sido auto-creado en LittleFS!");
}

void handleRoot() {
  if (LittleFS.exists("/index.html")) {
    File file = LittleFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "Error crítico: El archivo sigue sin existir.");
  }
}

void handleVariable() {
  server.send(200, "text/plain", String(soil_ph, 1));
}

float SensorPH() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  while (mod.available() > 0) {
    mod.read();
  }

  if (mod.write(ph, sizeof(ph)) == 8) {
    mod.flush();

    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    delay(50);

    int availableBytes = mod.available();
    if (availableBytes > 0) {
      for (byte i = 0; i < 11; i++) {
        values[i] = mod.read();
      }

      float ph_calculado = (float)values[4] / 10.0;
      return ph_calculado;
    } else {
      Serial.println("Error: No se recibieron datos del sensor.");
    }
  }
  return soil_ph; 
}

void setup() {
  Serial.begin(115200);
  mod.begin(4800, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  Serial.println("PH Meter Inicializado en ESP32-C3...");

  if (!LittleFS.begin(true)) {
    Serial.println("Error al montar LittleFS");
    return;
  }
  Serial.println("LittleFS montado correctamente.");

  if (!LittleFS.exists("/index.html")) {
    Serial.println("index.html faltante detectado. Iniciando auto-creacion...");
    autoCrearHTML();
  } else {
    Serial.println("index.html encontrado perfectamente en la memoria Flash.");
  }

  WiFi.softAP(ssid, password);
  Serial.print("IP del Access Point: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/leerPH", handleVariable);

  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  server.handleClient();

  static unsigned long ultimoTiempo = 0;
  if (millis() - ultimoTiempo > 2000) {
    ultimoTiempo = millis();
    soil_ph = SensorPH();
    Serial.print("Soil pH: ");
    Serial.println(soil_ph, 1);
  }
}
