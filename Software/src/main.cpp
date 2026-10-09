// =====================================================
//  main.cpp — Entry point del firmware Roomba
//  ESP32-S3-WROOM-1-N8R8
// =====================================================
#include <Arduino.h>
#include "config.h"
#include "motors.h"
#include "ultrasonic.h"
#include "suction.h"
#include "brush.h"
#include "navigation.h"
#include "web.h"

#define LOOP_PERIODO_MS     10      // control a 100 Hz
#define DEBUG_PERIODO_MS    500

static uint32_t _t_mision = 0;      // inicio de la mision (0 = sin mision)

// -----------------------------------------------------------

static void actualizar_mision() {
    // La mision corre mientras algun actuador de limpieza este encendido
    bool limpiando = succion_get() > 0 || cepillo_get() > 0;
    if (limpiando && !_t_mision) _t_mision = millis();
    if (!limpiando) _t_mision = 0;
}

static void procesar_comando(const Comando &c) {
    switch (c.tipo) {
        case CMD_DRIVE:
            nav_drive_manual(c.a, c.b);
            break;
        case CMD_SUCCION:
            succion_set(constrain(c.a, 0, 100));
            break;
        case CMD_CEPILLO:
            cepillo_set(constrain(c.a, 0, 100));
            break;
        case CMD_MODO:
            nav_set_modo((ModoRobot)c.a);
            Serial.printf("Modo: %s\n", nav_modo_nombre((ModoRobot)c.a));
            break;
        case CMD_CONFIG:
            if (c.a >= 0) nav_set_umbral(c.a);
            if (c.b >= 0) nav_set_velocidad(c.b);
            break;
        case CMD_STOP:
            nav_parada_emergencia();
            Serial.println("PARADA DE EMERGENCIA");
            break;
    }
    actualizar_mision();
}

static void procesar_serial() {
    // Comandos de depuracion por Serial
    if (!Serial.available()) return;
    switch (Serial.read()) {
        case 's': procesar_comando({ CMD_MODO, MODO_AUTO, 0 });    break;
        case 'p': procesar_comando({ CMD_MODO, MODO_PAREDES, 0 }); break;
        case 'e': procesar_comando({ CMD_MODO, MODO_ESPIRAL, 0 }); break;
        case 'x': procesar_comando({ CMD_STOP, 0, 0 });            break;
    }
}

static void enviar_telemetria() {
    UltrasonicReading izq = ultrasonic_get(SENSOR_IZQ);
    UltrasonicReading ctr = ultrasonic_get(SENSOR_CENTRO);
    UltrasonicReading der = ultrasonic_get(SENSOR_DER);
    uint32_t mision_s = _t_mision ? (millis() - _t_mision) / 1000 : 0;

    char buf[256];
    int n = snprintf(buf, sizeof(buf),
        "{\"radar\":[%.1f,%.1f,%.1f],\"estado\":\"%s\",\"modo\":\"%s\","
        "\"suc\":%u,\"cep\":%u,\"rpm\":%u,\"mis\":%lu,\"umbral\":%u,\"vel\":%u}",
        izq.valido ? izq.distancia_cm : -1.0f,
        ctr.valido ? ctr.distancia_cm : -1.0f,
        der.valido ? der.distancia_cm : -1.0f,
        nav_estado_texto(), nav_modo_nombre(nav_get_modo()),
        succion_get(), cepillo_get(), succion_rpm(), (unsigned long)mision_s,
        nav_get_umbral(), nav_get_velocidad());
    if (n > 0 && n < (int)sizeof(buf)) web_enviar_telemetria(buf, n);
}

static void actualizar_led(uint32_t ahora) {
    // Fijo = cliente conectado | parpadeo lento = esperando conexion
    bool on = web_clientes() > 0 || (ahora / 500) % 2;
    digitalWrite(PIN_LED_VERDE, on);
}

// -----------------------------------------------------------

void setup() {
    // Actuadores primero: dejar los PWM en 0 lo antes posible tras el reset
    succion_init();
    cepillo_init();
    motors_init();

    Serial.begin(BAUD_RATE);
    Serial.println("Inicializando...");

    pinMode(PIN_LED_VERDE, OUTPUT);
    digitalWrite(PIN_LED_VERDE, LOW);

    ultrasonic_init();
    nav_init();
    web_init();
    randomSeed(esp_random());

    Serial.println("Listo. Serial: 's' auto | 'p' paredes | 'e' espiral | 'x' stop");
}

void loop() {
    static uint32_t t_loop = 0, t_tele = 0, t_debug = 0, t_mant = 0;
    uint32_t ahora = millis();

    // Sensores: no bloqueante, se llama lo mas seguido posible
    ultrasonic_update();

    if (ahora - t_loop < LOOP_PERIODO_MS) return;
    t_loop = ahora;

    Comando cmd;
    while (web_recibir(&cmd)) procesar_comando(cmd);
    procesar_serial();

    nav_update();
    actualizar_mision();   // un modo autonomo puede encender los actuadores
    actualizar_led(ahora);

    if (ahora - t_tele >= TELEMETRIA_PERIODO_MS) {
        t_tele = ahora;
        enviar_telemetria();
    }

    if (ahora - t_mant >= 1000) {
        t_mant = ahora;
        web_mantenimiento();
    }

    if (ahora - t_debug >= DEBUG_PERIODO_MS) {
        t_debug = ahora;
        Serial.printf("IZQ: %.1f | CTR: %.1f | DER: %.1f cm | %s | suc %u%% cep %u%%\n",
            ultrasonic_get(SENSOR_IZQ).distancia_cm,
            ultrasonic_get(SENSOR_CENTRO).distancia_cm,
            ultrasonic_get(SENSOR_DER).distancia_cm,
            nav_estado_texto(), succion_get(), cepillo_get());
    }
}
