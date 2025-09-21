#ifndef AGENDA_H
#define AGENDA_H

#include <time.h>

void set_agendamento(const char *msg);
time_t obter_agendamento(void);
void obter_horario_ntp(void);
void esperar_agendamento(int segundos);
void salvar_agendamento_nvs(time_t ag);
time_t carregar_agendamento_nvs(void);
time_t obter_horario_ntp_sync(void);

#endif // AGENDA_H
