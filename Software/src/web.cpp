#include "web.h"
#include "config.h"
#include "navigation.h"

#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

static AsyncWebServer _server(80);
static AsyncWebSocket _ws("/ws");

// Los eventos del WebSocket corren en la tarea async_tcp: los comandos se
// pasan al loop() por una cola para no tocar el hardware desde dos tareas.
static QueueHandle_t _cola = nullptr;

static void _encolar(TipoComando tipo, int16_t a = 0, int16_t b = 0) {
    Comando c = { tipo, a, b };
    if (xQueueSend(_cola, &c, 0) != pdTRUE && tipo == CMD_STOP) {
        // Cola llena: la parada de emergencia nunca se descarta
        xQueueReset(_cola);
        xQueueSend(_cola, &c, 0);
    }
}

static void _procesar_mensaje(AsyncWebSocketClient *client, const uint8_t *data, size_t len) {
    JsonDocument doc;
    if (deserializeJson(doc, data, len)) return;

    const char *cmd = doc["cmd"];
    if (!cmd) return;

    if (strcmp(cmd, "DRIVE") == 0) {
        _encolar(CMD_DRIVE, doc["lin"] | 0, doc["ang"] | 0);
    } else if (strcmp(cmd, "SUCCION") == 0) {
        _encolar(CMD_SUCCION, doc["val"] | 0);
    } else if (strcmp(cmd, "CEPILLO") == 0) {
        _encolar(CMD_CEPILLO, doc["val"] | 0);
    } else if (strcmp(cmd, "MODO") == 0) {
        ModoRobot m;
        if (nav_modo_desde_texto(doc["val"], &m)) _encolar(CMD_MODO, m);
    } else if (strcmp(cmd, "CONFIG") == 0) {
        _encolar(CMD_CONFIG, doc["umbral"] | -1, doc["vel"] | -1);
    } else if (strcmp(cmd, "STOP") == 0 || strcmp(cmd, "EMERGENCY_STOP") == 0) {
        _encolar(CMD_STOP);
    } else if (strcmp(cmd, "PING") == 0) {
        char buf[40];
        snprintf(buf, sizeof(buf), "{\"pong\":%.0f}", doc["t"] | 0.0);
        client->text(buf);
    }
}

static void _on_ws_event(AsyncWebSocket *server, AsyncWebSocketClient *client,
                         AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("[WS] Cliente #%u conectado (%s)\n", client->id(),
                          client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("[WS] Cliente #%u desconectado\n", client->id());
            // Detener el movimiento manual por seguridad
            _encolar(CMD_DRIVE, 0, 0);
            break;
        case WS_EVT_DATA: {
            AwsFrameInfo *info = (AwsFrameInfo *)arg;
            // Solo mensajes de texto completos en un unico frame
            if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                _procesar_mensaje(client, data, len);
            }
            break;
        }
        default:
            break;
    }
}

void web_init() {
    _cola = xQueueCreate(16, sizeof(Comando));

    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS, WIFI_AP_CANAL);
    WiFi.setSleep(false);   // menor latencia en el control
    Serial.printf("[WiFi] AP \"%s\" activo — http://%s\n", WIFI_AP_SSID,
                  WiFi.softAPIP().toString().c_str());

    if (!LittleFS.begin(true)) {
        Serial.println("[FS] Error montando LittleFS (subir con 'Upload Filesystem Image')");
    }

    _ws.onEvent(_on_ws_event);
    _server.addHandler(&_ws);
    _server.serveStatic("/", LittleFS, "/")
           .setDefaultFile("index.html")
           .setCacheControl("max-age=600");
    _server.onNotFound([](AsyncWebServerRequest *req) {
        req->send(404, "text/plain", "No encontrado");
    });
    _server.begin();
}

bool web_recibir(Comando *cmd) {
    return _cola && xQueueReceive(_cola, cmd, 0) == pdTRUE;
}

void web_enviar_telemetria(const char *json, size_t len) {
    if (_ws.count() > 0 && _ws.availableForWriteAll()) {
        _ws.textAll(json, len);
    }
}

void web_mantenimiento() {
    _ws.cleanupClients();
}

uint8_t web_clientes() {
    return _ws.count();
}
