#pragma once

#include "wled.h"

#define SUXSEM_AC_BTN_PIN             0
#define SUXSEM_SHORT_MAX_MS         600
#define SUXSEM_DEBOUNCE_THRESHOLD    50
#define SUXSEM_PRESSED_LEVEL        LOW
#define SUXSEM_DIMMING_TICK_MS        8

class UsermodSuxsem : public Usermod {
  private:
    unsigned long buttonPressedTime = 0;
    unsigned long lastDimmTime = 0;
    bool alreadyTurnedOn = false;
    int direzione = 1;
    bool dimming = false;

  public:

    void setup() {
      pinMode(SUXSEM_AC_BTN_PIN, INPUT);
    }

    void loop() {
      if (strip.isUpdating()) return;

      unsigned long now = millis();
      unsigned long elapsed = now - buttonPressedTime;

      if (buttonPressedTime == 0 && digitalRead(SUXSEM_AC_BTN_PIN) == SUXSEM_PRESSED_LEVEL) { //premo per la prima volta
        buttonPressedTime = now;
      } else if (buttonPressedTime != 0 && elapsed > SUXSEM_DEBOUNCE_THRESHOLD && !alreadyTurnedOn && bri == 0) { //effettivamente premuto
        //TURN ON
        jsonTransitionOnce = true;
        strip.setTransition(0);
        bri = 255;
        stateUpdated(CALL_MODE_BUTTON);
        alreadyTurnedOn = true;
      } else if (buttonPressedTime != 0 && digitalRead(SUXSEM_AC_BTN_PIN) != SUXSEM_PRESSED_LEVEL) { //rilascio
        if (!alreadyTurnedOn && elapsed <= SUXSEM_SHORT_MAX_MS && elapsed > SUXSEM_DEBOUNCE_THRESHOLD && bri != 0) {
          jsonTransitionOnce = true;
          strip.setTransition(3000);
          bri = 0;
          stateUpdated(CALL_MODE_BUTTON);
        }
        dimming = false;
        alreadyTurnedOn = false;
        buttonPressedTime = 0;
      } else if (buttonPressedTime != 0 && elapsed > SUXSEM_SHORT_MAX_MS && !dimming) { //continuo a premere
        //DIMMING
        dimming = true;
      }

      if (dimming && (now - lastDimmTime) > SUXSEM_DIMMING_TICK_MS) {
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
};