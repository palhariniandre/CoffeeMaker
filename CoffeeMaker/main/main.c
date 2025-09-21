#include "wifi.h"
#include "mqtt.h"
#include <time.h>
#include "agenda.h"
#include "controle.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "MAIN";



void app_main(void) {
    // Inicializa NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Inicializa Wi-Fi
    init_wifi();

    // Obter hora NTP sincronizada
    time_t agora = obter_horario_ntp_sync();

    // Inicializa MQTT
    mqtt_start();

    // Aguarda mensagem MQTT com agendamento por até 30s
    ESP_LOGI(TAG, "Aguardando agendamento...");
    esperar_agendamento(300);

    time_t agendamento = obter_agendamento();
    ESP_LOGI(TAG, "DEBUG: agora = %ld, agendamento = %ld, diff = %ld", agora, agendamento, agora - agendamento);

    if (agendamento > agora) {
        int64_t delta = (agendamento - agora) * 1000000LL;
        ESP_LOGI(TAG, "Agendamento para %lld segundos. Dormindo...", delta / 1000000);
        buzinar_agendado(); 
        buzinar_agendado(); 
        esp_sleep_enable_timer_wakeup(delta);
        esp_deep_sleep_start();
    } 
    else if (agendamento != 0 && agora >= agendamento && (agora - agendamento) <= 120) {
        ESP_LOGI(TAG, "Hora do café! (atraso tolerado de %ld segundos)", agora - agendamento);
        buzinar_agendado();
        ligar_rele_por_5_min();

        ESP_LOGI(TAG, "rele ligado por 5 minutos.");
        buzinar_10s();

        ESP_LOGI(TAG, "buzzer tocou por 10 segundos.");
        desliga_rele();
        ESP_LOGI(TAG, "rele desligado. Dormindo 1 hora.");
        salvar_agendamento_nvs(0); // limpa agendamento
        esp_sleep_enable_timer_wakeup(3600 * 1000000LL);
        esp_deep_sleep_start();
    }
    else if (agendamento == 0) {
        ESP_LOGI(TAG, "Nenhum agendamento. Dormindo 1 hora.");
        buzinar_agendado();
        esp_sleep_enable_timer_wakeup(3600 * 1000000LL);
        esp_deep_sleep_start();
    } 
    else {
        ESP_LOGI(TAG, "Agendamento expirado há mais de 60s. Limpando. Dormindo 1 hora.");
        buzinar_agendado();
        buzinar_agendado();
        buzinar_agendado();
        salvar_agendamento_nvs(0); // limpar agendamento expirado
        esp_sleep_enable_timer_wakeup(3600 * 1000000LL);
        esp_deep_sleep_start();
        
    }
}
