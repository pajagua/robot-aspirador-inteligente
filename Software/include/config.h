// pines y macros
#pragma once

// =====================================================
//  config.h — Mapa de pines y macros del sistema
//  PCB: Roomba ESP32-S3 Custom
//  MCU: ESP32-S3-WROOM-1-N8R8
//  NOTA: Todos los pines son GPIO del ESP32-S3
// =====================================================

// --- FUENTES DE ALIMENTACION (referencia, no son pines) ---
// Bateria 12V  → TPS54202 Step-Down → 5V (Fuente5V)
// Fuente5V     → LM1117-3.3         → 3.3V (Fuente3V, alimenta el ESP)

// =====================================================
//  DRIVER DE MOTORES — TB6612FNG
// =====================================================
// Motor A = Motor Izquierdo (J3)
// Motor B = Motor Derecho   (J4)

#define PIN_STBY        4  // io4_espi → STBY: Encendido/Reposo del Driver
                             // HIGH = activo | LOW = standby (ahorra energia)

#define PIN_PWMA        7   // io7_espi → PWMA: Velocidad Motor A (Izquierdo)
#define PIN_AIN1        5   // io05_espi → AIN1: Dirección Motor A (1)
#define PIN_AIN2        6   // io06_espi → AIN2: Dirección Motor A (2)

#define PIN_PWMB        8   // io08_espi → PWMB: Velocidad Motor B (Derecho)
#define PIN_BIN1        39  // io39_espi → BIN1: Dirección Motor B (1)  (io17 en esquema)
#define PIN_BIN2        40  // io40_espi → BIN2: Dirección Motor B (2)

// Macros de direccion (combinaciones AIN1/AIN2)
//  Adelante:   AIN1=HIGH, AIN2=LOW
//  Atras:      AIN1=LOW,  AIN2=HIGH
//  Freno:      AIN1=HIGH, AIN2=HIGH
//  Libre:      AIN1=LOW,  AIN2=LOW

// =====================================================
//  MOTOR DE SUCCION — MOSFET NMOS Q1
// =====================================================
// Controlado via PWM: duty cycle 0-255 = 0-100% potencia
// Protegido con diodo SS14 (DiodoProteccion1)

#define PIN_MOTOR_SUCCION   47   // io47_espi → Gate del NMOS

// =====================================================
//  SENSORES ULTRASONICOS — HC-SR04 (x3)
// =====================================================
// Echo reducido de 5V a 3.3V con divisor resistivo (R9/R10, R13/R14, R11/R12)
// Trigger: salida directa del ESP (3.3V es suficiente para el sensor)

#define PIN_TRIGGER_IZQ     10   // io10_espi → trigger1 (J5 Left Ultrasonic)
#define PIN_ECHO_IZQ        11   // io11_espi → echo1

#define PIN_TRIGGER_CENTRO  12   // io12_espi → trigger2 (J6 Central Ultrasonic)
#define PIN_ECHO_CENTRO     13   // io13_espi → echo2   (io04 en esquema — verificar)

#define PIN_TRIGGER_DER     14   // io14_espi → trigger3 (J7 Right Ultrasonic)
#define PIN_ECHO_DER        21   // io21_espi → echo3  (verificar en PCB)

// =====================================================
//  LEDS DE ESTADO
// =====================================================
#define PIN_LED_VERDE       1  // io1_espi → LED Verde (J10) — Indica que el sistema está encendido y funcionando
#define PIN_LED_ROJO        2  // io2_espi → LED Rojo (J10) — Indica error o estado crítico

// =====================================================
//  UART DE DEBUG — J2-UART1
// =====================================================
#define PIN_UART_TX         43   // txd0_espi → TX (pin 3 de J2)
#define PIN_UART_RX         44   // rxd0_espi → RX (pin 4 de J2)

// =====================================================
//  USB NATIVO (no tocar — reservados por hardware)
// =====================================================
// GPIO19 = USB D-  (USB_N)
// GPIO20 = USB D+  (USB_P)
// Conectados al conector USB-C J11 via UJ20-C-H-G-SMT

// =====================================================
//  CRISTAL EXTERNO
// =====================================================
// GPIO15 = io15_espi (XTAL_P) — C11 12pF
// GPIO16 = io16_espi (XTAL_N) — C12 12pF
// No usar estos pines para otra funcion

// =====================================================
//  BOOT / ENABLE
// =====================================================
// EN   → SW1 (boton ENABLE, activo en LOW)
// IO0  → SW2 (boton BOOT, activo en LOW para modo bootloader)
//        R3 10K pull-up a 3.3V

// =====================================================
//  PINES DE EXPANSION — J10 (TD-105-T-A)
// =====================================================
// io01, io03, io09, io17, io41, io42, io43, io46, io48
// Ver esquematico para asignacion completa

// =====================================================
//  PARAMETROS DEL SISTEMA
// =====================================================

// Velocidad PWM motores (0-255)
#define MOTOR_PWM_MAX       255
#define MOTOR_PWM_MIN       0
#define MOTOR_PWM_DEFAULT   180   // ~70% — arranque seguro

// Ultrasonico
#define ULTRASONIC_TIMEOUT_US   30000   // 30ms = ~5 metros max
#define ULTRASONIC_TRIGGER_US   10      // pulso de trigger: 10 microsegundos

// Motor de succion
#define SUCCION_PWM_MAX     255
#define SUCCION_PWM_MIN     0

// Serial debug
#define BAUD_RATE           115200