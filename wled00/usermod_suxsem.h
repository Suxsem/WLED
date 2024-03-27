#pragma once

#include "wled.h"

#define SUXSEM_AC_BTN_PIN             0
#define SUXSEM_SHORT_MAX_MS         600
#define SUXSEM_DEBOUNCE_THRESHOLD    50
#define SUXSEM_INVERT_PRESSED     false
#define SUXSEM_DIMMING_TICK_MS        8
#define SUXSEM_FADEOUT_MS          2000

class UsermodSuxsem : public Usermod {
  private:

    static const char _name[];
    static const char _enabled[];
    unsigned short btnPin = SUXSEM_AC_BTN_PIN;
    unsigned int shortMaxMs = SUXSEM_SHORT_MAX_MS;
    unsigned int debounceMs = SUXSEM_DEBOUNCE_THRESHOLD;
    bool invertPressed = SUXSEM_INVERT_PRESSED;
    unsigned int dimmingTickMs = SUXSEM_DIMMING_TICK_MS;
    unsigned int fadeoutMs = SUXSEM_FADEOUT_MS;
    
    bool enabled = false;

    unsigned long buttonPressedTime = 0;
    unsigned long lastDimmTime = 0;
    bool alreadyTurnedOn = false;
    int direzione = 1;
    bool dimming = false;

  public:

    inline void enable(bool enable) { enabled = enable; }
    inline bool isEnabled() { return enabled; }

    void setup() {
      pinMode(btnPin, INPUT);
    }

    void loop() {
      if (!enabled || strip.isUpdating()) return;

      unsigned long now = millis();
      unsigned long elapsed = now - buttonPressedTime;

      if (buttonPressedTime == 0 && digitalRead(btnPin) != invertPressed) { //premo per la prima volta
        buttonPressedTime = now;
      } else if (buttonPressedTime != 0 && elapsed > debounceMs && !alreadyTurnedOn && bri == 0) { //effettivamente premuto
        //TURN ON
        jsonTransitionOnce = true;
        strip.setTransition(0);
        bri = 255;
        stateUpdated(CALL_MODE_BUTTON);
        alreadyTurnedOn = true;
      } else if (buttonPressedTime != 0 && digitalRead(btnPin) == invertPressed) { //rilascio
        if (!alreadyTurnedOn && elapsed <= shortMaxMs && elapsed > debounceMs && bri != 0) {
          jsonTransitionOnce = true;
          strip.setTransition(fadeoutMs);
          bri = 0;
          stateUpdated(CALL_MODE_BUTTON);
        }
        dimming = false;
        alreadyTurnedOn = false;
        buttonPressedTime = 0;
      } else if (buttonPressedTime != 0 && elapsed > shortMaxMs && !dimming) { //continuo a premere
        //DIMMING
        dimming = true;
      }

      if (dimming && (now - lastDimmTime) > dimmingTickMs) {
        if (bri == 255 && direzione == 1) {
          direzione = -1;
        } else if (bri <= 1 && direzione == -1) {
          direzione = 1;
        }
        jsonTransitionOnce = true;
        strip.setTransition(0);
        bri += direzione;
        stateUpdated(CALL_MODE_BUTTON);
        lastDimmTime = now;
      }

    }

    void addToConfig(JsonObject& root) override
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;
      top["btnPin"] = btnPin;
      top["shortMaxMs"] = shortMaxMs;
      top["debounceMs"] = debounceMs;
      top["invertPressed"] = invertPressed;
      top["dimmingTickMs"] = dimmingTickMs;
      top["fadeoutMs"] = fadeoutMs;
    }

    bool readFromConfig(JsonObject& root) override
    {
      JsonObject top = root[FPSTR(_name)];
      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled, false);
      configComplete &= getJsonValue(top["btnPin"], btnPin, SUXSEM_AC_BTN_PIN);
      configComplete &= getJsonValue(top["shortMaxMs"], shortMaxMs, SUXSEM_SHORT_MAX_MS);
      configComplete &= getJsonValue(top["debounceMs"], debounceMs, SUXSEM_DEBOUNCE_THRESHOLD);
      configComplete &= getJsonValue(top["invertPressed"], invertPressed, SUXSEM_INVERT_PRESSED);
      configComplete &= getJsonValue(top["dimmingTickMs"], dimmingTickMs, SUXSEM_DIMMING_TICK_MS);
      configComplete &= getJsonValue(top["fadeoutMs"], fadeoutMs, SUXSEM_FADEOUT_MS);
      return configComplete;
    }
};

const char UsermodSuxsem::_name[]    PROGMEM = "SuxsemUsermod";
const char UsermodSuxsem::_enabled[] PROGMEM = "enabled";
