// para sensor HC-SR04

#pragma once
#include <Arduino.h>
#include "config.h"

/*
    ultrasonic.h - Sensores HC-SR04 x3 (lectura NO bloqueante)
    Echo protegido: divisor R 1K / 2K (5V -> 3.3V)

    ultrasonic_update() se llama en cada loop: dispara un sensor por turno
    (round-robin cada ULTRASONIC_PERIODO_MS) y mide el eco por interrupcion,
    asi el loop nunca queda bloqueado esperando el eco.
*/

typedef enum {
    SENSOR_IZQ = 0,
    SENSOR_CENTRO = 1,
    SENSOR_DER = 2,
    SENSOR_COUNT = 3
} SensorID;

typedef struct {
    float distancia_cm; // Distancia medida en cm, -1 si fuera de rango o timeout
    bool valido;        // Lectura valida (no timeout)
} UltrasonicReading;

void ultrasonic_init();
void ultrasonic_update();

// Ultima lectura de un sensor
UltrasonicReading ultrasonic_get(SensorID sensor);

// true si el sensor ve un objeto a menos de umbral_cm
bool ultrasonic_obstacle(SensorID sensor, float umbral_cm);

// Distancia para decidir giros: si no hay eco se asume espacio libre
float ultrasonic_libre_cm(SensorID sensor);
