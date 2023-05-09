#include "PowerFSM.h"
#include "RadioLibInterface.h"
#include "buzz.h"
#include "concurrency/OSThread.h"
#include "configuration.h"
#include "graphics/Screen.h"
#include "power.h"
#include <OneButton.h>
//#include "MeshService.h"
#include "ProtobufModule.h"

extern OneButton *userButtonUp;
extern OneButton *userButtonLe;
extern OneButton *userButtonCe;
extern OneButton *userButtonRi;
extern OneButton *userButtonDo;

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
        userButton.setClickTicks(300);
        userButton.attachDuringLongPress(userButtonPressedLong);
        userButton.attachDoubleClick(userButtonDoublePressed);
        userButton.attachMultiClick(userButtonMultiPressed);
        userButton.attachLongPressStart(userButtonPressedLongStart);
        userButton.attachLongPressStop(userButtonPressedLongStop);
        wakeOnIrq(config.device.button_gpio ? config.device.button_gpio : BUTTON_PIN, FALLING);
#endif
#ifdef BUTTON_CENTER
        userButtonUp = new OneButton(BUTTON_UP, true, true);
        userButtonLe = new OneButton(BUTTON_LEFT, true, true);
        userButtonCe = new OneButton(BUTTON_CENTER, true, true);
        userButtonRi = new OneButton(BUTTON_RIGHT, true, true);
        userButtonDo = new OneButton(BUTTON_DOWN, true, true);

        userButtonUp->attachClick(userButtonUpClick, userButtonUp);
        userButtonLe->attachClick(userButtonLeClick, userButtonLe);
        userButtonCe->attachClick(userButtonCeClick, userButtonCe);
        userButtonRi->attachClick(userButtonRiClick, userButtonRi);
        userButtonDo->attachClick(userButtonDoClick, userButtonDo);

        userButtonUp->attachDoubleClick(userButtonUpDoubleClick, userButtonUp);
        userButtonLe->attachDoubleClick(userButtonLeDoubleClick, userButtonLe);
        userButtonCe->attachDoubleClick(userButtonCeDoubleClick, userButtonCe);
        userButtonRi->attachDoubleClick(userButtonRiDoubleClick, userButtonRi);
        userButtonDo->attachDoubleClick(userButtonDoDoubleClick, userButtonDo);

        userButtonUp->attachMultiClick(userButtonUpMultiClick, userButtonUp);
        userButtonLe->attachMultiClick(userButtonLeMultiClick, userButtonLe);
        userButtonCe->attachMultiClick(userButtonCeMultiClick, userButtonCe);
        userButtonRi->attachMultiClick(userButtonRiMultiClick, userButtonRi);
        userButtonDo->attachMultiClick(userButtonDoMultiClick, userButtonDo);

        userButtonUp->attachLongPressStart(userButtonUpLongPressStart, userButtonUp);
        userButtonLe->attachLongPressStart(userButtonLeLongPressStart, userButtonLe);
        userButtonCe->attachLongPressStart(userButtonCeLongPressStart, userButtonCe);
        userButtonRi->attachLongPressStart(userButtonRiLongPressStart, userButtonRi);
        userButtonDo->attachLongPressStart(userButtonDoLongPressStart, userButtonDo);

        userButtonUp->attachLongPressStop(userButtonUpLongPressStop, userButtonUp);
        userButtonLe->attachLongPressStop(userButtonLeLongPressStop, userButtonLe);
        userButtonCe->attachLongPressStop(userButtonCeLongPressStop, userButtonCe);
        userButtonRi->attachLongPressStop(userButtonRiLongPressStop, userButtonRi);
        userButtonDo->attachLongPressStop(userButtonDoLongPressStop, userButtonDo);

        #define MS 300
        userButtonUp->setClickTicks(MS);
        userButtonLe->setClickTicks(MS);
        userButtonCe->setClickTicks(MS);
        userButtonRi->setClickTicks(MS);
        userButtonDo->setClickTicks(MS);
        wakeOnIrq(BUTTON_CENTER, FALLING);
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
#ifdef BUTTON_CENTER
        userButtonUp->tick();
        userButtonLe->tick();
        userButtonCe->tick();
        userButtonRi->tick();
        userButtonDo->tick();
        canSleep &= userButtonUp->isIdle();
        canSleep &= userButtonLe->isIdle();
        canSleep &= userButtonCe->isIdle();
        canSleep &= userButtonRi->isIdle();
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

    static void sendToPhone(OneButton *oneButton, meshtastic_PtdButtonEvent event)
    {
        meshtastic_PtdButtons b;
        memset(&b, 0, sizeof(b));
        b.button_pin = oneButton->pin();
        b.event = event;
        b.buttons_states = ((uint64_t)userButtonUp->debouncedValue() << userButtonUp->pin())
                         | ((uint64_t)userButtonLe->debouncedValue() << userButtonLe->pin())
                         | ((uint64_t)userButtonCe->debouncedValue() << userButtonCe->pin())
                         | ((uint64_t)userButtonRi->debouncedValue() << userButtonRi->pin())
                         | ((uint64_t)userButtonDo->debouncedValue() << userButtonDo->pin());

        LOG_DEBUG("sendToPhone button_pin=%u=0x%08X event=%d buttons_states=0x%08X\n", b.button_pin, 1 << b.button_pin, b.event, b.buttons_states);

        meshtastic_MeshPacket *p = router->allocForSending();
        p->to = p->from;
        p->decoded.portnum = meshtastic_PortNum_PTD_APP;
        p->decoded.want_response = false;
        p->decoded.payload.size = sizeof(meshtastic_PtdButtons);
        memcpy(p->decoded.payload.bytes, &b, p->decoded.payload.size);
        p->priority = meshtastic_MeshPacket_Priority_MIN;
        p->hop_limit = 0;

        service.sendToPhone(p);
    }

    // Click
    static void userButtonUpClick(void *oneButton)
    {
        LOG_DEBUG("Up Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_CLICK);
    }

    static void userButtonLeClick(void *oneButton)
    {
        LOG_DEBUG("Left Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_CLICK);
    }

    static void userButtonCeClick(void *oneButton)
    {
        LOG_DEBUG("Center Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_CLICK);
    }

    static void userButtonRiClick(void *oneButton)
    {
        LOG_DEBUG("Right Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_CLICK);
    }

    static void userButtonDoClick(void *oneButton)
    {
        LOG_DEBUG("Down Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_CLICK);
    }

    // DoubleClick
    static void userButtonUpDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Up Double Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    static void userButtonLeDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Left Double Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    static void userButtonCeDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Center Double Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    static void userButtonRiDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Right Double Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    static void userButtonDoDoubleClick(void *oneButton)
    {
        LOG_DEBUG("Down Double Click\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_DOUBLE_CLICK);
    }

    // MultiClick
    static void userButtonUpMultiClick(void *oneButton)
    {
        LOG_DEBUG("Up MultiClick\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    static void userButtonLeMultiClick(void *oneButton)
    {
        LOG_DEBUG("Left MultiClick\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    static void userButtonCeMultiClick(void *oneButton)
    {
        LOG_DEBUG("Center MultiClick\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    static void userButtonRiMultiClick(void *oneButton)
    {
        LOG_DEBUG("Right MultiClick\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    static void userButtonDoMultiClick(void *oneButton)
    {
        LOG_DEBUG("Down MultiClick\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_MULTI_CLICK);
    }

    // LongPressStart
    static void userButtonUpLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Up LongPressStart\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    static void userButtonLeLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Left LongPressStart\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    static void userButtonCeLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Center LongPressStart\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    static void userButtonRiLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Right LongPressStart\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    static void userButtonDoLongPressStart(void *oneButton)
    {
        LOG_DEBUG("Down LongPressStart\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_START);
    }

    // LongPressStop
    static void userButtonUpLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Up LongPressStop\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

    static void userButtonLeLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Left LongPressStop\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

    static void userButtonCeLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Center LongPressStop\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

    static void userButtonRiLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Right LongPressStop\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

    static void userButtonDoLongPressStop(void *oneButton)
    {
        LOG_DEBUG("Down LongPressStop\n");
        sendToPhone((OneButton *)oneButton, meshtastic_PtdButtonEvent_EVENT_LONG_PRESS_STOP);
    }

};

} // namespace concurrency
