// para el driver de motores TB6612FNG

#pragma once
#include <Arduino.h>
#include "config.h"
/*
    motors.h - Control TB6612FNG 
    Motor A = Motor Izquierdo (J3)
    Motor B = Motor Derecho   (J4)
*/

//Canales LEDC (PWM por hardware del ESP32)
#define PWM_CH_MOTOR_A 0
#define PWM_CH_MOTOR_B 1
#define PWM_FREQ_HZ    20000    // 20kHz - inaudible para el humano
#define PWM_RESOLUTION 8        // 8 bits: duty cycle 0-255

void motors_init();
void motors_standby();
void motors_wake();

// Control individual
void motor_izq(int8_t velocidad); // -100 a 100
void motor_der(int8_t velocidad); // -100 a 100


// Control combinado (diferencial)
void motors_drive(int8_t lineal, int8_t angular); // lineal: adelante/atras, angular: giro

void motors_stop();
void motors_brake();