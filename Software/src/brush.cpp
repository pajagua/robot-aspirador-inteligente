#include "brush.h"
#include "pwm.h"

static uint8_t _potencia_actual = 0;

void cepillo_init() {
    pwm_attach(PIN_MOTOR_CEPILLO, PWM_CH_CEPILLO, CEPILLO_PWM_FREQ_HZ, PWM_RESOLUTION_BITS);
    cepillo_off();
}

void cepillo_set(uint8_t porcentaje) {
    porcentaje = min<uint8_t>(porcentaje, 100);
    _potencia_actual = porcentaje;
    pwm_write(PIN_MOTOR_CEPILLO, PWM_CH_CEPILLO, (uint32_t)porcentaje * PWM_DUTY_MAX / 100);
}

void cepillo_on()  { cepillo_set(100); }
void cepillo_off() { cepillo_set(0); }

uint8_t cepillo_get() { return _potencia_actual; }
