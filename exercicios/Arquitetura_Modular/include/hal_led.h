// Verifica se a macro HAL_LED_H já foi definida
#ifndef HAL_LED_H 
// Define a macro HAL_LED_H para evitar múltiplas inclusões
#define HAL_LED_H 

#include "pico/stdlib.h" // Inclui a biblioteca padrão da Pico (ex: tipos como bool, funções de delay)
#include "led_embutido.h" // Inclui o cabeçalho do driver do LED embutido, pois a HAL utiliza suas funções

void hal_led_toggle(void); // Função da HAL que alterna o estado do LED (liga se estiver desligado e vice-versa)
int hal_led_init(void); // Função da HAL que inicializa o LED embutido chamando a função de driver correspondente

#endif // Fim da diretiva de inclusão condicional