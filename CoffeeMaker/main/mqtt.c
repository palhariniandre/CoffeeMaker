#include "mqtt.h"
#include "agenda.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_event.h"  // Necessário para tipos de eventos

static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t client;

// Handler com nova assinatura exigida pelo ESP-IDF 5+
static void mqtt_event_handler_cb(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            esp_mqtt_client_subscribe(event->client, "/cafeteira/agendar", 0);
            ESP_LOGI(TAG, "Conectado e inscrito.");
            break;

        case MQTT_EVENT_DATA: {
            char msg[32];
            snprintf(msg, sizeof(msg), "%.*s", event->data_len, event->data);
            ESP_LOGI(TAG, "Mensagem recebida: %s", msg);
            set_agendamento(msg);
            break;
        }

        default:
            break;
    }
}

void mqtt_start() {
    esp_mqtt_client_config_t cfg = {
        .broker.address.uri = "mqtt://192.168.0.66",
        .broker.address.port = 1883,
        .credentials.username = NULL, // se precisar, coloque o usuário
        .credentials.authentication.password = NULL // se precisar, coloque a senha
        
    };

    client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, NULL);
    esp_mqtt_client_start(client);
}
