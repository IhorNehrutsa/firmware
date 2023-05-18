#pragma once

#define USE_SPEEX_MODULE

#ifdef USE_SPEEX_MODULE
/*
 * DMA_BUF_LEN_IN_FRAMES -> adc_buffer -> speech->bits->tx_encode_frame ->           ->            ->   rx_encode_frame->bits->output_buffer -> output_buffer -> DMA_BUF_LEN_IN_FRAMES
 * i2s.driver()          -> i2s.read() ->      speex.encode()           -> lora-tx() -> lora->rx() ->         speex.decode()                 -> i2s.write()   -> i2s.driver()
 */
#define RUN_ENCODE_DECODE /// +
#ifdef RUN_ENCODE_DECODE
  // #define RUN_TX_RX /// +
#endif
#define SELF_LISTENING_I2S /// -

#define USE_BUTTERWORTH_FILTER

#include "SinglePortModule.h"
#include "concurrency/NotifiedWorkerThread.h"
#include "configuration.h"
#if defined(ARCH_ESP32)
#include "NodeDB.h"
#include <Arduino.h>
#include <ButterworthFilter.h>
#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>
//#ifdef RUN_ENCODE_DECODE
// extern "C" {
//#include <lsp.h>
//#include <lpc.h>
#include <speex.h>
//}
//#endif
#include <driver/i2s.h>
#include <functional>

/*
 * Samlpe rate in Hz
 *  8000 // narrowband     // Speex delay is 30 ms
 * 16000 // wideband       // Speex delay is 34 ms
 * 32000 // ultra-wideband // Speex delay is ?? ms
 */
#define SAMPLE_RATE_HZ 8000

/*
 * Frame time is 20ms (50 frames por second).
 * Time interval between samples (8 kbps) = 1s/8000 = 0.125ms = 125μs (16 bits per sample)
 * Frame length = 160 samples (16 bit) x 125μs = 20 ms
 * In narrowband, Speex frames are 20 ms long (160 samples)
*/
#define FRAME_TIME_IN_MS 20

/*
 * Frame length in samples
 * 20ms / (1 / SAMPLE_RATE_HZ) == 20ms * SAMPLE_RATE_HZ == 20 * SAMPLE_RATE_HZ / 1000
 * 160 // narrowband     // 20 *  8000 / 1000
 * 320 // wideband       // 20 * 16000 / 1000
 * 640 // ultra-wideband // 20 * 32000 / 1000
 */
#define FRAME_LENGTH_IN_SAMPLES (FRAME_TIME_IN_MS * SAMPLE_RATE_HZ / 1000)

/*
 * Samlpe size in bits, ADC/DAC resolution
 */
//#define SAMPLE_SIZE_IN_BITS (I2S_BITS_PER_SAMPLE_8BIT)
#define SAMPLE_SIZE_IN_BITS (I2S_BITS_PER_SAMPLE_16BIT)
//#define SAMPLE_SIZE_IN_BITS (I2S_BITS_PER_SAMPLE_24BIT)

#define SAMPLE_SIZE_IN_BYTES (((SAMPLE_SIZE_IN_BITS + 15) / 16) * 2)

#define FRAME_SIZE_IN_BITS (FRAME_LENGTH_IN_SAMPLES * SAMPLE_SIZE_IN_BITS)
#define FRAME_SIZE_IN_BYTES (FRAME_LENGTH_IN_SAMPLES * SAMPLE_SIZE_IN_BYTES)

/*
 *      |<-----                     20 ms FRAME                             ----->|
 * _____            __________            __________
 *      \__________/          \__________/          \_____
 *            L          R
 *         SAMPLE     SAMPLE
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
#define CHANNEL_FORMAT (I2S_CHANNEL_FMT_ONLY_LEFT)
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
#define ADC_BUFFER_SIZE_IN_BYTES ((FRAME_LENGTH_IN_SAMPLES * ACTIVE_CHANELS * SAMPLE_SIZE_IN_BYTES) * 10)
/*
 * Buffer size for i2s in frames
 */
#define DMA_BUF_LEN_IN_FRAMES ((FRAME_LENGTH_IN_SAMPLES * ACTIVE_CHANELS) * 4)
#define DMA_BUF_COUNT 2
// SAMPLE_RATE_HZ  16k // 32k
//                  2, //  4, //  8, //

/*
 * WORD SELECT = SAMPLE_RATE_HZ
 */
#define F_WS (SAMPLE_RATE_HZ) // 8kHz for samlpe rate 8kHz
/*
 * bit_clock = rate * (number of channels) * bits_per_sample
 *     256k  =   8k *          2           *      16
 */
#define F_CLK (F_WS * SAMPLE_SIZE_IN_BITS * 2) // 256kHz for 2 channels(Left & Right), 8kHz samlpe rate and 16bit per sample

#define I2S_PORT I2S_NUM_0

// #define AUDIO_MODULE_RX_BUFFER 128
#define SPEEX_MODE meshtastic_ModuleConfig_Audio_Config_Speex_Bit_Rate_SPEEX_3950

enum SpeexRadioState { speex_standby, speex_rx, speex_tx };

const char speex_magic[3] = {0xc0, 0xde, 0xc2}; // Magic number for speex header

struct speex_header {
    char magic[3];
    char mode;
};

class SpeexModule : public SinglePortModule, public Observable<const UIFrameEvent *>, private concurrency::OSThread
{
  public:
    unsigned char rx_encode_frame[meshtastic_Constants_DATA_PAYLOAD_LEN] = {};
    unsigned char tx_encode_frame[meshtastic_Constants_DATA_PAYLOAD_LEN] = {};
    speex_header tx_header = {};
    spx_int16_t speech[ADC_BUFFER_SIZE_IN_BYTES] = {};
    spx_int16_t output_buffer[ADC_BUFFER_SIZE_IN_BYTES] = {};
    unsigned char adc_buffer[ADC_BUFFER_SIZE_IN_BYTES] = {};
    unsigned int adc_buffer_size = 0; // in bytes
    unsigned int adc_buffer_index = 0;
    unsigned int tx_encode_frame_index = sizeof(speex_header); // leave room for header
    unsigned int rx_encode_frame_index = 0;
    unsigned int encode_codec_size = 0; // speex_bits_per_frame ???
    unsigned int encode_frame_size = 0;
    volatile SpeexRadioState radio_state = SpeexRadioState::speex_rx;

    void *speex_enc = NULL; // speex encoder state
    void *speex_dec = NULL; // speex decoder state

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
