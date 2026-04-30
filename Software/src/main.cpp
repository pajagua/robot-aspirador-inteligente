#include <Arduino.h>
#include "config.h"
#include "motors.h"
#include "ultrasonic.h"
#include "suction.h"

static bool ultrasonic_obstacle(const UltrasonicReading sensores[], int sensor, float umbral) {
    return sensores[sensor].valido && sensores[sensor].distancia_cm < umbral;
}

// =====================================================
//  main.cpp — Entry point del firmware Roomba
//  ESP32-S3-WROOM-1-N8R8
// =====================================================

// --- Estado de la maquina ---
typedef enum {
    STATE_INIT,
    STATE_IDLE,
    STATE_CLEANING,
    STATE_AVOID_LEFT,
    STATE_AVOID_RIGHT,
    STATE_BACK_UP,
} RoombaState;

static RoombaState estado = STATE_INIT;
static uint32_t tiempo_estado = 0;

#define OBSTACULO_CM        20.0f   // Distancia minima para considerar obstaculo
#define VEL_AVANCE          60      // Velocidad de avance normal
#define TIEMPO_RETROCESO    600     // ms de retroceso al detectar obstaculo
#define TIEMPO_GIRO         400     // ms de giro

// -----------------------------------------------------------

void setup() {
    Serial.begin(BAUD_RATE);
    Serial.println("=== Roomba ESP32-S3 ===");
    Serial.println("Inicializando...");

    // LED de estado
    pinMode(PIN_LED_VERDE, OUTPUT);
    digitalWrite(PIN_LED_VERDE, LOW);
    pinMode(PIN_LED_ROJO, OUTPUT);
    digitalWrite(PIN_LED_ROJO, LOW);

    // Inicializar modulos
    motors_init();
    ultrasonic_init();
    succion_init();

    // Parpadeo de confirmacion
    for (int i = 0; i < 3; i++) {
        digitalWrite(PIN_LED_VERDE, HIGH);
        digitalWrite(PIN_LED_ROJO, HIGH);
        delay(200);
        digitalWrite(PIN_LED_VERDE, LOW);
        digitalWrite(PIN_LED_ROJO, LOW);
        delay(200);
    }

    estado = STATE_IDLE;
    Serial.println("Listo. Enviando 's' por Serial para iniciar limpieza.");
}

// -----------------------------------------------------------

void loop() {
    // Comando por Serial para control basico
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 's':
                estado = STATE_CLEANING;
                motors_wake();
                succion_on();
                Serial.println("Limpieza iniciada");
                break;
            case 'x':
                estado = STATE_IDLE;
                motors_brake();
                motors_standby();
                succion_off();
                Serial.println("Detenido");
                break;
        }
    }

    // Leer sensores
    UltrasonicReading sensores[SENSOR_COUNT];
    ultrasonic_read_all(sensores);

    // Debug de distancias
    if (millis() % 500 < 50) {
        Serial.printf("IZQ: %.1f cm | CTR: %.1f cm | DER: %.1f cm\n",
            sensores[SENSOR_IZQ].distancia_cm,
            sensores[SENSOR_CENTRO].distancia_cm,
            sensores[SENSOR_DER].distancia_cm
        );
    }

    // Maquina de estados
    uint32_t ahora = millis();

    switch (estado) {

        case STATE_IDLE:
            motors_brake();
            succion_off();
            break;

        case STATE_CLEANING:
            // Avanzar mientras no haya obstaculos
            if (ultrasonic_obstacle(sensores, SENSOR_CENTRO, OBSTACULO_CM) ||
                ultrasonic_obstacle(sensores, SENSOR_IZQ, OBSTACULO_CM * 0.8f)) {
                // Obstaculo al frente/izquierda → retroceder y girar a la derecha
                estado = STATE_BACK_UP;
                tiempo_estado = ahora;
                motors_drive(-VEL_AVANCE, 0);
                Serial.println("Obstaculo detectado — retrocediendo");

            } else if (ultrasonic_obstacle(sensores, SENSOR_DER, OBSTACULO_CM * 0.8f)) {
                // Obstaculo a la derecha → girar izquierda
                estado = STATE_AVOID_LEFT;
                tiempo_estado = ahora;
                motors_drive(0, -40);

            } else {
                // Camino libre
                motors_drive(VEL_AVANCE, 0);
                digitalWrite(PIN_LED_ROJO, LOW);
            }
            break;

        case STATE_BACK_UP:
            if (ahora - tiempo_estado > TIEMPO_RETROCESO) {
                // Decidir giro segun sensor izquierdo
                if (sensores[SENSOR_IZQ].valido &&
                    sensores[SENSOR_IZQ].distancia_cm < OBSTACULO_CM) {
                    estado = STATE_AVOID_RIGHT;
                    motors_drive(0, 50);   // girar derecha
                } else {
                    estado = STATE_AVOID_LEFT;
                    motors_drive(0, -50);  // girar izquierda
                }
                tiempo_estado = ahora;
            }
            break;

        case STATE_AVOID_LEFT:
            if (ahora - tiempo_estado > TIEMPO_GIRO) {
                estado = STATE_CLEANING;
            }
            break;

        case STATE_AVOID_RIGHT:
            if (ahora - tiempo_estado > TIEMPO_GIRO) {
                estado = STATE_CLEANING;
            }
            break;

        default:
            estado = STATE_IDLE;
            break;
    }

    delay(20);   // Loop a 50Hz
}