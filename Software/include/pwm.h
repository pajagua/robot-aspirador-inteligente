#pragma once
#include <Arduino.h>

/*
    pwm.h - Envoltura LEDC compatible con Arduino-ESP32 2.x y 3.x
    2.x: ledcSetup + ledcAttachPin, ledcWrite(canal)
    3.x: ledcAttachChannel, ledcWrite(pin)
*/

inline void pwm_attach(uint8_t pin, uint8_t canal, uint32_t freq, uint8_t bits) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttachChannel(pin, freq, bits, canal);
#else
    ledcSetup(canal, freq, bits);
    ledcAttachPin(pin, canal);
#endif
}

inline void pwm_write(uint8_t pin, uint8_t canal, uint32_t duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    (void)canal;
    ledcWrite(pin, duty);
#else
    (void)pin;
    ledcWrite(canal, duty);
#endif
}
