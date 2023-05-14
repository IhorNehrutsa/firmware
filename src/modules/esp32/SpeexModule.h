#pragma once

#if 1
/*
 * i2s.read() -> speex.encode() -> lora-tx() -> lora->rx() -> speex.decode() -> i2s.write()
 */
//#define RUN_ENCODE_DECODE
#ifdef RUN_ENCODE_DECODE
#define RUN_TX_RX
#endif

// #define USE_BUTTERWORTH_FILTER

#include "SinglePortModule.h"
#include "concurrency/NotifiedWorkerThread.h"
#include "configuration.h"
#if defined(ARCH_ESP32)
#include "NodeDB.h"
#include <Arduino.h>
#include <ButterworthFilter.h>
#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>
#ifdef RUN_ENCODE_DECODE
#include "speex/speex.h"
#endif
#include <driver/i2s.h>
//#include <driver/i2s_std.h>
#include <functional>

#define DMA_BUF_LEN_IN_I2S_FRAMES 256

/*
 * Samlpe size in bits, ADC/DAC resolution
 */
#define SAMPLE_SIZE_16bit I2S_BITS_PER_SAMPLE_16BIT
#define SAMPLE_SIZE_24bit I2S_BITS_PER_SAMPLE_24BIT
#define SAMPLE_SIZE       SAMPLE_SIZE_16bit

#if SAMPLE_SIZE == SAMPLE_SIZE_16bit
  #define SAMPLE_SIZE_IN_BYTES 2
#elif SAMPLE_SIZE == SAMPLE_SIZE_24bit
  #define SAMPLE_SIZE_IN_BYTES 4
#else
  #error SAMPLE_SIZE_IN_BYTES
#endif

/*
 * Samlpe rate in Hz
 */
#define SAMPLE_RATE_8kHz   8000 // narrowband     // Speex delay is 30 ms
#define SAMPLE_RATE_16kHz 16000 // wideband       // Speex delay is 34 ms
#define SAMPLE_RATE_32kHz 32000 // ultra-wideband // Speex delay is ??? ms
#define SAMPLE_RATE       SAMPLE_RATE_8kHz

#define F_WS (SAMPLE_RATE) // 8kHz for samlpe rate 8kHz
#define F_CLK (F_WS * SAMPLE_SIZE * 2) // 256kHz for 2 channels, 8kHz samlpe rate and 16bit sample size
/*
 * At a frame rate of 50 the frames are 20 milliseconds long.
 * 20ms (50 frames por second).
 */

/*
 * Frame size in samples
 * 20ms / (1 / SAMPLE_RATE) == 20ms * SAMPLE_RATE == 20 * SAMPLE_RATE / 1000
 */
#define FRAME_SIZE  (20 * SAMPLE_RATE / 1000) // in samples
#define FRAME_SIZE_8kHz  160 // 20 * 8000 / 1000
#define FRAME_SIZE_16kHz 320 // 20 * 16000 / 1000
#define FRAME_SIZE_32kHz 640 // 20 * 32000 / 1000

#if SAMPLE_RATE == SAMPLE_RATE_8kHz
  #if FRAME_SIZE != FRAME_SIZE_8kHz
    #error FRAME_SIZE 8kHz
  #endif
#elif SAMPLE_RATE == SAMPLE_RATE_16kHz
  #if FRAME_SIZE != FRAME_SIZE_16kHz
    #error FRAME_SIZE 16kHz
  #endif
#elif SAMPLE_RATE == SAMPLE_RATE_32kHz
  #if FRAME_SIZE != FRAME_SIZE_32kHz
    #error FRAME_SIZE 32kHz
  #endif
#else
  #error FRAME_SIZE
#endif

/*
 * Buffers size for ADC, DAC, Speex in bytes
 */
#define ADC_BUFFER_SIZE_MAX (FRAME_SIZE * SAMPLE_SIZE_IN_BYTES) // in bytes !!!

#define I2S_PORT I2S_NUM_0

// #define AUDIO_MODULE_RX_BUFFER 128
#define AUDIO_MODULE_MODE meshtastic_ModuleConfig_Audio_Config_Speex_Bit_Rate_SPEEX_5950

enum SpeexRadioState { speex_standby, speex_rx, speex_tx };

const char speex_c2_magic[3] = {0xc0, 0xde, 0xc2}; // Magic number for speex header

struct speex_c2_header {
    char magic[3];
    char mode;
};

class SpeexModule : public SinglePortModule, public Observable<const UIFrameEvent *>, private concurrency::OSThread
{
  public:
    unsigned char rx_encode_frame[meshtastic_Constants_DATA_PAYLOAD_LEN] = {};
    unsigned char tx_encode_frame[meshtastic_Constants_DATA_PAYLOAD_LEN] = {};
    speex_c2_header tx_header = {};
    int16_t speech[ADC_BUFFER_SIZE_MAX] = {};
    int16_t output_buffer[ADC_BUFFER_SIZE_MAX] = {};
    uint16_t adc_buffer[ADC_BUFFER_SIZE_MAX] = {};
    int adc_buffer_size = 0;
    uint16_t adc_buffer_index = 0;
    int tx_encode_frame_index = sizeof(speex_c2_header); // leave room for header
    int rx_encode_frame_index = 0;
    int encode_codec_size = 0; // codec2_bits_per_frame
    int encode_frame_size = 0;
    volatile SpeexRadioState radio_state = SpeexRadioState::speex_rx;

    void *speex = NULL; // speex encoder state

    SpeexModule();

    bool shouldDraw();

    /**
     * Send our payload into the mesh
     */
    void sendPayload(NodeNum dest = NODENUM_BROADCAST, bool wantReplies = false);

  protected:
    int encode_frame_num = 0;
    bool firstTime = true;

    virtual int32_t runOnce() override;

    virtual meshtastic_MeshPacket *allocReply() override;

    virtual bool wantUIFrame() override { return this->shouldDraw(); }
    virtual Observable<const UIFrameEvent *> *getUIFrameObservable() override { return this; }
#if !HAS_SCREEN
    void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
#else
    virtual void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) override;
#endif

    /** Called to handle a particular incoming message
     * @return ProcessMessage::STOP if you've guaranteed you've handled this message and no other handlers should be considered
     * for it
     */
    virtual ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;
};

extern SpeexModule *speexModule;

#endif
#endif
