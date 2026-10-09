/**
 * @file Prueba.ino
 * @brief Blink básico para ESP32 - LED en GPIO2
 * 
 * Parpadeo simple del LED built-in conectado al GPIO2.
 * Plataforma: ESP32 (cualquier variante con LED en GPIO2)
 */

// ============================================================
// Definición de pines y constantes
// ============================================================

/** @brief Pin del LED built-in del ESP32 */
#define LED_PIN 2

/** @brief Tiempo de encendido/apagado del LED en milisegundos */
#define BLINK_DELAY_MS 1000

// ============================================================
// Setup - Se ejecuta una sola vez al iniciar
// ============================================================
void setup() {
    // Inicializar comunicación serial para debug
    Serial.begin(115200);

    // Configurar GPIO2 como salida digital
    pinMode(LED_PIN, OUTPUT);

    // Estado inicial: LED apagado
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 Blink - GPIO2 inicializado");
}

// ============================================================
// Loop - Se ejecuta indefinidamente
// ============================================================
void loop() {
    // Encender LED (nivel HIGH)
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(BLINK_DELAY_MS);

    // Apagar LED (nivel LOW)
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(BLINK_DELAY_MS);
}
