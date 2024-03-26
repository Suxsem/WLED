#pragma once

#include "wled.h"

//This is an empty v2 usermod template. Please see the file usermod_v2_example.h in the EXAMPLE_v2 usermod folder for documentation on the functions you can use!

#define SUXSEM_AC_BTN_PIN             0
#define SUXSEM_SHORT_MAX_MS         500
#define SUXSEM_DEBOUNCE_THRESHOLD    50
#define SUXSEM_PRESSED_LEVEL        LOW

class UsermodSuxsem : public Usermod {
  private:
    bool enabled = false;
    unsigned long buttonPressedTime = 0;

  public:

    inline void enable(bool enable) { enabled = enable; }
    inline bool isEnabled() { return enabled; }

    void setup() {
      pinMode(SUXSEM_AC_BTN_PIN, INPUT);
    }

    void loop() {
      if (!enabled || strip.isUpdating()) return;

      unsigned long now = millis();

      if (buttonPressedTime == 0 && digitalRead(SUXSEM_AC_BTN_PIN) == SUXSEM_PRESSED_LEVEL) {
        buttonPressedTime = now;
      } else if (buttonLongPressed != 0 && digitalRead(SUXSEM_AC_BTN_PIN) != SUXSEM_PRESSED_LEVEL) {
        
        buttonPressedTime = 0;
      }

      if (now - buttonPressedTime > SUXSEM_DEBOUNCE_THRESHOLD) {

      }

      if ()

    }
};