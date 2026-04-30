
/*
    motores.cpp - Implementacion TB6612FNG
*/
#include "motors.h"

static void _set_motor_a(int speed){
    // speed: -255 a 255
    if (speed > 0){
        digitalWrite(PIN_AIN1, HIGH);
        digitalWrite(PIN_AIN2, LOW);
    } else if (speed < 0){
        digitalWrite(PIN_AIN1, LOW);
        digitalWrite(PIN_AIN2, HIGH);
        speed = -speed; // Convertir a positivo para PWM
    } else {
        digitalWrite(PIN_AIN1, LOW);
        digitalWrite(PIN_AIN2, LOW);
    }
    ledcWrite(PWM_CH_MOTOR_A, speed);
}

static void _set_motor_b(int speed){
    // speed: -255 a 255
    if (speed > 0){
        digitalWrite(PIN_BIN1, HIGH);
        digitalWrite(PIN_BIN2, LOW);
    } else if (speed < 0){
        digitalWrite(PIN_BIN1, LOW);
        digitalWrite(PIN_BIN2, HIGH);
        speed = -speed; // Convertir a positivo para PWM
    } else {
        digitalWrite(PIN_BIN1, LOW);
        digitalWrite(PIN_BIN2, LOW);
    }
    ledcWrite(PWM_CH_MOTOR_B, speed);
}

void motors_init(){
    pinMode(PIN_STBY, OUTPUT);
    pinMode(PIN_AIN1, OUTPUT);
    pinMode(PIN_AIN2, OUTPUT);
    pinMode(PIN_BIN1, OUTPUT);
    pinMode(PIN_BIN2, OUTPUT);

    ledcSetup(PWM_CH_MOTOR_A, PWM_FREQ_HZ, PWM_RESOLUTION);
    ledcSetup(PWM_CH_MOTOR_B, PWM_FREQ_HZ, PWM_RESOLUTION);
    ledcAttachPin(PIN_PWMA, PWM_CH_MOTOR_A);
    ledcAttachPin(PIN_PWMB, PWM_CH_MOTOR_B);

    motors_brake();
    motors_standby();
}

void motors_standby(){
    digitalWrite(PIN_STBY, LOW); // TB6612FNG en modo standby (bajo consumo)
}

void motors_wake(){
    digitalWrite(PIN_STBY, HIGH); // TB6612FNG activo
}   

void motor_izq(int8_t velocidad){
    // Convierte -100 a 100 en -255 a 255
    int speed = map(velocidad, -100, 100, -255, 255);
    _set_motor_a(speed);
}   

void motor_der(int8_t velocidad){
    // Convierte -100 a 100 en -255 a 255
    int speed = map(velocidad, -100, 100, -255, 255);
    _set_motor_b(speed);
}

void motors_drive(int8_t lineal, int8_t angular){
    // Conduccion diferencial simple
    // linel: velocidad de avance (-100 =atras, 100 = adelante)
    // angular: rotacion (-100 = giro izquierda, 100 = giro derecha)
    int izq = constrain(lineal + angular, -100, 100);
    int der = constrain(lineal - angular, -100, 100);
    motor_izq(izq);
    motor_der(der);
}

void motors_stop(){
    // Libre (sin freno, rueda libremente)
    _set_motor_a(0);
    _set_motor_b(0);
}

void motors_brake(){
    // Freno (ambos pines HIGH)
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, HIGH);
    digitalWrite(PIN_BIN1, HIGH);
    digitalWrite(PIN_BIN2, HIGH);
    ledcWrite(PWM_CH_MOTOR_A, 0);
    ledcWrite(PWM_CH_MOTOR_B, 0);
}