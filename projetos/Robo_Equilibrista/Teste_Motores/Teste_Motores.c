#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h" 
#include "hardware/gpio.h" 
#include "hardware/clocks.h" 

#define BUTTON_A  5 
#define BUTTON_B  6
#define INA1 4
#define INA2 9
#define PWM_A 8
#define INB1 18
#define INB2 19
#define PWM_B 16
#define STAND_BY 20
#define PERIOD 2000
#define DIVIDER_PWM 16.

uint16_t pwm_val = 0;

void setup_gpios() {

    // Botões com pull-up
    gpio_init(BUTTON_A); 
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A); 

    gpio_init(BUTTON_B); 
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B); 

    gpio_init(INA1);
    gpio_set_dir(INA1, GPIO_OUT);

    gpio_init(INA2);
    gpio_set_dir(INA2, GPIO_OUT);

    gpio_init(INB1);
    gpio_set_dir(INB1, GPIO_OUT);

    gpio_init(INB2);
    gpio_set_dir(INB2, GPIO_OUT);

    gpio_init(STAND_BY);
    gpio_set_dir(STAND_BY, GPIO_OUT);
    gpio_put(STAND_BY, 1);

    gpio_init(PWM_A);
    gpio_init(PWM_B);

    gpio_set_function(PWM_A, GPIO_FUNC_PWM);
    gpio_set_function(PWM_B, GPIO_FUNC_PWM);

    uint slice_a = pwm_gpio_to_slice_num(PWM_A);
    uint slice_b = pwm_gpio_to_slice_num(PWM_B); 
    
    pwm_set_clkdiv(slice_a, DIVIDER_PWM);
    pwm_set_clkdiv(slice_b, DIVIDER_PWM);

    pwm_set_wrap(slice_a, PERIOD);      
    pwm_set_wrap(slice_b, PERIOD);

    pwm_set_gpio_level(PWM_A, pwm_val);
    pwm_set_gpio_level(PWM_B, pwm_val);

    pwm_set_enabled(slice_a, true);          
    pwm_set_enabled(slice_b, true);         
}

void start_engines() {

    pwm_val = 128 << 8;

    if(gpio_get(BUTTON_A) == 0) {

        gpio_put(INA1, 1);
        gpio_put(INA2, 0);
        gpio_put(INB1, 1);
        gpio_put(INB2, 0);

        pwm_set_gpio_level(PWM_A, pwm_val);
        pwm_set_gpio_level(PWM_B, pwm_val);

        sleep_ms(100);

    }
}

void stop_engines() {

    if(gpio_get(BUTTON_B) == 0) {

    pwm_set_gpio_level(PWM_A, 0);
    pwm_set_gpio_level(PWM_B, 0);

    sleep_ms(100);

    }
}

int main()
{
    stdio_init_all();
    setup_gpios();

    while (true) {

        start_engines();

        stop_engines();

        sleep_ms(300);
    }
}
