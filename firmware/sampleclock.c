#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_system.h"

#define LED_PIN 2

// Timer ISR: toggle LED every second
void IRAM_ATTR timer_isr(void *arg) {
    int level = gpio_get_level(LED_PIN);
    gpio_set_level(LED_PIN, !level);
    TIMERG0.int_clr_timers.t0 = 1;
}

void setup_timer() {
    timer_config_t config = {
        .divider = 80,               // 80MHz / 80 = 1MHz
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000000); // 1 second
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
}

void app_main(void) {
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    setup_timer();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // idle
    }
}


// esp32_timer_clock.c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"

#define LED_PIN 2

void IRAM_ATTR timer_isr(void *arg) {
    gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));
    TIMERG0.int_clr_timers.t0 = 1;
}

void app_main(void) {
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    timer_config_t config = {
        .divider = 80,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true,
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000000);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);

    timer_isr_register(
        TIMER_GROUP_0,
        TIMER_0,
        timer_isr,
        NULL,
        ESP_INTR_FLAG_IRAM,
        NULL
    );

    timer_start(TIMER_GROUP_0, TIMER_0);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
