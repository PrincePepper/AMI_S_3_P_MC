#include <periph/gpio.h>
#include <periph/pwm.h>
#include <xtimer.h>
#include <board.h>
#include <thread.h>

#define PWM_LED_DEVICE (PWM_DEV(3))

#define PHOTORESISTOR GPIO_PIN(PORT_A, 0)
#define LED           GPIO_PIN(PORT_A, 1)

int main(void) {
    xtimer_init();

    gpio_init(PHOTORESISTOR, GPIO_IN);
    gpio_init(LED, GPIO_OUT);

    pwm_init(PWM_LED_DEVICE, PWM_LEFT, 1000, 255);


    while(true){
        pwm_set(PWM_LED_DEVICE, 1, (gpio_read(PHOTORESISTOR) * 255));
    }

    return 0;
}
