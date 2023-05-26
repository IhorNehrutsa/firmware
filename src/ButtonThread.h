#include "PowerFSM.h"
#include "RadioLibInterface.h"
#include "buzz.h"
#include "concurrency/OSThread.h"
#include "configuration.h"
#include "graphics/Screen.h"
#include "power.h"
#include <OneButton.h>
#include "ProtobufModule.h"

#ifdef BUTTON_UP
extern OneButton *userButtonUp;
#endif
#ifdef BUTTON_LEFT
extern OneButton *userButtonLe;
#endif
#ifdef BUTTON_CENTER
extern OneButton *userButtonCe;
#endif
#ifdef BUTTON_RIGHT
extern OneButton *userButtonRi;
#endif
#ifdef BUTTON_DOWN
extern OneButton *userButtonDo;
#endif

namespace concurrency
{
/**
 * Watch a GPIO and if we get an IRQ, wake the main thread.
 * Use to add wake on button press
 */
void wakeOnIrq(int irq, int mode)
{
    attachInterrupt(
        irq,
        [] {
            BaseType_t higherWake = 0;
            mainDelay.interruptFromISR(&higherWake);
        },
        mode);
}

class ButtonThread : public concurrency::OSThread
{
// Prepare for button presses
#ifdef BUTTON_PIN
    OneButton userButton;
#endif
#ifdef BUTTON_PIN_ALT
    OneButton userButtonAlt;
#endif
#ifdef BUTTON_PIN_TOUCH
    OneButton userButtonTouch;
#endif
    static bool shutdown_on_long_stop;

  public:
    static uint32_t longPressTime;

    // callback returns the period for the next callback invocation (or 0 if we should no longer be called)
    ButtonThread() : OSThread("Button")
    {
#ifdef BUTTON_PIN
        userButton = OneButton(config.device.button_gpio ? config.device.button_gpio : BUTTON_PIN, true, true);
#ifdef INPUT_PULLUP_SENSE
        // Some platforms (nrf52) have a SENSE variant which allows wake from sleep - override what OneButton did
        pinMode(config.device.button_gpio ? config.device.button_gpio : BUTTON_PIN, INPUT_PULLUP_SENSE);
#endif
        userButton.attachClick(userButtonPressed);
        userButton.setClickMs(300);
        userButton.attachDuringLongPress(userButtonPressedLong);
        userButton.attachDoubleClick(userButtonDoublePressed);
        userButton.attachMultiClick(userButtonMultiPressed);
        userButton.attachLongPressStart(userButtonPressedLongStart);
        userButton.attachLongPressStop(userButtonPressedLongStop);
        wakeOnIrq(config.device.button_gpio ? config.device.button_gpio : BUTTON_PIN, FALLING);
#endif

        #define MS 300
#ifdef BUTTON_UP
        userButtonUp = new OneButton(BUTTON_UP, false, false);
        userButtonUp->setClickMs(MS);
        userButtonUp->attachClick(userButtonUpClick, userButtonUp);
        userButtonUp->attachDoubleClick(userButtonUpDoubleClick, userButtonUp);
        userButtonUp->attachMultiClick(userButtonUpMultiClick, userButtonUp);
        userButtonUp->attachLongPressStart(userButtonUpLongPressStart, userButtonUp);
        userButtonUp->attachLongPressStop(userButtonUpLongPressStop, userButtonUp);
#endif
#ifdef BUTTON_LEFT
        userButtonLe = new OneButton(BUTTON_LEFT, false, false);
        userButtonLe->setClickMs(MS);
        userButtonLe->attachClick(userButtonLeClick, userButtonLe);
        userButtonLe->attachDoubleClick(userButtonLeDoubleClick, userButtonLe);
        userButtonLe->attachMultiClick(userButtonLeMultiClick, userButtonLe);
        userButtonLe->attachLongPressStart(userButtonLeLongPressStart, userButtonLe);
        userButtonLe->attachLongPressStop(userButtonLeLongPressStop, userButtonLe);
#endif
#ifdef BUTTON_CENTER
        userButtonCe = new OneButton(BUTTON_CENTER, false, false);
        userButtonCe->setClickMs(MS);
        userButtonCe->attachClick(userButtonCeClick, userButtonCe);
        userButtonCe->attachDoubleClick(userButtonCeDoubleClick, userButtonCe);
        userButtonCe->attachMultiClick(userButtonCeMultiClick, userButtonCe);
        userButtonCe->attachLongPressStart(userButtonCeLongPressStart, userButtonCe);
        userButtonCe->attachLongPressStop(userButtonCeLongPressStop, userButtonCe);
        wakeOnIrq(BUTTON_CENTER, RISING);
#endif
#ifdef BUTTON_RIGHT
        userButtonRi = new OneButton(BUTTON_RIGHT, false, false);
        userButtonRi->setClickMs(MS);
        userButtonRi->attachClick(userButtonRiClick, userButtonRi);
        userButtonRi->attachDoubleClick(userButtonRiDoubleClick, userButtonRi);
        userButtonRi->attachMultiClick(userButtonRiMultiClick, userButtonRi);
        userButtonRi->attachLongPressStart(userButtonRiLongPressStart, userButtonRi);
        userButtonRi->attachLongPressStop(userButtonRiLongPressStop, userButtonRi);
#endif
#ifdef BUTTON_DOWN
        userButtonDo = new OneButton(BUTTON_DOWN, false, false);
        userButtonDo->setClickMs(MS);
        userButtonDo->attachClick(userButtonDoClick, userButtonDo);
        userButtonDo->attachDoubleClick(userButtonDoDoubleClick, userButtonDo);
        userButtonDo->attachMultiClick(userButtonDoMultiClick, userButtonDo);
        userButtonDo->attachLongPressStart(userButtonDoLongPressStart, userButtonDo);
        userButtonDo->attachLongPressStop(userButtonDoLongPressStop, userButtonDo);
#endif

#ifdef BUTTON_PIN_ALT
        userButtonAlt = OneButton(BUTTON_PIN_ALT, true, true);
#ifdef INPUT_PULLUP_SENSE
        // Some platforms (nrf52) have a SENSE variant which allows wake from sleep - override what OneButton did
        pinMode(BUTTON_PIN_ALT, INPUT_PULLUP_SENSE);
#endif
        userButtonAlt.attachClick(userButtonPressed);
        userButtonAlt.attachDuringLongPress(userButtonPressedLong);
        userButtonAlt.attachDoubleClick(userButtonDoublePressed);
        userButtonAlt.attachLongPressStart(userButtonPressedLongStart);
        userButtonAlt.attachLongPressStop(userButtonPressedLongStop);
        wakeOnIrq(BUTTON_PIN_ALT, FALLING);
#endif

#ifdef BUTTON_PIN_TOUCH
        userButtonTouch = OneButton(BUTTON_PIN_TOUCH, true, true);
        userButtonTouch.attachClick(touchPressed);
        wakeOnIrq(BUTTON_PIN_TOUCH, FALLING);
#endif
    }

  protected:
    /// If the button is pressed we suppress CPU sleep until release
    int32_t runOnce() override
    {
        canSleep = true; // Assume we should not keep the board awake

#ifdef BUTTON_PIN
        userButton.tick();
        canSleep &= userButton.isIdle();
#endif
#ifdef BUTTON_UP
        userButtonUp->tick();
        canSleep &= userButtonUp->isIdle();
#endif
#ifdef BUTTON_LEFT
        userButtonLe->tick();
        canSleep &= userButtonLe->isIdle();
#endif
#ifdef BUTTON_CENTER
        userButtonCe->tick();
        canSleep &= userButtonCe->isIdle();
#endif
#ifdef BUTTON_RIGHT
        userButtonRi->tick();
        canSleep &= userButtonRi->isIdle();
#endif
#ifdef BUTTON_DOWN
        userButtonDo->tick();
        canSleep &= userButtonDo->isIdle();
#endif
#ifdef BUTTON_PIN_ALT
        userButtonAlt.tick();
        canSleep &= userButtonAlt.isIdle();
#endif
#ifdef BUTTON_PIN_TOUCH
        userButtonTouch.tick();
        canSleep &= userButtonTouch.isIdle();
#endif
        // if (!canSleep) LOG_DEBUG("Supressing sleep!\n");
        // else LOG_DEBUG("sleep ok\n");

        return 5;
    }

  private:
#ifdef BUTTON_PIN_TOUCH
    static void touchPressed()
    {
        screen->forceDisplay();
        LOG_DEBUG("touch press!\n");
    }
#endif

    static void userButtonPressed()
    {
        LOG_DEBUG("press!\n");
#ifdef BUTTON_PIN
        if (((config.device.button_gpio ? config.device.button_gpio : BUTTON_PIN) !=
             moduleConfig.canned_message.inputbroker_pin_press) ||
            !moduleConfig.canned_message.enabled) {
            powerFSM.trigger(EVENT_PRESS);
        }
#endif
    }

    static void userButtonPressedLong()
    {
        LOG_DEBUG("Long press!\n");
#ifdef ARCH_ESP32
        screen->adjustBrightness();
#endif
        // If user button is held down for 5 seconds, shutdown the device.
        if ((millis() - longPressTime > 5000) && (longPressTime > 0)) {
#if defined(ARCH_NRF52) || defined(ARCH_ESP32)
            // Do actual shutdown when button released, otherwise the button release
            // may wake the board immediatedly.
            if ((!shutdown_on_long_stop) && (millis() > 30 * 1000)) {
                screen->startShutdownScreen();
                LOG_INFO("Shutdown from long press");
                playBeep();
#ifdef PIN_LED1
                ledOff(PIN_LED1);
#endif
#ifdef PIN_LED2
                ledOff(PIN_LED2);
#endif
#ifdef PIN_LED3
                ledOff(PIN_LED3);
#endif
                shutdown_on_long_stop = true;
            }
#endif
        } else {
            LOG_DEBUG("Long press %u\n", (millis() - longPressTime));
        }
    }

    static void userButtonDoublePressed()
    {
#if defined(USE_EINK) && defined(PIN_EINK_EN)
        digitalWrite(PIN_EINK_EN, digitalRead(PIN_EINK_EN) == LOW);
#endif
        screen->print("Sent ad-hoc ping\n");
        service.refreshMyNodeInfo();
        service.sendNetworkPing(NODENUM_BROADCAST, true);
    }

    static void userButtonMultiPressed()
    {
#if defined(GPS_POWER_TOGGLE)
        if (config.position.gps_enabled) {
            LOG_DEBUG("Flag set to false for gps power\n");
        } else {
            LOG_DEBUG("Flag set to true to restore power\n");
        }
        config.position.gps_enabled = !(config.position.gps_enabled);
        doGPSpowersave(config.position.gps_enabled);
#endif
    }

    static void userButtonPressedLongStart()
    {
        if (millis() > 30 * 1000) {
            LOG_DEBUG("Long press start!\n");
            longPressTime = millis();
        }
    }

    static void userButtonPressedLongStop()
    {
        if (millis() > 30 * 1000) {
            LOG_DEBUG("Long press stop!\n");
            longPressTime = 0;
            if (shutdown_on_long_stop) {
                playShutdownMelody();
                delay(3000);
                power->shutdown();
            }
        }
    }

    static void _sendToPhone(OneButton *oneButton, meshtastic_PtdButtons_PtdButtonEvent event)
    {
        meshtastic_PtdButtons b;
        memset(&b, 0, sizeof(b));
        switch (oneButton->pin()) {
            case BUTTON_UP:
                b.button = meshtastic_PtdButtons_PtdButtonId_BUTTON_UP;
                break;
            case BUTTON_LEFT:
                b.button = meshtastic_PtdButtons_PtdButtonId_BUTTON_LEFT;
                break;
            case BUTTON_CENTER:
                b.button = meshtastic_PtdButtons_PtdButtonId_BUTTON_CENTER;
                break;
            case BUTTON_RIGHT:
                b.button = meshtastic_PtdButtons_PtdButtonId_BUTTON_RIGHT;
                break;
            case BUTTON_DOWN:
                b.button = meshtastic_PtdButtons_PtdButtonId_BUTTON_DOWN;
                break;
        }
        b.event = event;

        b.buttons_states = 0;
        #ifdef BUTTON_UP
        b.buttons_states |= userButtonUp->debouncedValue() ? meshtastic_PtdButtons_PtdButtonId_BUTTON_UP : 0;
        #endif
        #ifdef BUTTON_LEFT
        b.buttons_states |= userButtonLe->debouncedValue() ? meshtastic_PtdButtons_PtdButtonId_BUTTON_LEFT : 0;
        #endif
        #ifdef BUTTON_CENTER
        b.buttons_states |= userButtonCe->debouncedValue() ? meshtastic_PtdButtons_PtdButtonId_BUTTON_CENTER : 0;
        #endif
        #ifdef BUTTON_RIGHT
        b.buttons_states |= userButtonRi->debouncedValue() ? meshtastic_PtdButtons_PtdButtonId_BUTTON_RIGHT : 0;
        #endif
        #ifdef BUTTON_DOWN
        b.buttons_states |= userButtonDo->debouncedValue() ? meshtastic_PtdButtons_PtdButtonId_BUTTON_DOWN : 0;
        #endif

        LOG_DEBUG("sendToPhone button_pin=%u=0x%08X event=%d buttons_states=0x%08X\n", b.button, b.button, b.event, b.buttons_states);

        // LOG_DEBUG("sendToPhone button_pin=%u=0x%08X event=%d\n", b.button, b.button, b.event);

        meshtastic_MeshPacket *p = router->allocForSending();
        p->to = p->from;
        p->decoded.portnum = meshtastic_PortNum_PTD_APP;
        p->decoded.want_response = false;
        p->decoded.payload.size = sizeof(meshtastic_PtdButtons);
        memcpy(p->decoded.payload.bytes, &b, p->decoded.payload.size);
        p->priority = meshtastic_MeshPacket_Priority_MIN;
        p->hop_limit = 0;

        // service.sendToPhone(p);
        service.sendToMesh(p, RX_SRC_LOCAL, true);
    }

    // Click
    static void userButtonUpClick(void *oneButton)
    {
        LOG_DEBUG("Up Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_CLICK);
    }

    static void userButtonLeClick(void *oneButton)
    {
        LOG_DEBUG("Left Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_CLICK);
    }

    static void userButtonCeClick(void *oneButton)
    {
        LOG_DEBUG("Center Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_CLICK);
    }

    static void userButtonRiClick(void *oneButton)
    {
        LOG_DEBUG("Right Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_CLICK);
    }

    static void userButtonDoClick(void *oneButton)
    {
        LOG_DEBUG("Down Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_CLICK);
    }

    // DoubleClick
    static void userButtonUpDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Up Double Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    static void userButtonLeDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Left Double Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    static void userButtonCeDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Center Double Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    static void userButtonRiDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Right Double Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    static void userButtonDoDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Down Double Click\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    // MultiClick
    static void userButtonUpMultiClick(void *oneButton)
    {
        LOG_DEBUG("Up MultiClick\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    static void userButtonLeMultiClick(void *oneButton)
    {
        LOG_DEBUG("Left MultiClick\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    static void userButtonCeMultiClick(void *oneButton)
    {
        LOG_DEBUG("Center MultiClick\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    static void userButtonRiMultiClick(void *oneButton)
    {
        LOG_DEBUG("Right MultiClick\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    static void userButtonDoMultiClick(void *oneButton)
    {
        LOG_DEBUG("Down MultiClick\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    // LongPressStart
    static void userButtonUpLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Up LongPressStart\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    static void userButtonLeLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Left LongPressStart\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    static void userButtonCeLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Center LongPressStart\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    static void userButtonRiLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Right LongPressStart\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    static void userButtonDoLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Down LongPressStart\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    // LongPressStop
    static void userButtonUpLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Up LongPressStop\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

    static void userButtonLeLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Left LongPressStop\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

    static void userButtonCeLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Center LongPressStop\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

    static void userButtonRiLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Right LongPressStop\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

    static void userButtonDoLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Down LongPressStop\n");
        _sendToPhone((OneButton *)oneButton, meshtastic_PtdButtons_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

};

} // namespace concurrency
