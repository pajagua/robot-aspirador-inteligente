#include "suction.h"
#include "pwm.h"

static uint8_t _potencia_actual = 0;

#if PIN_SUCCION_FG >= 0
static volatile uint32_t _pulsos_fg = 0;
static uint32_t _rpm_t0 = 0;
static uint16_t _rpm = 0;

static void IRAM_ATTR _isr_fg() { _pulsos_fg++; }
#endif

static void _write_duty(uint32_t duty) {
#if SUCCION_PWM_INVERTIDO
    duty = PWM_DUTY_MAX - duty;
#endif
    pwm_write(PIN_SUCCION_PWM, PWM_CH_SUCCION, duty);
}

void succion_init() {
    pwm_attach(PIN_SUCCION_PWM, PWM_CH_SUCCION, SUCCION_PWM_FREQ_HZ, PWM_RESOLUTION_BITS);
    succion_off();
#if PIN_SUCCION_FG >= 0
    pinMode(PIN_SUCCION_FG, INPUT_PULLUP);   // FG suele ser colector abierto
    attachInterrupt(digitalPinToInterrupt(PIN_SUCCION_FG), _isr_fg, FALLING);
    _rpm_t0 = millis();
#endif
}

void succion_set(uint8_t porcentaje) {
    porcentaje = min<uint8_t>(porcentaje, 100);
    _potencia_actual = porcentaje;
    if (porcentaje == 0) {
        _write_duty(0);
        return;
    }
    // 1..100 % → SUCCION_DUTY_MIN_PCT..100 % para evitar la zona muerta del driver
    uint32_t pct = map(porcentaje, 1, 100, SUCCION_DUTY_MIN_PCT, 100);
    _write_duty(pct * PWM_DUTY_MAX / 100);
}

void succion_on()  { succion_set(100); }
void succion_off() { succion_set(0); }

uint8_t succion_get() { return _potencia_actual; }

uint16_t succion_rpm() {
#if PIN_SUCCION_FG >= 0
    uint32_t ahora = millis();
    uint32_t dt = ahora - _rpm_t0;
    if (dt >= 500) {
        noInterrupts();
        uint32_t pulsos = _pulsos_fg;
        _pulsos_fg = 0;
        interrupts();
        _rpm = (uint16_t)(pulsos * 60000UL / (dt * SUCCION_FG_PULSOS_VUELTA));
        _rpm_t0 = ahora;
    }
    return _rpm;
#else
    return 0;
#endif
}
