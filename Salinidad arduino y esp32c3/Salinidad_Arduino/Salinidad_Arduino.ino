const int pinSensor = A0;

const float rangoMaximoSalinidad = 1000.0;

void setup() {
  Serial.begin(9600);
  Serial.println("--- Lectura de Salinidad RK500-23 (0-1000 ppm) ---");
}

void loop() {
  int adcVal = analogRead(pinSensor);

  float voltaje = adcVal * (5.0 / 1023.0);

  float miliamperios = (voltaje / 220.0) * 1000.0;

  float salinidadPPM = 0.0;
  if (miliamperios >= 3.8) {  // Pequeño margen de tolerancia para el cero
    salinidadPPM = ((miliamperios - 4.0) / 16.0) * rangoMaximoSalinidad;
  }

  if (salinidadPPM < 0) salinidadPPM = 0.0;

  Serial.print("Voltaje en A0: ");
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
