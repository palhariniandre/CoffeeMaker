#include "controle.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN_RELE GPIO_NUM_16
#define PIN_BUZZER GPIO_NUM_25

void ligar_rele_por_5_min() {
    gpio_set_direction(PIN_RELE, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_RELE, 1);
    vTaskDelay(5 * 60 * 1000 / portTICK_PERIOD_MS); // 5 minutos
}

void desliga_rele() {
    gpio_set_level(PIN_RELE, 0);
}

void buzinar_10s() {
    gpio_set_direction(PIN_BUZZER, GPIO_MODE_OUTPUT);
    for (int i = 0; i < 10; i++) {
        gpio_set_level(PIN_BUZZER, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(PIN_BUZZER, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void buzinar_agendado() {
    gpio_set_direction(PIN_BUZZER, GPIO_MODE_OUTPUT);

    gpio_set_level(PIN_BUZZER, 1);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_BUZZER, 0);
    vTaskDelay(500 / portTICK_PERIOD_MS);

}
