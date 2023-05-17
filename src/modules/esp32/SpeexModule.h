#pragma once

#if 1
/*
 * i2s.read() -> speex.encode() -> lora-tx() -> lora->rx() -> speex.decode() -> i2s.write()
 */
// #define SELF_LISTENING_I2S
#ifndef SELF_LISTENING_I2S
  #define RUN_ENCODE_DECODE
  #ifdef RUN_ENCODE_DECODE
  // #define RUN_TX_RX
  #endif
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
extern "C" {
#include <speex.h>
}
#endif
#include <driver/i2s.h>
#include <functional>

/*
 * Time interval between samples (8 kbps) = 1s/8000 = 0.125ms = 125μs (16 bits per sample)
 * Frame is 20ms.
 * Frame length = 160 samples (16 bit) x 125μs = 20 ms
 * One frame will compress to 20 bytes (8 kbps). Time per byte = 20 ms / 20 bytes = 1 ms / byte
*/

#define CHANNEL_FORMAT (I2S_CHANNEL_FMT_ONLY_LEFT)

/*
 * Samlpe rate in Hz
 *
 *  8000 // narrowband     // Speex delay is 30 ms
 * 16000 // wideband       // Speex delay is 34 ms
 * 32000 // ultra-wideband // Speex delay is ??? ms
 */
#define SAMPLE_RATE_HZ 8000
/*
 * WORD SELECT = SAMPLE_RATE_HZ
 */
#define F_WS (SAMPLE_RATE_HZ) // 8kHz for samlpe rate 8kHz
/*
 * bit_clock = rate * (number of channels) * bits_per_sample
 *     256k  =   8k *          2           *      16
 */
#define F_CLK (F_WS * SAMPLE_SIZE_IN_BITS * 2) // 256kHz for 2 channels(Left & Right), 8kHz samlpe rate and 16bit per sample

/*
 * Samlpe size in bits, ADC/DAC resolution
 */
#define SAMPLE_SIZE_IN_BITS (I2S_BITS_PER_SAMPLE_16BIT)
//#define SAMPLE_SIZE_IN_BITS (I2S_BITS_PER_SAMPLE_24BIT)

#define SAMPLE_SIZE_IN_BYTES (((SAMPLE_SIZE_IN_BITS + 15) / 16) * 2)

/*
 * At a frame rate of 50 the frames are 20 milliseconds long.
 * 20ms (50 frames por second).
 */

/*
 * Frames in 1 seconds with 20ms frame
 * 20ms / (1 / SAMPLE_RATE_HZ) == 20ms * SAMPLE_RATE_HZ == 20 * SAMPLE_RATE_HZ / 1000
 */
#define FRAMES_PER_1s  (20 * SAMPLE_RATE_HZ / 1000)
#define FRAMES_PER_1s_8kHz  160 // narrowband     // 20 * 8000 / 1000
#define FRAMES_PER_1s_16kHz 320 // wideband       // 20 * 16000 / 1000
#define FRAMES_PER_1s_32kHz 640 // ultra-wideband // 20 * 32000 / 1000

/*
 *        |<-----   FRAME  ----->|
 *    _____            __________
 *         \__________/          \_____
 *               L          R
 *            SAMPLE     SAMPLE
 */

/*
  Get active channel number according to channel format
    switch (chan_fmt)
    case I2S_CHANNEL_FMT_RIGHT_LEFT: //fall through
    case I2S_CHANNEL_FMT_ALL_RIGHT:  //fall through
    case I2S_CHANNEL_FMT_ALL_LEFT:
        active_chan = 2;
    case I2S_CHANNEL_FMT_ONLY_RIGHT: //fall through
    case I2S_CHANNEL_FMT_ONLY_LEFT:
        active_chan = 1;
 */
#define ACTIVE_CHANELS ((CHANNEL_FORMAT == I2S_CHANNEL_FMT_RIGHT_LEFT) || (CHANNEL_FORMAT == I2S_CHANNEL_FMT_ALL_RIGHT) || (CHANNEL_FORMAT == I2S_CHANNEL_FMT_ALL_LEFT) ? 2 : (CHANNEL_FORMAT == I2S_CHANNEL_FMT_ONLY_RIGHT) || (CHANNEL_FORMAT == I2S_CHANNEL_FMT_ONLY_LEFT) ? 1 : -1)

/**
 * I2S DMA buffer size in bytes
 *
 *   bytes_per_sample is bytes per sample, align to 16 bit
 *   bytes_per_frame = bytes_per_sample * active_chanels
 *   DMA buffer size in bytes = dma_buf_len * bytes_per_frame;
 */
/*
 * Buffers size for ADC, DAC, Speex in bytes
 */
#define ADC_BUFFER_SIZE_IN_BYTES (FRAMES_PER_1s * ACTIVE_CHANELS * SAMPLE_SIZE_IN_BYTES) * 10
/*
 * Buffer size for i2s in frames
 */
#define DMA_BUF_LEN_IN_I2S_FRAMES (FRAMES_PER_1s * ACTIVE_CHANELS)

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
    int16_t speech[ADC_BUFFER_SIZE_IN_BYTES] = {};
    int16_t output_buffer[ADC_BUFFER_SIZE_IN_BYTES] = {};
    uint8_t adc_buffer[ADC_BUFFER_SIZE_IN_BYTES] = {};
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
