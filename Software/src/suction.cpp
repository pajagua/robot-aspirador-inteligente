#include "suction.h"

static uint8_t _potencia_actual = 0;

void succion_init(){
    ledcSetup(PWM_CH_SUCCION, PWM_FREQ_HZ_SUCCION, 8);
    ledcAttachPin(PIN_MOTOR_SUCCION, PWM_CH_SUCCION);
    succion_off();
}

void succion_set(uint8_t porcentaje){
    porcentaje = constrain(porcentaje, 0, 100);
    _potencia_actual = porcentaje;
    uint8_t duty = map(porcentaje, 0, 100, 0, 255);
    ledcWrite(PWM_CH_SUCCION, duty);
}

void succion_on(){
    succion_set(100);
}

void succion_off(){
    _potencia_actual = 0;
    ledcWrite(PWM_CH_SUCCION, 0);
}

uint8_t succion_get(){
    return _potencia_actual;
}
