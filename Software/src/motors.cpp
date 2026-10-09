/*
    motors.cpp - Implementacion TB6612FNG
*/
#include "motors.h"
#include "pwm.h"

typedef struct {
    uint8_t in1, in2, pwm, canal;
} MotorPins;

static const MotorPins _motor_a = { PIN_AIN1, PIN_AIN2, PIN_PWMA, PWM_CH_MOTOR_A };
static const MotorPins _motor_b = { PIN_BIN1, PIN_BIN2, PIN_PWMB, PWM_CH_MOTOR_B };

static void _set_motor(const MotorPins &m, int velocidad) {
    // velocidad: -100 a 100
    velocidad = constrain(velocidad, -100, 100);
    digitalWrite(m.in1, velocidad > 0 ? HIGH : LOW);
    digitalWrite(m.in2, velocidad < 0 ? HIGH : LOW);
    pwm_write(m.pwm, m.canal, (uint32_t)abs(velocidad) * PWM_DUTY_MAX / 100);
}

static void _init_motor(const MotorPins &m) {
    pinMode(m.in1, OUTPUT);
    pinMode(m.in2, OUTPUT);
    pwm_attach(m.pwm, m.canal, MOTOR_PWM_FREQ_HZ, PWM_RESOLUTION_BITS);
}

void motors_init() {
    pinMode(PIN_STBY, OUTPUT);
    _init_motor(_motor_a);
    _init_motor(_motor_b);
    motors_brake();
    motors_standby();
}

void motors_standby() {
    digitalWrite(PIN_STBY, LOW);   // TB6612FNG en standby (bajo consumo)
}

void motors_wake() {
    digitalWrite(PIN_STBY, HIGH);  // TB6612FNG activo
}

void motor_izq(int velocidad) { _set_motor(_motor_a, velocidad); }
void motor_der(int velocidad) { _set_motor(_motor_b, velocidad); }

void motors_drive(int lineal, int angular) {
    // Conduccion diferencial: angular > 0 gira a la derecha
    motor_izq(lineal + angular);
    motor_der(lineal - angular);
}

void motors_stop() {
    // Libre (sin freno, rueda libremente)
    _set_motor(_motor_a, 0);
    _set_motor(_motor_b, 0);
}

void motors_brake() {
    // Freno: IN1 = IN2 = HIGH
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, HIGH);
    digitalWrite(PIN_BIN1, HIGH);
    digitalWrite(PIN_BIN2, HIGH);
    pwm_write(PIN_PWMA, PWM_CH_MOTOR_A, 0);
    pwm_write(PIN_PWMB, PWM_CH_MOTOR_B, 0);
}
