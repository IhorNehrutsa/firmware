#include "configuration.h"
#if defined(ARCH_ESP32)
#include "SpeexModule.h"
#endif

#ifdef USE_SPEEX_MODULE

#if defined(ARCH_ESP32)
#include "FSCommon.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "RTC.h"
#include "Router.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#ifdef OLED_RU
#include "graphics/fonts/OLEDDisplayFontsRU.h"
#endif

#ifdef OLED_UA
#include "graphics/fonts/OLEDDisplayFontsUA.h"
#endif

/*
    SpeexModule
        A interface to send raw speex audio data over the mesh network. Based on the example code from the ???.
        https://github.com/???

        ???
        Codec 2 is a low-bitrate speech_ audio codec (speech_ coding)
        that is patent free and open source develop by David Grant Rowe.
        http://www.rowetel.com/ and https://github.com/drowe67/codec2
        ???

    Basic Usage:
        1) Enable the module by setting audio.speex_enabled to CODEC_SPEEX.
        2) Set the pins for the I2S interface. Recommended on TLora is I2S_WS 13/I2S_SD 15/I2S_SIN 2/I2S_SCK 14
        3) Set audio.bitrate to the desired speex rate (SPEEX_3200, SPEEX_2400, SPEEX_1600, SPEEX_1400, SPEEX_1300,
   SPEEX_1200, SPEEX_700, SPEEX_700B)

    KNOWN PROBLEMS
        * Half Duplex
        * Will not work on NRF and the Linux device targets (yet?).
*/

#define xTaskGetMs() (xTaskGetTickCount() * 1000 / configTICK_RATE_HZ)
#define esp_millis() ((esp_timer_get_time() + 500ULL) / 1000ULL)

tx_header_t tx_header = {
    .magic = {0xc0, 0xde, 0xc2}, // Magic number for speex header
    .mode =0
};

static ButterworthFilter hp_filter(240, 8000, ButterworthFilter::ButterworthFilter::Highpass, 1);

TaskHandle_t speexHandlerTask;
SpeexModule *speexModule;

#ifdef ARCH_ESP32
// ESP32 doesn't use that flag
#define YIELD_FROM_ISR(x) portYIELD_FROM_ISR()
#else
#define YIELD_FROM_ISR(x) portYIELD_FROM_ISR(x)
#endif

#if defined(USE_EINK) || defined(ILI9341_DRIVER) || defined(ST7735_CS)
// The screen is bigger so use bigger fonts
#define FONT_SMALL ArialMT_Plain_16
#define FONT_MEDIUM ArialMT_Plain_24
#define FONT_LARGE ArialMT_Plain_24
#else
#ifdef OLED_RU
#define FONT_SMALL ArialMT_Plain_10_RU
#else
#ifdef OLED_UA
#define FONT_SMALL ArialMT_Plain_10_UA
#else
#define FONT_SMALL ArialMT_Plain_10
#endif
#endif
#define FONT_MEDIUM ArialMT_Plain_16
#define FONT_LARGE ArialMT_Plain_24
#endif

#define fontHeight(font) ((font)[1] + 1) // height is position 1

#define FONT_HEIGHT_SMALL fontHeight(FONT_SMALL)
#define FONT_HEIGHT_MEDIUM fontHeight(FONT_MEDIUM)
#define FONT_HEIGHT_LARGE fontHeight(FONT_LARGE)

void run_speex(void *parameter)
{
    LOG_INFO("Starting speex task\n");
#if 0
    LOG_INFO("portTICK_PERIOD_MS :  %d\n", portTICK_PERIOD_MS);
    LOG_INFO("portTICK_RATE_MS : %d\n", portTICK_RATE_MS);
    LOG_INFO("uxTaskGetNumberOfTasks() : %d\n", uxTaskGetNumberOfTasks());

    LOG_INFO("pcTaskGetName=%s\n",pcTaskGetName(0));
    LOG_INFO("uxTaskPriorityGet=%d\n",(int)uxTaskPriorityGet(0));

    long long int Timer1, Timer2;
    Timer1 = esp_timer_get_time();
    vTaskDelay(pdMS_TO_TICKS(1));
    Timer2 = esp_timer_get_time();
    LOG_INFO("esp_timer_get_time()\n");
    LOG_INFO("Timer1: %lld μs\n", Timer1);
    LOG_INFO("Timer2: %lld μs\n", Timer2);
    LOG_INFO("Difference: %lld μs\n", Timer2 - Timer1);

    Timer1 = portGET_RUN_TIME_COUNTER_VALUE();
    vTaskDelay(pdMS_TO_TICKS(1));
    Timer2 = portGET_RUN_TIME_COUNTER_VALUE();
    LOG_INFO("portGET_RUN_TIME_COUNTER_VALUE()\n");
    LOG_INFO("Timer1: %lld μs\n", Timer1);
    LOG_INFO("Timer2: %lld μs\n", Timer2);
    LOG_INFO("Difference: %lld μs\n", Timer2 - Timer1);

    Timer1 = millis();
    vTaskDelay(pdMS_TO_TICKS(1));
    Timer2 = millis();
    LOG_INFO("millis()\n");
    LOG_INFO("Timer1: %lld ms\n", Timer1);
    LOG_INFO("Timer2: %lld ms\n", Timer2);
    LOG_INFO("Difference: %lld ms\n", Timer2 - Timer1);

    Timer1 = xTaskGetMs();
    vTaskDelay(pdMS_TO_TICKS(1));
    Timer2 = xTaskGetMs();
    LOG_INFO("xTaskGetMs()\n");
    LOG_INFO("Timer1: %lld ms\n", Timer1);
    LOG_INFO("Timer2: %lld ms\n", Timer2);
    LOG_INFO("Difference: %lld ms\n", Timer2 - Timer1);
#endif
    // Initialization of the structure that holds the bits
    SpeexBits bits;
    speex_bits_init(&bits);

    esp_err_t res;
    int status;
    int nbBytes;
    while (true) {
        uint32_t tcount = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(10000));
        if (tcount != 0) {
            if (speexModule->radio_state == SpeexRadioState::speex_tx) {
                #ifdef USE_BUTTERWORTH_FILTER
                for (int i = 0; i < ADC_BUF_SIZE_IN_BYTES; i++)
                    speexModule->speech[i] = (int16_t)hp_filter.Update((float)speexModule->speech[i]);
                #endif
#ifdef DO_ENCODE_DECODE
                // Flush all the bits in the struct so we can encode a new frame
                speex_bits_reset(&bits);
                // Encode frame
                status = speex_encode_int(speexModule->speex_enc, speexModule->speech, &bits);
                // Get number of bytes that need to be written
                nbBytes = speex_bits_nbytes(&bits);
                if (nbBytes) {
                    // Copy the bits to an array of char that can be written
                    nbBytes = speex_bits_write(&bits, (char *)speexModule->tx_encode_frame + speexModule->tx_encode_frame_index,
                     meshtastic_Constants_DATA_PAYLOAD_LEN - speexModule->tx_encode_frame_index);

                    speexModule->tx_encode_frame_index += nbBytes;

                    LOG_INFO("tx_encode_frame_index=%d, nbBytes=%d\n", speexModule->tx_encode_frame_index, nbBytes);
#else
                if (true) {
                    memcpy(speexModule->tx_encode_frame + speexModule->tx_encode_frame_index, speexModule->speech, FRAME_LENGTH_IN_SAMPLES);
                    speexModule->tx_encode_frame_index += FRAME_LENGTH_IN_SAMPLES;

                    LOG_INFO("tx_encode_frame_index=%d, FRAME_LENGTH_IN_SAMPLES=%d\n", speexModule->tx_encode_frame_index, FRAME_LENGTH_IN_SAMPLES);
#endif

//                    if (speexModule->tx_encode_frame_index >= (ENCODE_FRAME_SIZE + sizeof(tx_header_t))) {
                    if (speexModule->tx_encode_frame_index > sizeof(tx_header_t)) {
#ifdef SELF_LISTENING_ENCODE
                        speexModule->rx_encode_frame_index = speexModule->tx_encode_frame_index;
                        memcpy(speexModule->rx_encode_frame, speexModule->tx_encode_frame, speexModule->rx_encode_frame_index);
#endif

                        LOG_INFO("Sending %d speex bytes\n", speexModule->tx_encode_frame_index);
                        speexModule->sendPayload();
                        speexModule->tx_encode_frame_index = sizeof(tx_header_t);
                    }
                }
            } else if (speexModule->radio_state == SpeexRadioState::speex_rx) {
                size_t bytesOut = 0;
                for (int i = sizeof(tx_header_t); i < speexModule->rx_encode_frame_index; i += speexModule->encode_codec_size) {
                    tx_header_t h;
                    memcpy(&h, speexModule->rx_encode_frame, sizeof(h));

                    #ifdef DO_ENCODE_DECODE
                    speex_decoder_ctl(speexModule->speex_dec, SPEEX_SET_MODE, &h.mode);

                    // Copy data packet to Speex bitstream
                    speex_bits_read_from(&bits, (char*)speexModule->rx_encode_frame, FRAME_LENGTH_IN_SAMPLES);

                    status =  speex_decode_int(speexModule->speex_dec, &bits, speexModule->output_buffer + i); //+ speexModule->rx_encode_frame
                    if (status) {
                        LOG_ERROR("speex_decode_int: status %d\n", status);
                    }
                    #else
                    memcpy(speexModule->output_buffer, speexModule->rx_encode_frame + i + sizeof(h), FRAME_LENGTH_IN_SAMPLES);
                    #endif
                    // #ifndef SELF_LISTENING_I2S
                    res = i2s_write(I2S_PORT, &speexModule->output_buffer, ADC_BUF_SIZE_IN_BYTES, &bytesOut, pdMS_TO_TICKS(500));
                    if (res == ESP_OK) {
                        LOG_INFO("i2s_write: res=%d, bytesOut=%d, ADC_BUF_SIZE_IN_BYTES=%d\n", res, bytesOut, ADC_BUF_SIZE_IN_BYTES);
                        if (ADC_BUF_SIZE_IN_BYTES != bytesOut) {
                            LOG_ERROR("i2s_write: ADC_BUF_SIZE_IN_BYTES != bytesOut %d , %d\n", ADC_BUF_SIZE_IN_BYTES, bytesOut);
                        }
                    } else {
                        LOG_ERROR("i2s_write: result %d\n", res);
                    }
                    // #endif
                }
            }
        }
    }
    /*
    // Destroy the encoder/decoder state
    speex_encoder_destroy(speexModule->speex_enc);
    speex_encoder_destroy(speexModule->speex_dec);
    // Destroy the bit-stream struct
    speex_bits_destroy(&bits);
    */
}


#ifdef DO_ENCODE_DECODE
/*
#define makechar( x ) #@x
#define stringer( x ) #x
#define paster( n ) "##n"
*/
//#define GET_NAME(name) #name
//#define GET_NAME_VALUE(name) ##name
#ifdef DEBUG_PORT
spx_int32_t value;
#define LOG_SPEEX_GET(CMD_NAME, CMD) \
    speex_encoder_ctl(speex_enc, CMD, &value); \
    LOG_INFO(#CMD_NAME"=%d\n", value);
#endif
#endif

SpeexModule::SpeexModule() : SinglePortModule("SpeexModule", meshtastic_PortNum_AUDIO_SPEEX_APP), concurrency::OSThread("SpeexModule")
{
#if 1
    moduleConfig.audio_config.codec = meshtastic_ModuleConfig_Audio_Config_Audio_Codec_CODEC_SPEEX;
    moduleConfig.audio_config.i2s_ws = I2S_WS;
    moduleConfig.audio_config.i2s_sd = I2S_SD;
    moduleConfig.audio_config.i2s_din = I2S_DIN;
    moduleConfig.audio_config.i2s_sck = I2S_SCK;
    moduleConfig.audio_config.ptt_pin = PTT_PIN;

    // myRegion->audioPermitted = true;
#endif
    if ((moduleConfig.audio_config.codec == meshtastic_ModuleConfig_Audio_Config_Audio_Codec_CODEC_SPEEX) && (myRegion->audioPermitted)) {
        tx_header.mode = (moduleConfig.audio_config.bitrate.speex_bitrate ? moduleConfig.audio_config.bitrate.speex_bitrate : SPEEX_MODE);

        // 4 bytes of header in each frame hex c0 de c2 plus the bitrate
        memcpy(tx_encode_frame, &tx_header, sizeof(tx_header_t));

        LOG_INFO("Setting up speex in mode %u\n", tx_header.mode);
        LOG_INFO("SAMPLE_RATE_HZ=%u\n", SAMPLE_RATE_HZ);
        LOG_INFO("FRAME_TIME_IN_MS=%u\n", FRAME_TIME_IN_MS);
        LOG_INFO("FRAME_LENGTH_IN_SAMPLES=%u\n", FRAME_LENGTH_IN_SAMPLES);
        LOG_INFO("SAMPLE_SIZE_IN_BITS=%u\n", SAMPLE_SIZE_IN_BITS);
        LOG_INFO("SAMPLE_SIZE_IN_BYTES=%u\n", SAMPLE_SIZE_IN_BYTES);
        LOG_INFO("FRAME_SIZE_IN_BITS=%u\n", FRAME_SIZE_IN_BITS);
        LOG_INFO("FRAME_SIZE_IN_BYTES=%u\n", FRAME_SIZE_IN_BYTES);
        LOG_INFO("CHANNEL_FORMAT=%u\n", CHANNEL_FORMAT);
        LOG_INFO("ACTIVE_CHANELS=%u\n", ACTIVE_CHANELS);
        LOG_INFO("ADC_BUF_SIZE_IN_BYTES=%u\n", ADC_BUF_SIZE_IN_BYTES);
        LOG_INFO("DMA_BUF_LEN_IN_FRAMES=%u\n", DMA_BUF_LEN_IN_FRAMES);
        LOG_INFO("DMA_BUF_COUNT=%u\n", DMA_BUF_COUNT);
        LOG_INFO("F_WS=%u\n", F_WS);
        LOG_INFO("F_CLK=%u\n", F_CLK);
        /*
        LOG_INFO("=%u\n", );
        LOG_INFO("=%u\n", );
        LOG_INFO("=%u\n", );
        LOG_INFO("=%u\n", );
        */
#ifdef DO_ENCODE_DECODE
        // Create a new encoder state in narrowband mode
        speex_enc = speex_encoder_init(&speex_nb_mode);
        if (!speex_enc) {
            LOG_ERROR("speex_encoder_init() allocation fails\n");
        }
        speex_dec = speex_decoder_init(&speex_nb_mode);
        if (!speex_dec) {
            LOG_ERROR("speex_decoder_init() allocation fails\n");
        }
/*
SPEEX_GET_ENH              Get perceptual enhancer status (spx_int32_t)
SPEEX_GET_FRAME_SIZE       Get the number of samples per frame for the current mode (spx_int32_t)
SPEEX_GET_QUALITY          Get the current encoder speech quality (spx_int32_t from 0 to 10)
SPEEX_GET_MODE             Get the current mode number, as specified in the RTP spec (spx_int32_t)
SPEEX_GET_VBR              Get variable bit-rate (VBR) status (spx_int32_t)
SPEEX_GET_VBR_QUALITY      Get the current encoder VBR speech quality (float 0 to 10)
SPEEX_GET_COMPLEXITY       Get the CPU resources allowed for the encoder (spx_int32_t from 1 to 10, default is
SPEEX_GET_BITRATE          Get the current bit-rate in use (spx_int32_t in bits per second)
SPEEX_GET_SAMPLING_RATE    Get real sampling rate (spx_int32_t in Hz)
SPEEX_RESET_STATE          Reset the encoder/decoder state to its original state, clearing all memories (no argument)
SPEEX_GET_VAD              Get voice activity detection (VAD) status (spx_int32_t)
SPEEX_GET_DTX              Get discontinuous transmission (DTX) status (spx_int32_t)
SPEEX_GET_ABR              Get average bit-rate (ABR) setting (spx_int32_t in bits per second)
SPEEX_GET_PLC_TUNING       Get the current tuning of the encoder for PLC (spx_int32_t in percent)
SPEEX_GET_VBR_MAX_BITRATE  Get the current maximum bit-rate allowed in VBR operation (spx_int32_t in
SPEEX_GET_HIGHPASS         Get the current high-pass filter status (spx_int32_t)
*/
        LOG_SPEEX_GET("SPEEX_GET_ENH", SPEEX_GET_ENH);
        LOG_SPEEX_GET("SPEEX_GET_FRAME_SIZE", SPEEX_GET_FRAME_SIZE);
        LOG_SPEEX_GET("5", 5); // SPEEX_GET_QUALITY
        LOG_SPEEX_GET("SPEEX_GET_MODE", SPEEX_GET_MODE);
        LOG_SPEEX_GET("SPEEX_GET_VBR", SPEEX_GET_VBR);
        //LOG_SPEEX_GET("SPEEX_GET_VBR_QUALITY", SPEEX_GET_VBR_QUALITY);
        LOG_SPEEX_GET("SPEEX_GET_COMPLEXITY", SPEEX_GET_COMPLEXITY);
        LOG_SPEEX_GET("SPEEX_GET_BITRATE", SPEEX_GET_BITRATE);
        LOG_SPEEX_GET("SPEEX_GET_SAMPLING_RATE", SPEEX_GET_SAMPLING_RATE);
        // LOG_SPEEX_GET("SPEEX_RESET_STATE", SPEEX_RESET_STATE);
        LOG_SPEEX_GET("SPEEX_GET_VAD", SPEEX_GET_VAD);
        LOG_SPEEX_GET("SPEEX_GET_DTX", SPEEX_GET_DTX);
        LOG_SPEEX_GET("SPEEX_GET_ABR", SPEEX_GET_ABR);
        LOG_SPEEX_GET("SPEEX_GET_PLC_TUNING", SPEEX_GET_PLC_TUNING);
        LOG_SPEEX_GET("SPEEX_GET_VBR_MAX_BITRATE", SPEEX_GET_VBR_MAX_BITRATE);
        LOG_SPEEX_GET("SPEEX_GET_HIGHPASS", SPEEX_GET_HIGHPASS);

        //// Set quality to 8 (15 kbps)
        spx_int32_t quality = 2; // 5,950kbps
        speex_encoder_ctl(speex_enc, SPEEX_SET_QUALITY, &quality);

        spx_int32_t mode;
        speex_encoder_ctl(speex_enc, SPEEX_GET_MODE, &mode);
        speex_decoder_ctl(speex_dec, SPEEX_SET_MODE, &mode);
        spx_int32_t speex_frame_size;
        speex_decoder_ctl(speex_dec, SPEEX_GET_FRAME_SIZE, &speex_frame_size);
        LOG_INFO("speex_dec SPEEX_GET_FRAME_SIZE=%d\n", speex_frame_size);

        encode_codec_size = FRAME_LENGTH_IN_SAMPLES; // (speex_bits_per_frame(speex) + 7) / 8;
        encode_frame_num = (meshtastic_Constants_DATA_PAYLOAD_LEN - sizeof(tx_header_t)) / encode_codec_size;
        speex_encoder_ctl(speex_enc, SPEEX_GET_FRAME_SIZE, &speex_frame_size); // Get the number of samples per frame for the current mode (spx_int32_t)
        LOG_INFO("using %d frames of %d bytes for a total payload length of %d bytes\n", encode_frame_num, encode_codec_size, ENCODE_FRAME_SIZE);

#endif
        xTaskCreate(&run_speex, "speex_task", 32768, NULL, configMAX_PRIORITIES, &speexHandlerTask);
    } else {
        disable();
    }
}

void SpeexModule::drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    char buffer[50];

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(FONT_SMALL);
    display->fillRect(0 + x, 0 + y, x + display->getWidth(), y + FONT_HEIGHT_SMALL);
    display->setColor(BLACK);
    display->drawStringf(0 + x, 0 + y, buffer, "Speex Mode %d Audio", tx_header.mode);
    display->setColor(WHITE);
    display->setFont(FONT_LARGE);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    switch (radio_state) {
    case SpeexRadioState::speex_tx:
        display->drawString(display->getWidth() / 2 + x, (display->getHeight() - FONT_HEIGHT_SMALL) / 2 + y, "PTT");
        break;
    default:
        display->drawString(display->getWidth() / 2 + x, (display->getHeight() - FONT_HEIGHT_SMALL) / 2 + y, "Receive");
        break;
    }
}

    uint32_t _millis_wr=0, _millis_rd =0;
    uint32_t millis_wr=0, millis_rd =0;
int32_t SpeexModule::runOnce()
{
    if ((moduleConfig.audio_config.codec == meshtastic_ModuleConfig_Audio_Config_Audio_Codec_CODEC_SPEEX) && (myRegion->audioPermitted)) {
        esp_err_t res;
        if (firstTime) {
            radio_state = SpeexRadioState::speex_rx;

            // Set up I2S Processor configuration. This will produce 16bit samples at 8 kHz instead of 12 from the ADC
            LOG_INFO("Initializing I2S SD: %d DIN: %d WS: %d SCK: %d\n", moduleConfig.audio_config.i2s_sd, moduleConfig.audio_config.i2s_din,
                     moduleConfig.audio_config.i2s_ws, moduleConfig.audio_config.i2s_sck);
            i2s_config_t i2s_config = {.mode = (i2s_mode_t)(I2S_MODE_MASTER | (moduleConfig.audio_config.i2s_sd ? I2S_MODE_RX : 0) |
                                                            (moduleConfig.audio_config.i2s_din ? I2S_MODE_TX : 0)),
                                       .sample_rate = SAMPLE_RATE_HZ,
                                       .bits_per_sample = SAMPLE_SIZE_IN_BITS,
                                       .channel_format = CHANNEL_FORMAT,
                                       .communication_format = I2S_COMM_FORMAT_STAND_I2S,
                                       .intr_alloc_flags = 0,
                                       .dma_buf_count = DMA_BUF_COUNT,
                                       .dma_buf_len = DMA_BUF_LEN_IN_FRAMES,
                                       .use_apll = false,
                                       .tx_desc_auto_clear = true,
                                       .fixed_mclk = 0};
            res = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
            if (res != ESP_OK) {
                LOG_ERROR("Failed to install I2S driver: %d\n", res);
            }

            const i2s_pin_config_t pin_config = {
                .mck_io_num = I2S_PIN_NO_CHANGE,
                .bck_io_num = moduleConfig.audio_config.i2s_sck,
                .ws_io_num = moduleConfig.audio_config.i2s_ws,
                .data_out_num = moduleConfig.audio_config.i2s_din ? moduleConfig.audio_config.i2s_din : I2S_PIN_NO_CHANGE,
                .data_in_num = moduleConfig.audio_config.i2s_sd ? moduleConfig.audio_config.i2s_sd : I2S_PIN_NO_CHANGE};
            res = i2s_set_pin(I2S_PORT, &pin_config);
            if (res != ESP_OK) {
                LOG_ERROR("Failed to set I2S pin config: %d\n", res);
            }
            /*
            // clear the DMA buffers
            res = i2s_zero_dma_buffer(I2S_PORT);
            if (res != ESP_OK) {
                LOG_ERROR("Failed to zero I2S dma buffer: %d\n", res);
            }
            */
            res = i2s_start(I2S_PORT);
            if (res != ESP_OK) {
                LOG_ERROR("Failed to start I2S: %d\n", res);
            }

            // Configure PTT input
            LOG_INFO("Initializing PTT on Pin %u\n", moduleConfig.audio_config.ptt_pin ? moduleConfig.audio_config.ptt_pin : PTT_PIN);
            pinMode(moduleConfig.audio_config.ptt_pin ? moduleConfig.audio_config.ptt_pin : PTT_PIN, INPUT);

            firstTime = false;
        } else {
            UIFrameEvent e = {false, true};
            // Check if PTT is pressed. TODO hook that into Onebutton/Interrupt drive.
            if (digitalRead(moduleConfig.audio_config.ptt_pin ? moduleConfig.audio_config.ptt_pin : PTT_PIN) == HIGH) {
                if (radio_state == SpeexRadioState::speex_rx) {
                    LOG_INFO("PTT pressed, switching to TX\n");
                    radio_state = SpeexRadioState::speex_tx;
                    e.frameChanged = true;
                    this->notifyObservers(&e);
                }
            } else {
                if (radio_state == SpeexRadioState::speex_tx) {
                    LOG_INFO("PTT released, switching to RX\n");
                    if (tx_encode_frame_index > sizeof(tx_header_t)) {
                        // Send the incomplete frame
                        LOG_INFO("Sending %d speex bytes (incomplete)\n", tx_encode_frame_index);
                        sendPayload();
                    }
                    tx_encode_frame_index = sizeof(tx_header_t);
                    radio_state = SpeexRadioState::speex_rx;
                    e.frameChanged = true;
                    this->notifyObservers(&e);
                }
            }
            if (radio_state == SpeexRadioState::speex_tx) {
                // Get I2S data from the microphone and place in data buffer
                size_t bytesIn = 0;
                res = i2s_read(I2S_PORT, adc_buffer + adc_buffer_index, ADC_BUF_SIZE_IN_BYTES - adc_buffer_index, &bytesIn,
                               pdMS_TO_TICKS(40)); // wait 40ms(2 of 20ms frame time) for audio to arrive.
                if (res == ESP_OK) {
                    millis_rd = esp_millis();
                    LOG_INFO("i2s_read:  res=%d, bytesIn=%d,  adc_buffer_index=%d, ADC_BUF_SIZE_IN_BYTES=%d, esp_millis()=%lu, dt_millis()=%lu\n", res, bytesIn, adc_buffer_index, ADC_BUF_SIZE_IN_BYTES, millis_rd, millis_rd-_millis_rd);
                    _millis_rd = millis_rd;

                    adc_buffer_index += bytesIn;
                    if (adc_buffer_index == ADC_BUF_SIZE_IN_BYTES) {

                        #ifdef SELF_LISTENING_I2S
                        size_t bytesOut = 0;
                        res = i2s_write(I2S_PORT, adc_buffer, ADC_BUF_SIZE_IN_BYTES, &bytesOut, pdMS_TO_TICKS(40));
                        millis_wr = esp_millis();
                        LOG_INFO("i2s_write: res=%d, bytesOut=%d, ADC_BUF_SIZE_IN_BYTES=%d, esp_millis()=%lu dt_millis()=%lu\n", res, bytesOut, ADC_BUF_SIZE_IN_BYTES, millis_wr, millis_wr - _millis_wr);
                        _millis_wr = millis_wr;
                        if (res != ESP_OK) {
                            LOG_ERROR("i2s_write: result %d\n", res);
                        }
                        #endif

                        adc_buffer_index = 0;
                        memcpy((void *)speech, (void *)adc_buffer, ADC_BUF_SIZE_IN_BYTES);

                        // Notify run_speex task that the buffer is ready.
                        radio_state = SpeexRadioState::speex_tx;
                        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                        vTaskNotifyGiveFromISR(speexHandlerTask, &xHigherPriorityTaskWoken);
                        if (xHigherPriorityTaskWoken == pdTRUE)
                            YIELD_FROM_ISR(xHigherPriorityTaskWoken);
                    }
                } else {
                    LOG_ERROR("i2s_read result %d\n", res);
                }
            }
        }
        return 100;
    } else {
        return disable();
    }
}

meshtastic_MeshPacket *SpeexModule::allocReply()
{
    auto reply = allocDataPacket();
    return reply;
}

bool SpeexModule::shouldDraw()
{
    if (moduleConfig.audio_config.codec != meshtastic_ModuleConfig_Audio_Config_Audio_Codec_CODEC_SPEEX) {
        return false;
    }
    return (radio_state == SpeexRadioState::speex_tx);
}

void SpeexModule::sendPayload(NodeNum dest, bool wantReplies)
{
    meshtastic_MeshPacket *p = allocReply();
    p->to = dest;
    p->decoded.want_response = wantReplies;

    p->want_ack = false;                              // Audio is shoot&forget. No need to wait for ACKs.
    p->priority = meshtastic_MeshPacket_Priority_MAX; // Audio is important, because realtime

    p->decoded.payload.size = tx_encode_frame_index;
    memcpy(p->decoded.payload.bytes, tx_encode_frame, p->decoded.payload.size);

    service.sendToMesh(p);
}

ProcessMessage SpeexModule::handleReceived(const meshtastic_MeshPacket &mp)
{
    if ((moduleConfig.audio_config.codec == meshtastic_ModuleConfig_Audio_Config_Audio_Codec_CODEC_SPEEX) && (myRegion->audioPermitted)) {
        auto &p = mp.decoded;
        if (getFrom(&mp) != nodeDB.getNodeNum()) {
            memcpy(rx_encode_frame, p.payload.bytes, p.payload.size);
            rx_encode_frame_index = p.payload.size;

            // Notify run_speex task that the buffer is ready.
            radio_state = SpeexRadioState::speex_rx;
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(speexHandlerTask, &xHigherPriorityTaskWoken);
            if (xHigherPriorityTaskWoken == pdTRUE)
                YIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    return ProcessMessage::CONTINUE;
}

#endif
#endif
