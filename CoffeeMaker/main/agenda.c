#include "agenda.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sntp.h"
#include <time.h>
#include "nvs_flash.h"
#include "nvs.h"

static time_t agendamento = 0;
static const char *TAG = "AGENDA";

void salvar_agendamento_nvs(time_t ag) {
    nvs_handle_t nvs;
    if (nvs_open("storage", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i64(nvs, "agendamento", ag);
        nvs_commit(nvs);
        nvs_close(nvs);
    }
}

time_t carregar_agendamento_nvs() {
    nvs_handle_t nvs;
    time_t ag = 0;
    if (nvs_open("storage", NVS_READONLY, &nvs) == ESP_OK) {
        nvs_get_i64(nvs, "agendamento", &ag);
        nvs_close(nvs);
    }
    return ag;
}

void set_agendamento(const char *msg) {
    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    // tenta interpretar "YYYY-MM-DD HH:MM"
    if (strptime(msg, "%Y-%m-%d %H:%M", &tm)) {
        time_t agora = time(NULL);
        time_t agendado = mktime(&tm);

        if (agendado > agora) {
            agendamento = agendado;
            salvar_agendamento_nvs(agendamento);
            ESP_LOGI(TAG, "Agendado para %s (timestamp: %ld)", msg, agendamento);
        } else {
            ESP_LOGW(TAG, "Data/hora já passou. Limpando agendamento.");
            agendamento = 0;
            salvar_agendamento_nvs(0);
        }
    } 
    else {
        ESP_LOGW(TAG, "Formato inválido. Use YYYY-MM-DD HH:MM");
    }
}



time_t obter_agendamento() {
    if (agendamento == 0) {
        agendamento = carregar_agendamento_nvs();
    }
    return agendamento;
}

void obter_horario_ntp() {
    setenv("TZ", "BRT3", 1);  // Ajustar fuso se necessário
    tzset();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    time_t now = 0;
    struct tm timeinfo;
     

    while (time(&now), localtime_r(&now, &timeinfo), timeinfo.tm_year < 120) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Hora NTP obtida: %s", asctime(&timeinfo));
}
// Função NTP que retorna o timestamp correto
time_t obter_horario_ntp_sync() {
    setenv("TZ", "BRT3", 1);  // Ajusta fuso horário
    tzset();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    time_t now = 0;
    struct tm timeinfo;

    // Espera até que o NTP sincronize (ano >= 2020)
    while (time(&now), localtime_r(&now, &timeinfo), timeinfo.tm_year < 120) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Hora NTP obtida: %s", asctime(&timeinfo));
    return now;  // retorna timestamp correto
}
void esperar_agendamento(int segundos) {
    for (int i = 0; i < segundos; i++) {
        if (obter_agendamento() != 0) break;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
