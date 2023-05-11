#pragma once

#if 0
aaa
#include "SinglePortModule.h"
#include "concurrency/NotifiedWorkerThread.h"
#include "configuration.h"
#if defined(ARCH_ESP32)
#include "NodeDB.h"
#include <Arduino.h>
#include <ButterworthFilter.h>
#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>
#include <speex.h>
#include <driver/i2s.h>
#include <functional>

enum SpeexRadioState { speex_standby, speex_rx, speex_tx };

const char speex_c2_magic[3] = {0xc0, 0xde, 0xc2}; // Magic number for speex header

struct speex_c2_header {
    char magic[3];
    char mode;
};

#define ADC_BUFFER_SIZE_MAX 320
#define PTT_PIN 39

#define I2S_PORT I2S_NUM_0

#define AUDIO_MODULE_RX_BUFFER 128
#define AUDIO_MODULE_MODE 700 // meshtastic_ModuleConfig_AudioConfig_Audio_Baud_SPEEX_700

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
    int encode_codec_size = 0;
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
