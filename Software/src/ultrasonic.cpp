#include "ultrasonic.h"

// Mapeo de pines por sensor
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

static UltrasonicReading _lecturas[SENSOR_COUNT];

// Estado de la medicion en curso (escrito por la ISR)
static volatile uint32_t _t_subida = 0;
static volatile uint32_t _t_bajada = 0;
static volatile int8_t   _sensor_activo = -1;

static uint8_t  _siguiente = 0;
static uint32_t _t_disparo_us = 0;
static uint32_t _t_ultimo_ms = 0;

static void IRAM_ATTR _isr_echo(void *arg) {
    int8_t id = (int8_t)(intptr_t)arg;
    if (id != _sensor_activo) return;
    if (digitalRead(_echo_pins[id])) {
        _t_subida = micros();
    } else if (_t_subida) {
        _t_bajada = micros();
    }
}

void ultrasonic_init() {
    for (int i = 0; i < SENSOR_COUNT; i++) {
        pinMode(_trigger_pins[i], OUTPUT);
        digitalWrite(_trigger_pins[i], LOW);
        pinMode(_echo_pins[i], INPUT);
        attachInterruptArg(digitalPinToInterrupt(_echo_pins[i]), _isr_echo,
                           (void *)(intptr_t)i, CHANGE);
        _lecturas[i] = { -1.0f, false };
    }
}

static void _cerrar_medicion() {
    int8_t id = _sensor_activo;
    UltrasonicReading r = { -1.0f, false };

    if (_t_bajada) {
        // Velocidad del sonido ~343 m/s → distancia = duracion/2 * 0.0343 cm/us
        float cm = (_t_bajada - _t_subida) * 0.01715f;
        if (cm > 1.0f && cm < ULTRASONIC_MAX_CM) {
            r = { cm, true };
        }
    }
    _lecturas[id] = r;
    _sensor_activo = -1;
}

void ultrasonic_update() {
    // 1. Cerrar la medicion en curso si llego el eco o vencio el timeout
    if (_sensor_activo >= 0) {
        bool timeout = (micros() - _t_disparo_us) > ULTRASONIC_TIMEOUT_US;
        if (!_t_bajada && !timeout) return;
        _cerrar_medicion();
    }

    // 2. Disparar el siguiente sensor (respetando la pausa anti-eco cruzado)
    uint32_t ahora = millis();
    if (ahora - _t_ultimo_ms < ULTRASONIC_PERIODO_MS) return;
    _t_ultimo_ms = ahora;

    uint8_t trig = _trigger_pins[_siguiente];
    _t_subida = 0;
    _t_bajada = 0;
    _sensor_activo = _siguiente;

    digitalWrite(trig, HIGH);
    delayMicroseconds(ULTRASONIC_TRIGGER_US);
    digitalWrite(trig, LOW);
    _t_disparo_us = micros();

    _siguiente = (_siguiente + 1) % SENSOR_COUNT;
}

UltrasonicReading ultrasonic_get(SensorID sensor) {
    return _lecturas[sensor];
}

bool ultrasonic_obstacle(SensorID sensor, float umbral_cm) {
    const UltrasonicReading &r = _lecturas[sensor];
    return r.valido && r.distancia_cm < umbral_cm;
}

float ultrasonic_libre_cm(SensorID sensor) {
    const UltrasonicReading &r = _lecturas[sensor];
    return r.valido ? r.distancia_cm : ULTRASONIC_MAX_CM;
}
