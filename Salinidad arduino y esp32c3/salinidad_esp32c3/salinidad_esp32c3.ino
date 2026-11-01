const int pinSensor = 3;


// Cambiar a 220.0 SOLO si no pusiste las dos resistencias en paralelo.
// Se recomienda usar 110.0 (dos de 220 de forma paralela) por seguridad.
const float valorResistencia = 110.0;

const float rangoMaximoSalinidad = 1000.0;

void setup() {
  Serial.begin(115200);

  analogSetAttenuation(ADC_11db);

  Serial.println("\n--- Lectura de Salinidad RK500-23 en ESP32-C3 ---");
}

void loop() {
  int adcVal = analogRead(pinSensor);

  float voltaje = adcVal * (3.1 / 4095.0);

  float miliamperios = (voltaje / valorResistencia) * 1000.0;

  float salinidadPPM = 0.0;
  if (miliamperios >= 3.8) {
    salinidadPPM = ((miliamperios - 4.0) / 16.0) * rangoMaximoSalinidad;
  }

  if (salinidadPPM < 0) salinidadPPM = 0.0;
  if (salinidadPPM > rangoMaximoSalinidad) salinidadPPM = rangoMaximoSalinidad;

  Serial.print("Voltaje en IO3: ");
  Serial.print(voltaje, 3);
  Serial.print(" V | ");
  Serial.print("Corriente: ");
  Serial.print(miliamperios, 2);
  Serial.print(" mA | ");
  Serial.print("Salinidad: ");
  Serial.print(salinidadPPM, 1);
  Serial.println(" ppm");

  delay(1500);
}
