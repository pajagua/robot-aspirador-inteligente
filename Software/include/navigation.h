#pragma once
#include <Arduino.h>
#include "config.h"

/*
    navigation.h - Maquina de estados del robot
    Modos: MANUAL (joystick), AUTO (rebote), PAREDES (seguir pared derecha),
           ESPIRAL (espiral creciente, luego pasa a AUTO)
*/

typedef enum {
    MODO_MANUAL = 0,
    MODO_AUTO,
    MODO_PAREDES,
    MODO_ESPIRAL,
    MODO_COUNT
} ModoRobot;

void nav_init();
void nav_update();                       // llamar en cada loop

void nav_set_modo(ModoRobot modo);
ModoRobot nav_get_modo();
const char *nav_modo_nombre(ModoRobot modo);
bool nav_modo_desde_texto(const char *txt, ModoRobot *out);

void nav_drive_manual(int lineal, int angular);   // solo tiene efecto en MANUAL
void nav_parada_emergencia();

void nav_set_umbral(uint8_t cm);
void nav_set_velocidad(uint8_t vel);
uint8_t nav_get_umbral();
uint8_t nav_get_velocidad();

const char *nav_estado_texto();          // estado actual para telemetria
