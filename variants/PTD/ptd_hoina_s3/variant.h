#define HAS_SCREEN 0
#define HAS_GPS 0
#define HAS_BUTTON 1
#define HAS_WIRE 0
#define HAS_TELEMETRY 0
#define HAS_SENSOR 0

/*
 * PTD Buttons
 */
//#define BUTTON_UP     14 // 0x0004000
//#define BUTTON_LEFT   12 // 0x0001000
#define BUTTON_CENTER 26 // 0x4000000
//#define BUTTON_RIGHT  13 // 0x0002000
//#define BUTTON_DOWN   15 // 0x0008000
//                       // 0x400f000

/*
 *                                  INMP441 Microphone Module                     MAX98357A Mono Amplifier Module
 */
// 3.3V                             VDD - The Input voltage, from 1.62-3.63V      Vin=2.5-5.5V
// GND                              GND - Ground                                  GND - Ground
// GND                              L/R - Channel selection
// 3.3V                                                                           SD - Left Channel
// Not/Connected                                                                  GANE 9dB
//     /* I2S Word Select */
// moduleConfig.audio.i2s_ws =   21; WS - Word Select                             LRC - Left-Right Channel
#define I2S_WS          GPIO_NUM_NC
//     /* I2S Data IN */
// moduleConfig.audio.i2s_sd =   23; SD - The I2S Serial Data connection
#define I2S_SD          GPIO_NUM_NC
//     /* I2S Data OUT */
// moduleConfig.audio.i2s_din =  4;                                               DIN - Serial Data Input
#define I2S_DIN         GPIO_NUM_NC
//     /* I2S Clock */
// moduleConfig.audio.i2s_sck = 22; SCK - Serial Clock                            BCLK - Bit Clock Line
#define I2S_SCK        GPIO_NUM_NC
//
//     /* PTT Pin */
// moduleConfig.audio.ptt_pin = 26;
#define PTT_PIN        GPIO_NUM_NC

//       3.5 jack
// 1 R - microphone
// 4 - GND
// 2 - Left
// 3 - Right

/*
#define PIN_BUTTON_TOUCH BUTTON_RIGHT
*/
//#define BUTTON_PIN       BUTTON_UP // The middle button GPIO on the T-Beam
/*
#define BUTTON_PIN_ALT   BUTTON_RIGHT
#define BUTTON_PIN_TOUCH PIN_BUTTON_TOUCH
*/

//#define BATTERY_PIN 35 // A battery voltage measurement pin, voltage divider connected here to measure battery voltage
//#define ADC_MULTIPLIER 1.85 // (R1 = 470k, R2 = 680k)
//#define EXT_NOTIFY_OUT 27 // Overridden default pin to use for Ext Notify Module (#975).
//#define LED_PIN 27 // add status LED (compatible with core-pcb and DIY targets)

#define LORA_DIO0 -1  // a No connect on the SX1262/SX1268 module
#define LORA_RESET 25  // RST for SX1276, and for SX1262/SX1268
#define LORA_DIO1 21  // IRQ for SX1262/SX1268
#define LORA_DIO2 -1  // 16 // BUSY for SX1262/SX1268, BUT NOT for SX126X_E22 module
#define LORA_DIO3 -1  // Not connected on PCB, but internally on the TTGO SX1262/SX1268, if DIO3 is high the TXCO is enabled
#define LORA_RXEN 2
#define LORA_TXEN 4
#define LORA_BUSY 22  // BUSY for SX126X_E22 module
#define LORA_NSS 5

#define RF95_SCK 18
#define RF95_MISO 19
#define RF95_MOSI 23
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
