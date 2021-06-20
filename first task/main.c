#include <periph/gpio.h>
#include <periph/pwm.h>
#include <xtimer.h>
#include <board.h>
#include <thread.h>

#define PWM_LED_DEVICE (PWM_DEV(0))

#define MAX_LIGHT 255.0

int leds[4] = {0, 1, 2, 3};

typedef struct Thread {
    kernel_pid_t pid;
    char stack[THREAD_STACKSIZE_MAIN];
} Thread;

Thread blue_thread;
Thread white_thread;
Thread green_thread;
Thread red_thread;


bool mode = false;

unsigned int speed = 1000;


void blink_blue_mode_1(void);

void blink_white_mode_1(void);

void blink_green_mode_1(void);

void blink_red_mode_1(void);


void blink_blue_mode_2(unsigned int current_speed);

void blink_white_mode_2(unsigned int current_speed);

void blink_green_mode_2(unsigned int current_speed);

void blink_red_mode_2(unsigned int current_speed);


void button_click(void *arg);

void button_short_click(void);

void button_long_click(void);


static void *blue_thread_function(void *arg);

static void *white_thread_function(void *arg);

static void *green_thread_function(void *arg);

static void *red_thread_function(void *arg);


int main(void) {
    xtimer_init();

    gpio_init(leds[0], GPIO_OUT);

    gpio_init_int(BTN0_PIN, GPIO_IN_PU, GPIO_BOTH, button_click, NULL);

    pwm_init(PWM_LED_DEVICE, PWM_CENTER, 1000, MAX_LIGHT);

    blue_thread.pid  = thread_create(blue_thread.stack,  sizeof(blue_thread.stack),  THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST, blue_thread_function,  NULL, "blue thread");
    white_thread.pid = thread_create(white_thread.stack, sizeof(white_thread.stack), THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST, white_thread_function, NULL, "white thread");
    green_thread.pid = thread_create(green_thread.stack, sizeof(green_thread.stack), THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST, green_thread_function, NULL, "green thread");
    red_thread.pid   = thread_create(red_thread.stack,   sizeof(red_thread.stack),   THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST, red_thread_function,   NULL, "red thread");

    return 0;
}


void blink_blue_mode_1(void) {
    pwm_set(PWM_LED_DEVICE, leds[0], MAX_LIGHT);
    // gpio_set(leds[0]);
    xtimer_msleep(1000);
    // gpio_clear(leds[0]);
    pwm_set(PWM_LED_DEVICE, leds[0], 0);
    xtimer_msleep(1000);
}

void blink_white_mode_1(void) {
    pwm_set(PWM_LED_DEVICE, leds[1], MAX_LIGHT);
    xtimer_msleep(500);
    pwm_set(PWM_LED_DEVICE, leds[1], 0);
    xtimer_msleep(500);
    pwm_set(PWM_LED_DEVICE, leds[1], MAX_LIGHT);
    xtimer_msleep(500);
    pwm_set(PWM_LED_DEVICE, leds[1], 0);
    xtimer_msleep(500);
}

void blink_green_mode_1(void) {
    for (uint8_t i = 0; i < MAX_LIGHT; i++) {
        pwm_set(PWM_LED_DEVICE, leds[2], i);
        xtimer_msleep(1000.0 / MAX_LIGHT);
    }

    for (uint8_t i = MAX_LIGHT; i > 0; i--) {
        pwm_set(PWM_LED_DEVICE, leds[2], i);
        xtimer_msleep(1000.0 / MAX_LIGHT);
    }
}

void blink_red_mode_1(void) {
    for (uint8_t i = MAX_LIGHT; i > 0; i--) {
        pwm_set(PWM_LED_DEVICE, leds[3], i);
        xtimer_msleep(500.0 / MAX_LIGHT);
    }

    for (uint8_t i = 0; i < MAX_LIGHT; i++) {
        pwm_set(PWM_LED_DEVICE, leds[3], i);
        xtimer_msleep(500.0 / MAX_LIGHT);
    }

    pwm_set(PWM_LED_DEVICE, leds[3], 0);
}


void blink_blue_mode_2(unsigned int current_speed) {
    pwm_set(PWM_LED_DEVICE, leds[0], MAX_LIGHT);
    xtimer_msleep(current_speed);
    pwm_set(PWM_LED_DEVICE, leds[0], 0);
    xtimer_msleep(3 * current_speed);
}

void blink_white_mode_2(unsigned int current_speed) {
    xtimer_msleep(current_speed);
    pwm_set(PWM_LED_DEVICE, leds[1], MAX_LIGHT);
    xtimer_msleep(current_speed);
    pwm_set(PWM_LED_DEVICE, leds[1], 0);
    xtimer_msleep(2 * current_speed);
}

void blink_green_mode_2(unsigned int current_speed) {
    xtimer_msleep(2 * current_speed);
    pwm_set(PWM_LED_DEVICE, leds[2], MAX_LIGHT);
    xtimer_msleep(current_speed);
    pwm_set(PWM_LED_DEVICE, leds[2], 0);
    xtimer_msleep(current_speed);
}

void blink_red_mode_2(unsigned int current_speed) {
    xtimer_msleep(3 * current_speed);
    pwm_set(PWM_LED_DEVICE, leds[3], MAX_LIGHT);
    xtimer_msleep(current_speed);
    pwm_set(PWM_LED_DEVICE, leds[3], 0);
}


void button_click(void* arg) {
    (void)arg;

    uint32_t before = xtimer_now_usec();
    uint32_t difference = 0;

    while (!gpio_read(BTN0_PIN))
        difference = xtimer_now_usec() - before;

    if(difference >= 1000000)
        button_long_click();
    else if(difference >= 5000)
        button_short_click();
}

void button_short_click(void) {
    if(mode) {
        int temp = leds[0];
        leds[0] = leds[1];
        leds[1] = leds[2];
        leds[2] = leds[3];
        leds[3] = temp;
    }
    else
        switch (speed) {
            case 1000:
                speed = 500;
                break;

            case 500:
                speed = 300;
                break;

            case 300:
                speed = 1000;
                break;

            default:
                speed = 1000;
                break;
        }
}

void button_long_click(void) {
    mode = !mode;
}


static void *blue_thread_function(void *arg) {
    (void)arg;
    while(true) {
        if(mode)
            blink_blue_mode_1();
        else
            blink_blue_mode_2(speed);
    }

    return NULL;
}

static void *white_thread_function(void *arg) {
    (void)arg;
    while(true) {
        if(mode)
            blink_white_mode_1();
        else
            blink_white_mode_2(speed);
    }

    return NULL;
}

static void *green_thread_function(void *arg) {
    (void)arg;
    while(true) {
        if(mode)
            blink_green_mode_1();
        else
            blink_green_mode_2(speed);
    }

    return NULL;
}

static void *red_thread_function(void *arg) {
    (void)arg;
    while(true) {
        if(mode)
            blink_red_mode_1();
        else
            blink_red_mode_2(speed);
    }

    return NULL;
}
