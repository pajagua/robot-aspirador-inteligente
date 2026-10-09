#pragma once
// =====================================================
//  config.h — Pines y parametros del robot aspirador
//  ESP32-S3-WROOM-1-N8R8
// =====================================================

// =====================================================
//  DRIVER DE MOTORES DE TRACCION — TB6612FNG
// =====================================================
#define PIN_STBY        4   // io4_espi  → STBY: HIGH = activo | LOW = standby

#define PIN_PWMA        7   // io7_espi  → PWMA: Velocidad Motor A (Izquierdo)
#define PIN_AIN1        5   // io05_espi → AIN1: Direccion Motor A (1)
#define PIN_AIN2        6   // io06_espi → AIN2: Direccion Motor A (2)

#define PIN_PWMB        8   // io08_espi → PWMB: Velocidad Motor B (Derecho)
#define PIN_BIN1        39  // io39_espi → BIN1: Direccion Motor B (1)  (io17 en esquema)
#define PIN_BIN2        40  // io40_espi → BIN2: Direccion Motor B (2)

// Combinaciones xIN1/xIN2
//  Adelante:   IN1=HIGH, IN2=LOW
//  Atras:      IN1=LOW,  IN2=HIGH
//  Freno:      IN1=HIGH, IN2=HIGH
//  Libre:      IN1=LOW,  IN2=LOW

// =====================================================
//  MOTOR DE SUCCION — BLDC de 4 pines (driver integrado)
// =====================================================
// Pines del motor: V+ | GND | PWM | FG (tacometro)
// El driver va en la placa del motor: el PWM sale directo del ESP32.
// IMPORTANTE: medir el pin PWM del motor desconectado. Si tiene pull-up
// a 5V (>3.6V), poner un transistor/level-shifter: el ESP32 no es 5V tolerante.

#define PIN_SUCCION_PWM     48   // io48 (J10 expansion) → entrada PWM del motor
#define PIN_SUCCION_FG      -1   // Salida FG/tacometro del motor (-1 = no conectado)

#define SUCCION_PWM_FREQ_HZ     25000   // 25 kHz: estandar para drivers BLDC tipo ventilador
#define SUCCION_PWM_INVERTIDO   0       // 1 si el driver interpreta duty 0% como maxima velocidad
#define SUCCION_DUTY_MIN_PCT    15      // Duty minimo con el que el motor arranca (1% pedido → este valor)
#define SUCCION_FG_PULSOS_VUELTA 6      // Pulsos FG por revolucion (ver hoja de datos del motor)

// =====================================================
//  MOTOR DEL CEPILLO — MOSFET NMOS Q1 (PWM)
// =====================================================
// Motor DC con escobillas. Gate del NMOS controlado por PWM.
// Protegido con diodo flyback SS14 (DiodoProteccion1).

#define PIN_MOTOR_CEPILLO   47   // io47_espi → Gate del NMOS Q1

#define CEPILLO_PWM_FREQ_HZ 20000   // 20 kHz: inaudible

// =====================================================
//  SENSORES ULTRASONICOS — HC-SR04 (x3)
// =====================================================
// Echo protegido con divisor resistivo 1K / 2K (5V → 3.3V)

#define PIN_TRIGGER_IZQ     10   // io10_espi → trigger1 (J5 Left Ultrasonic)
#define PIN_ECHO_IZQ        11   // io11_espi → echo1

#define PIN_TRIGGER_CENTRO  12   // io12_espi → trigger2 (J6 Central Ultrasonic)
#define PIN_ECHO_CENTRO     13   // io13_espi → echo2   (io04 en esquema — verificar)

#define PIN_TRIGGER_DER     14   // io14_espi → trigger3 (J7 Right Ultrasonic)
#define PIN_ECHO_DER        21   // io21_espi → echo3  (verificar en PCB)

// =====================================================
//  LEDS DE ESTADO
// =====================================================
#define PIN_LED_VERDE       2    // LED Verde (J10) — sistema encendido

// =====================================================
//  UART DE DEBUG — J2-UART1
// =====================================================
#define PIN_UART_TX         43   // txd0_espi → TX (pin 3 de J2)
#define PIN_UART_RX         44   // rxd0_espi → RX (pin 4 de J2)

// =====================================================
//  PINES RESERVADOS (no usar)
// =====================================================
// GPIO19/20 = USB D-/D+
// GPIO15/16 = cristal externo (XTAL_P / XTAL_N)
// GPIO35/36/37 = PSRAM octal (modulo N8R8)
// EN  → SW1 (ENABLE, activo LOW)
// IO0 → SW2 (BOOT, activo LOW), R3 10K pull-up a 3.3V
// Expansion J10 libre: io01, io03, io09, io17, io41, io42, io46

// =====================================================
//  CANALES PWM (LEDC)
// =====================================================
// En el ESP32-S3 cada par de canales comparte timer: (0,1) (2,3) (4,5) (6,7).
// Canales con frecuencias distintas deben ir en timers distintos.
#define PWM_CH_MOTOR_A      0
#define PWM_CH_MOTOR_B      1
#define PWM_CH_SUCCION      2
#define PWM_CH_CEPILLO      4

#define PWM_RESOLUTION_BITS 8          // duty 0-255
#define PWM_DUTY_MAX        255
#define MOTOR_PWM_FREQ_HZ   20000      // traccion: 20 kHz

// =====================================================
//  ULTRASONICOS
// =====================================================
#define ULTRASONIC_TIMEOUT_US   15000   // 15 ms ≈ 2.5 m de alcance util
#define ULTRASONIC_TRIGGER_US   10      // pulso de trigger
#define ULTRASONIC_PERIODO_MS   25      // un sensor cada 25 ms → cada sensor a ~13 Hz
#define ULTRASONIC_MAX_CM       250.0f

// =====================================================
//  NAVEGACION (valores por defecto, ajustables desde la web)
// =====================================================
#define NAV_UMBRAL_CM_DEFAULT   20      // distancia de obstaculo
#define NAV_VEL_DEFAULT         60      // velocidad de avance (0-100)
#define NAV_TIEMPO_RETROCESO_MS 600
#define NAV_TIEMPO_GIRO_MS      450
#define NAV_PARED_OBJETIVO_CM   15      // distancia a la pared en modo BORDES
#define NAV_ESPIRAL_MAX_MS      30000   // duracion maxima de la espiral

#define MANUAL_TIMEOUT_MS       400     // sin comando DRIVE en este tiempo → frenar

// Potencias por defecto al iniciar un modo autonomo (0-100 %)
#define SUCCION_POT_DEFAULT     75
#define CEPILLO_POT_DEFAULT     70

// =====================================================
//  WIFI / SERVIDOR WEB
// =====================================================
#define WIFI_AP_SSID        "Roomba-ESP32"
#define WIFI_AP_PASS        "roomba1234"     // minimo 8 caracteres
#define WIFI_AP_CANAL       6
#define TELEMETRIA_PERIODO_MS 150

// Serial debug
#define BAUD_RATE           115200
