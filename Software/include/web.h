#pragma once
#include <Arduino.h>

/*
    web.h - Punto de acceso WiFi + servidor web (LittleFS) + WebSocket /ws

    Protocolo cliente → robot (JSON):
      {"cmd":"DRIVE","lin":-100..100,"ang":-100..100}
      {"cmd":"SUCCION","val":0..100}
      {"cmd":"CEPILLO","val":0..100}
      {"cmd":"MODO","val":"MANUAL"|"AUTO"|"PAREDES"|"ESPIRAL"}
      {"cmd":"CONFIG","umbral":5..100,"vel":20..100}
      {"cmd":"STOP"}
      {"cmd":"PING","t":<numero>}            → responde {"pong":<numero>}

    Robot → cliente (telemetria periodica):
      {"radar":[izq,ctr,der],"estado":"...","modo":"...","suc":0..100,
       "cep":0..100,"rpm":0,"mis":<seg>,"umbral":20,"vel":60}
      Distancia -1 = sin eco (fuera de rango)
*/

typedef enum {
    CMD_DRIVE,
    CMD_SUCCION,
    CMD_CEPILLO,
    CMD_MODO,
    CMD_CONFIG,
    CMD_STOP,
} TipoComando;

typedef struct {
    TipoComando tipo;
    int16_t a;   // DRIVE: lin | SUCCION/CEPILLO: val | MODO: ModoRobot | CONFIG: umbral (-1 = sin cambio)
    int16_t b;   // DRIVE: ang | CONFIG: vel (-1 = sin cambio)
} Comando;

void web_init();
bool web_recibir(Comando *cmd);          // saca un comando de la cola (no bloqueante)
void web_enviar_telemetria(const char *json, size_t len);
void web_mantenimiento();                // limpiar clientes desconectados
uint8_t web_clientes();
