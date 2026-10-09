// para el driver de motores TB6612FNG

#pragma once
#include <Arduino.h>
#include "config.h"
/*
    motors.h - Control TB6612FNG
    Motor A = Motor Izquierdo (J3)
    Motor B = Motor Derecho   (J4)
*/

void motors_init();
void motors_standby();
void motors_wake();

// Control individual: -100 (atras) a 100 (adelante)
void motor_izq(int velocidad);
void motor_der(int velocidad);

// Control combinado (diferencial)
// lineal:  -100 = atras,          100 = adelante
// angular: -100 = giro izquierda, 100 = giro derecha
void motors_drive(int lineal, int angular);

void motors_stop();    // rueda libre
void motors_brake();   // freno activo
