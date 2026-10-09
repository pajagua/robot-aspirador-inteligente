#include "navigation.h"
#include "motors.h"
#include "ultrasonic.h"
#include "suction.h"
#include "brush.h"

// --- Estados internos ---
typedef enum {
    NAV_IDLE,
    NAV_MANUAL,
    NAV_AVANZAR,
    NAV_RETROCEDER,
    NAV_GIRAR,
    NAV_ESPIRAL,
    NAV_PARED,
} NavEstado;

static const char *const _modo_nombres[MODO_COUNT] = { "MANUAL", "AUTO", "PAREDES", "ESPIRAL" };

static ModoRobot _modo = MODO_MANUAL;
static NavEstado _estado = NAV_IDLE;
static uint32_t  _t_estado = 0;      // inicio del estado actual (ms)
static uint32_t  _dur_giro = 0;      // duracion del giro en curso (ms)
static int8_t    _dir_giro = 1;      // +1 derecha, -1 izquierda

static uint8_t _umbral_cm = NAV_UMBRAL_CM_DEFAULT;
static uint8_t _vel = NAV_VEL_DEFAULT;

// Comando manual (escrito desde el procesador de comandos del loop)
static int      _man_lin = 0;
static int      _man_ang = 0;
static uint32_t _t_man = 0;

// -----------------------------------------------------------

static void _cambiar_estado(NavEstado e) {
    _estado = e;
    _t_estado = millis();
}

static NavEstado _estado_base() {
    // Estado al que vuelve cada modo despues de esquivar
    return _modo == MODO_PAREDES ? NAV_PARED : NAV_AVANZAR;
}

static void _iniciar_esquive() {
    // Girar hacia el lado con mas espacio libre
    _dir_giro = ultrasonic_libre_cm(SENSOR_DER) > ultrasonic_libre_cm(SENSOR_IZQ) ? 1 : -1;
    // En modo paredes siempre se gira a la izquierda (pared a la derecha)
    if (_modo == MODO_PAREDES) _dir_giro = -1;
    _dur_giro = NAV_TIEMPO_GIRO_MS + random(0, 400);   // aleatorio: evita ciclos
    motors_drive(-_vel, 0);
    _cambiar_estado(NAV_RETROCEDER);
}

static bool _obstaculo_frontal() {
    return ultrasonic_obstacle(SENSOR_CENTRO, _umbral_cm) ||
           ultrasonic_obstacle(SENSOR_IZQ, _umbral_cm * 0.8f) ||
           ultrasonic_obstacle(SENSOR_DER, _umbral_cm * 0.8f);
}

// -----------------------------------------------------------

static void _update_manual(uint32_t ahora) {
    int lin = _man_lin, ang = _man_ang;

    // Sin comandos recientes (WiFi caido / app cerrada) → frenar
    if (ahora - _t_man > MANUAL_TIMEOUT_MS) lin = ang = 0;

    // Proteccion anti-choque: no avanzar con un obstaculo muy cerca
    if (lin > 0 && ultrasonic_obstacle(SENSOR_CENTRO, _umbral_cm * 0.5f)) lin = 0;

    if (lin == 0 && ang == 0) {
        if (_estado != NAV_IDLE) {
            motors_brake();
            _cambiar_estado(NAV_IDLE);
        }
        return;
    }
    if (_estado != NAV_MANUAL) {
        motors_wake();
        _cambiar_estado(NAV_MANUAL);
    }
    motors_drive(lin, ang);
}

static void _update_pared() {
    if (ultrasonic_obstacle(SENSOR_CENTRO, _umbral_cm)) {
        _iniciar_esquive();
        return;
    }
    int lin = _vel * 8 / 10;
    int ang;
    UltrasonicReading der = ultrasonic_get(SENSOR_DER);
    if (!der.valido || der.distancia_cm > NAV_PARED_OBJETIVO_CM * 3) {
        ang = 25;   // pared perdida: curvar a la derecha para buscarla
    } else {
        // Control proporcional: lejos → derecha, cerca → izquierda
        ang = constrain((int)((der.distancia_cm - NAV_PARED_OBJETIVO_CM) * 2.0f), -40, 40);
    }
    motors_drive(lin, ang);
}

static void _update_espiral(uint32_t ahora) {
    uint32_t t = ahora - _t_estado;
    if (_obstaculo_frontal() || t > NAV_ESPIRAL_MAX_MS) {
        // Fin de la espiral: continuar en modo AUTO
        _modo = MODO_AUTO;
        _iniciar_esquive();
        return;
    }
    // El giro disminuye con el tiempo → radio creciente
    int ang = map(t, 0, NAV_ESPIRAL_MAX_MS, _vel, _vel / 8);
    motors_drive(_vel, ang);
}

void nav_update() {
    uint32_t ahora = millis();

    switch (_estado) {
        case NAV_IDLE:
        case NAV_MANUAL:
            if (_modo == MODO_MANUAL) _update_manual(ahora);
            break;

        case NAV_AVANZAR:
            if (_obstaculo_frontal()) _iniciar_esquive();
            else motors_drive(_vel, 0);
            break;

        case NAV_RETROCEDER:
            if (ahora - _t_estado > NAV_TIEMPO_RETROCESO_MS) {
                motors_drive(0, _dir_giro * 50);
                _cambiar_estado(NAV_GIRAR);
            }
            break;

        case NAV_GIRAR:
            if (ahora - _t_estado > _dur_giro) _cambiar_estado(_estado_base());
            break;

        case NAV_ESPIRAL:
            _update_espiral(ahora);
            break;

        case NAV_PARED:
            _update_pared();
            break;
    }
}

// -----------------------------------------------------------

void nav_init() {
    _modo = MODO_MANUAL;
    _cambiar_estado(NAV_IDLE);
}

void nav_set_modo(ModoRobot modo) {
    if (modo >= MODO_COUNT) return;
    _modo = modo;

    if (modo == MODO_MANUAL) {
        _man_lin = _man_ang = 0;
        motors_brake();
        _cambiar_estado(NAV_IDLE);
        return;
    }

    // Modos autonomos: encender actuadores si estaban apagados
    if (succion_get() == 0) succion_set(SUCCION_POT_DEFAULT);
    if (cepillo_get() == 0) cepillo_set(CEPILLO_POT_DEFAULT);
    motors_wake();

    switch (modo) {
        case MODO_ESPIRAL: _cambiar_estado(NAV_ESPIRAL); break;
        case MODO_PAREDES: _cambiar_estado(NAV_PARED);   break;
        default:           _cambiar_estado(NAV_AVANZAR); break;
    }
}

ModoRobot nav_get_modo() { return _modo; }

const char *nav_modo_nombre(ModoRobot modo) {
    return modo < MODO_COUNT ? _modo_nombres[modo] : "?";
}

bool nav_modo_desde_texto(const char *txt, ModoRobot *out) {
    if (!txt) return false;
    for (int i = 0; i < MODO_COUNT; i++) {
        if (strcmp(txt, _modo_nombres[i]) == 0) {
            *out = (ModoRobot)i;
            return true;
        }
    }
    return false;
}

void nav_drive_manual(int lineal, int angular) {
    _man_lin = constrain(lineal, -100, 100);
    _man_ang = constrain(angular, -100, 100);
    _t_man = millis();
}

void nav_parada_emergencia() {
    nav_set_modo(MODO_MANUAL);
    succion_off();
    cepillo_off();
    motors_standby();
}

void nav_set_umbral(uint8_t cm)     { _umbral_cm = constrain(cm, 5, 100); }
void nav_set_velocidad(uint8_t vel) { _vel = constrain(vel, 20, 100); }
uint8_t nav_get_umbral()            { return _umbral_cm; }
uint8_t nav_get_velocidad()         { return _vel; }

const char *nav_estado_texto() {
    switch (_estado) {
        case NAV_IDLE:       return "IDLE";
        case NAV_MANUAL:     return "MANUAL";
        case NAV_AVANZAR:    return "AVANZANDO";
        case NAV_RETROCEDER: return "RETROCEDIENDO";
        case NAV_GIRAR:      return "GIRANDO";
        case NAV_ESPIRAL:    return "ESPIRAL";
        case NAV_PARED:      return "SIGUIENDO PARED";
    }
    return "?";
}
