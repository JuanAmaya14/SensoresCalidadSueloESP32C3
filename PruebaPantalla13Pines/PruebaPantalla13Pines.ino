#include <Arduino.h>
#include <U8g2lib.h>

#define PIN_CS   10  // Pin 1 (CS)
#define PIN_RSE  5   // Pin 2 (RSE)
#define PIN_RS   4   // Pin 3 (RS)
#define PIN_SCL  6   // Pin 4 (SCL)
#define PIN_SI   7   // Pin 5 (SI)

// Constructor ST7565 para GMG12864-06D
U8G2_ST7565_ERC12864_ALT_F_4W_SW_SPI u8g2(U8G2_R0, PIN_SCL, PIN_SI, PIN_CS, PIN_RS, PIN_RSE);

void setup(void) {
  u8g2.begin();
  u8g2.setContrast(60); // Ajusta este valor (0-255) si la pantalla se ve muy oscura o clara
}

void loop(void) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g_font_ncenB10r);
    u8g2.drawStr(10, 25, "GMG12864-06D");
    u8g2.drawStr(10, 45, "ESP32-C3 Listo!");
  } while ( u8g2.nextPage() );
  delay(1000);
}
