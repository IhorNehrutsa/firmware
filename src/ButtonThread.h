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
#ifdef BUTTON_CENTER
    OneButton userButtonUp;
    OneButton userButtonLeft;
    OneButton userButtonCenter;
    OneButton userButtonRight;
    OneButton userButtonDown;
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
        userButtonUp     = OneButton(BUTTON_UP, true, true);
        userButtonLeft   = OneButton(BUTTON_LEFT, true, true);
        userButtonCenter = OneButton(BUTTON_CENTER, true, true);
        userButtonRight  = OneButton(BUTTON_RIGHT, true, true);
        userButtonDown   = OneButton(BUTTON_DOWN, true, true);

        userButtonUp    .attachClick(userButtonUpPressed, &userButtonUp);
        userButtonLeft  .attachClick(userButtonLeftPressed, &userButtonLeft);
        userButtonCenter.attachClick(userButtonCenterPressed, &userButtonCenter);
        userButtonRight .attachClick(userButtonRightPressed, &userButtonRight);
        userButtonDown  .attachClick(userButtonDownPressed, &userButtonDown);

        userButtonUp    .attachRelease(userButtonUpReleased, &userButtonUp);
        userButtonLeft  .attachRelease(userButtonLeftReleased, &userButtonLeft);
        userButtonCenter.attachRelease(userButtonCenterReleased, &userButtonCenter);
        userButtonRight .attachRelease(userButtonRightReleased, &userButtonRight);
        userButtonDown  .attachRelease(userButtonDownReleased, &userButtonDown);

        userButtonUp    .attachDoubleClick(userButtonCenterDoublePressed, &userButtonUp);
        /*
        userButtonLeft  .attachDoubleClick(userButtonCenterDoublePressed);
        userButtonCenter.attachDoubleClick(userButtonCenterDoublePressed);
        userButtonRight .attachDoubleClick(userButtonCenterDoublePressed);
        userButtonDown  .attachDoubleClick(userButtonCenterDoublePressed);
        */
        #define MS 300
        userButtonUp    .setClickTicks(MS);
        userButtonLeft  .setClickTicks(MS);
        userButtonCenter.setClickTicks(MS);
        userButtonRight .setClickTicks(MS);
        userButtonDown  .setClickTicks(MS);
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
        userButtonUp.tick();
        userButtonLeft.tick();
        userButtonCenter.tick();
        userButtonRight.tick();
        userButtonDown.tick();
        canSleep &= userButtonUp.isIdle();
        canSleep &= userButtonLeft.isIdle();
        canSleep &= userButtonCenter.isIdle();
        canSleep &= userButtonRight.isIdle();
        canSleep &= userButtonDown.isIdle();
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

    static void sendToPhone(OneButton *oneButton)
    {
        LOG_DEBUG("sendToPhone pin=%d state=%d\n", oneButton->pin(), oneButton->state());

        meshtastic_PtdButtons b;
        memset(&b, 0, sizeof(b));
        b.button_pin = oneButton->pin();
        b.button_state = oneButton->state();

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

//#define PRESS 1
//#define RELEASE 2
    // Pressed
    static void userButtonUpPressed(void *oneButton)
    {
        LOG_DEBUG("Up Pressed\n");
        sendToPhone((OneButton *)oneButton);
    }

    static void userButtonLeftPressed(void *oneButton)
    {
        LOG_DEBUG("Left Pressed\n");
        sendToPhone((OneButton *)oneButton);
    }

    static void userButtonCenterPressed(void *oneButton)
    {
        LOG_DEBUG("Center Pressed\n");
        sendToPhone((OneButton *)oneButton);
    }

    static void userButtonRightPressed(void *oneButton)
    {
        LOG_DEBUG("Right Pressed\n");
        sendToPhone((OneButton *)oneButton);
    }

    static void userButtonDownPressed(void *oneButton)
    {
        LOG_DEBUG("Down Pressed\n");
        sendToPhone((OneButton *)oneButton);
    }

    // Released
    static void userButtonUpReleased(void *oneButton)
    {
        LOG_DEBUG("Up Released\n");
        sendToPhone((OneButton *)oneButton);
    }

    static void userButtonLeftReleased(void *oneButton)
    {
        LOG_DEBUG("Left Released\n");
        sendToPhone((OneButton *)oneButton);
    }

    static void userButtonCenterReleased(void *oneButton)
    {
        LOG_DEBUG("Center Released\n");
        sendToPhone((OneButton *)oneButton);
    }

    static void userButtonRightReleased(void *oneButton)
    {
        LOG_DEBUG("Right Released\n");
        sendToPhone((OneButton *)oneButton);
    }

    static void userButtonDownReleased(void *oneButton)
    {
        LOG_DEBUG("Down Released\n");
        sendToPhone((OneButton *)oneButton);
    }

    // DoublePressed
    static void userButtonCenterDoublePressed(void *oneButton)
    {
        LOG_DEBUG("Center Double Pressed\n");
        sendToPhone((OneButton *)oneButton);
    }
};

} // namespace concurrency
