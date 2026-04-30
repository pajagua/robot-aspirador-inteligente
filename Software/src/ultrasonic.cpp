#include "ultrasonic.h"

// Mapeo de pines por sesor
static const uint8_t _trigger_pins[SENSOR_COUNT] = {
    PIN_TRIGGER_IZQ,
    PIN_TRIGGER_CENTRO,
    PIN_TRIGGER_DER
};

static const uint8_t _echo_pins[SENSOR_COUNT] = {
    PIN_ECHO_IZQ,
    PIN_ECHO_CENTRO,
    PIN_ECHO_DER
};

void ultrasonic_init(){
    for (int i = 0; i < SENSOR_COUNT; i++){
        pinMode(_trigger_pins[i], OUTPUT);
        pinMode(_echo_pins[i], INPUT);
        digitalWrite(_trigger_pins[i], LOW); // Asegurar trigger en LOW
    }
    // Esperar estabilizacion del sensor
    delay(50);
}

UltrasonicReading ultrasonic_read(SensorID sensor){
    UltrasonicReading result = { -1.0f, false};

    uint8_t trig = _trigger_pins[sensor];
    uint8_t echo = _echo_pins[sensor];

    // Pulso de trigger: 10us HIGH
    digitalWrite(trig, LOW);
    delayMicroseconds(2);   // asegurar que empieza en LOW
    digitalWrite(trig, HIGH);
    delayMicroseconds(ULTRASONIC_TRIGGER_US);   // pulso de 10us
    digitalWrite(trig, LOW);

    // Medir duracion del puslo de echo
    long duracion = pulseIn(echo, HIGH, ULTRASONIC_TIMEOUT_US);
    
    if (duracion == 0){
        // Timeout: objeto demasiado lejos o sin respuesta
        return result; 
    }

    // Conversion: velocidad del sonido ~343 m/s = 0.0343 cm/us
    // Distancia = (duracion / 2) * 0.0343 (ida y vuelta)
    result.distancia_cm = (duracion * 0.01715f);
    result.valido = (result.distancia_cm < 400.0f); // HC-SR04 max ~400cm;
    return result;
}

void ultrasonic_read_all(UltrasonicReading lecturas[SENSOR_COUNT]){
    for (int i = 0; i < SENSOR_COUNT; i++){
        lecturas[i] = ultrasonic_read((SensorID)i);
        delay(10); // Pequeña pausa entre lecturas para evitar interferencias
    }
}

bool ultrasonic_obstacle(SensorID sensor, float umbral_cm){
    UltrasonicReading r = ultrasonic_read(sensor);
    if(!r.valido) return false; // No se detecto nada, no es un obstaculo
    return r.distancia_cm < umbral_cm;
}
