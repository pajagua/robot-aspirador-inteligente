// para control de motor de succion

#pragma once
#include <Arduino.h>
#include "config.h"

/*
    suction.h - Motor de succion con el MOSFET
    Control PWM: duty 0-100%
    Protegido con diodo flyback SS14
*/

#define PWM_CH_SUCCION 2
#define PWM_FREQ_HZ_SUCCION 20000 //25kHz, inaudible para humanos

void succion_init();
void succion_on(uint8_t porcentaje); // potencia 0-100%
void succion_on(); // potencia al maximo (100%)
void succion_off();
uint8_t succion_get(); // devuelve potencia actual (0-100%)

