// para control del motor de succion

#pragma once
#include <Arduino.h>
#include "config.h"

/*
    suction.h - Motor de succion BLDC de 4 pines (V+, GND, PWM, FG)
    El driver esta integrado en la placa del motor: el ESP32 solo genera
    la señal PWM de velocidad (y opcionalmente lee el tacometro FG).
*/

void succion_init();
void succion_set(uint8_t porcentaje); // potencia 0-100%
void succion_on();                    // potencia al maximo (100%)
void succion_off();
uint8_t succion_get();                // potencia actual (0-100%)
uint16_t succion_rpm();               // RPM medidas por FG (0 si no hay FG)
