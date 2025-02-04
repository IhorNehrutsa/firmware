#define LED_PIN 18

// ST7735S TFT LCD
#define ST7735S 1 // there are different (sub-)versions of ST7735
#define ST7735_CS 38
#define ST7735_RS 40  // DC
#define ST7735_SDA 42 // MOSI
#define ST7735_SCK 41
#define ST7735_RESET 39
#define ST7735_MISO -1
#define ST7735_BUSY -1
#define ST7735_BL 45
#define ST7735_SPI_HOST SPI3_HOST
#define ST7735_BACKLIGHT_EN 45
#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 16000000
#define SCREEN_ROTATE
#define TFT_HEIGHT 160
#define TFT_WIDTH 80
#define TFT_OFFSET_X 26
#define TFT_OFFSET_Y 0
#define VTFT_CTRL 46                  // Heltec Tracker needs this pulled low for TFT
#define SCREEN_TRANSITION_FRAMERATE 1 // fps
#define DISPLAY_FORCE_SMALL_FONTS

#define VEXT_ENABLE Vext // active low, powers the oled display and the lora antenna boost
#define BUTTON_PIN 0

#define BATTERY_PIN 1 // A battery voltage measurement pin, voltage divider connected here to measure battery voltage
#define ADC_CHANNEL ADC1_GPIO1_CHANNEL
#define ADC_ATTENUATION ADC_ATTEN_DB_2_5 // lower dB for high resistance voltage divider
#define ADC_MULTIPLIER 4.9

#undef GPS_RX_PIN
#undef GPS_TX_PIN
#define GPS_RX_PIN 33
#define GPS_TX_PIN 34
#define PIN_GPS_RESET 35
#define PIN_GPS_PPS 36
#define VGNSS_CTRL 37 // Heltec Tracker needs this pulled low for GPS
#define PIN_GPS_EN VGNSS_CTRL
#define GPS_EN_ACTIVE LOW
#define GPS_RESET_MODE LOW
#define GPS_UC6580

#define USE_SX1262
#define LORA_DIO0 -1 // a No connect on the SX1262 module
#define LORA_RESET 12
#define LORA_DIO1 14 // SX1262 IRQ
#define LORA_DIO2 13 // SX1262 BUSY
#define LORA_DIO3    // Not connected on PCB, but internally on the TTGO SX1262, if DIO3 is high the TXCO is enabled

#define RF95_SCK 9
#define RF95_MISO 11
#define RF95_MOSI 10
#define RF95_NSS 8

#define SX126X_CS RF95_NSS
#define SX126X_DIO1 LORA_DIO1
#define SX126X_BUSY LORA_DIO2
#define SX126X_RESET LORA_RESET
#define SX126X_E22