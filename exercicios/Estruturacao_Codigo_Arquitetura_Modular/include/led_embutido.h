
//As diretivas abaixo evitam que o conteúdo do arquivo seja incluído mais de uma vez durante a compilação, o que poderia causar erros

// Verifica se a macro LED_EMBUTIDO_H já foi definida
#ifndef LED_EMBUTIDO_H 
// Define a macro LED_EMBUTIDO_H para evitar múltiplas inclusões
#define LED_EMBUTIDO_H  

#include "pico/stdlib.h"  // Inclui funções e tipos padrão da Raspberry Pi Pico (ex: bool, sleep_ms)
#include "pico/cyw43_arch.h" // Inclui a biblioteca para controle do chip CYW43 (Wi-Fi e LED embutido)

void led_set_state(bool); // Função que define o estado do LED embutido (true = ligado, false = desligado)
int driver_init(void); // Função que inicializa a arquitetura CYW43, necessária para usar o LED embutido

#endif // Fim da diretiva de inclusão condicional