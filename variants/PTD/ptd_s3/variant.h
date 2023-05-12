#define HAS_SCREEN 0
#define HAS_GPS 0
#define HAS_BUTTON 1
#define HAS_WIRE 0
#define HAS_TELEMETRY 0
#define HAS_SENSOR 0

/*
 * PTD Buttons
 */
//#define BUTTON_UP     -1 // 0x
//#define BUTTON_LEFT   -1 // 0x
#define BUTTON_CENTER 14 // 0x PTT
//#define BUTTON_RIGHT  -1 // 0x
//#define BUTTON_DOWN   -1 // 0x
//                          0x

/*
 *                                  INMP441 Microphone Module                     MAX98357A Mono Amplifier Module
 */
// 3.3V                             VDD - The Input voltage, from 1.62-3.63V      Vin=2.5-5.5V
// GND                              GND - Ground                                  GND - Ground
// GND                              L/R - Channel selection                       LRC - Left-Right Channel
// Not/Connected                                                                  GANE
// Not/Connected                                                                  SD
//     /* I2S Word Select */
// moduleConfig.audio.i2s_ws =   6; WS - Word Select                              WS - Word Select(FS - Frame Select)
//     /* I2S Data IN */
// moduleConfig.audio.i2s_sd =   4; SD - The I2S Serial Data connection
//     /* I2S Data OUT */
// moduleConfig.audio.i2s_din =  5;                                               DIN - Serial Data Input
//     /* I2S Clock */
// moduleConfig.audio.i2s_sck =  7; SCK - Serial Clock                            BCLK - Bit Clock Line
//
//     /* PTT Pin */
// moduleConfig.audio.ptt_pin =  15;

//       3.5 jack
// 1 R - microphone
// 4 - GND
// 2 - Left
// 3 - Right

//#define BUTTON_PIN 39 // The middle button GPIO on the T-Beam
//#define BATTERY_PIN 35 // A battery voltage measurement pin, voltage divider connected here to measure battery voltage
//#define ADC_MULTIPLIER 1.85 // (R1 = 470k, R2 = 680k)
//#define EXT_NOTIFY_OUT 12 // Overridden default pin to use for Ext Notify Module (#975).
//#define LED_PIN 2 // add status LED (compatible with core-pcb and DIY targets)

#define LORA_DIO0 -1  // a No connect on the SX1262/SX1268 module
#define LORA_RESET 38  // RST for SX1276, and for SX1262/SX1268
#define LORA_DIO1 36  // IRQ for SX1262/SX1268
#define LORA_DIO2 -1  // 16 // BUSY for SX1262/SX1268, BUT NOT for SX126X_E22 module
#define LORA_DIO3 -1  // Not connected on PCB, but internally on the TTGO SX1262/SX1268, if DIO3 is high the TXCO is enabled
#define LORA_RXEN 18
#define LORA_TXEN 8
#define LORA_BUSY 37  // BUSY for SX126X_E22 module
#define LORA_NSS 42

#define RF95_SCK 41
#define RF95_MISO 39
#define RF95_MOSI 40
#define RF95_NSS LORA_NSS

// supported modules list
//#define USE_SX1262
/*
 DO NOT USE USE_SX1268 !
 firmware\src\mesh\SX1268Interface.h
 ...
    /// override frequency of the SX1268 module regardless of the region (use EU433 value)
    virtual float getFreq() override { return 433.175f; }
 ...
*/
#define USE_SX1268

// common pinouts for SX126X modules
#define SX126X_CS LORA_NSS // NSS for SX126X
#define SX126X_DIO1 LORA_DIO1
#define SX126X_BUSY LORA_BUSY
#define SX126X_RESET LORA_RESET
#define SX126X_RXEN LORA_RXEN
#define SX126X_TXEN LORA_TXEN

#define SX126X_E22 // Indicates this SX1262 is inside of an ebyte E22 module and special config should be done for that
