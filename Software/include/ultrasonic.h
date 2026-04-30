// para sensor HC-SR04

#pragma once
#include <Arduino.h>
#include "config.h"

/*
    ultrasonic.h - Sensores HC-SR04 x3
    Echo protegifo: dividor R 1K / 2K (5V -> 3.3V)
*/

typedef enum {
    SENSOR_IZQ = 0,
    SENSOR_CENTRO = 1,
    SENSOR_DER = 2,
    SENSOR_COUNT = 3
} SensorID;

typedef struct {
    float distancia_cm; // Distancia medida en cm, -1 si fuera de rango o timeout
    bool valido;        // Indica si la lectura es válida (no timeout)
} UltrasonicReading;

void ultrasonic_init();

// Lectura individual (bloqueante, ~30ms max por sensor)
UltrasonicReading ultrasonic_read(SensorID sensor);

// Lectura de los 3 sensores en secuencia (bloqueante, ~90ms max)
void ultrasonic_read_all(UltrasonicReading lecturas[SENSOR_COUNT]);

// Umbral de deteccion
bool ultrsonic_obstacle(SensorID sensor, float umbral_cm);