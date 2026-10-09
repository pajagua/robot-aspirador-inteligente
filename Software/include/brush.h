// para control del motor del cepillo

#pragma once
#include <Arduino.h>
#include "config.h"

/*
    brush.h - Motor del cepillo con MOSFET NMOS (Q1)
    Control PWM: duty 0-100%
    Protegido con diodo flyback SS14
*/

void cepillo_init();
void cepillo_set(uint8_t porcentaje); // potencia 0-100%
void cepillo_on();                    // potencia al maximo (100%)
void cepillo_off();
uint8_t cepillo_get();                // potencia actual (0-100%)
