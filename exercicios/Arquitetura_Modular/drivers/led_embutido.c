#include "pico/cyw43_arch.h" // Inclui a biblioteca da arquitetura CYW43 da Raspberry Pi Pico W que fornece funções para controlar o LED embutido 
#include "include/led_embutido.h" // Inclui o cabeçalho do driver (led_embutido.h) onde as funções `led_set_state` e `driver_init` estão declaradas para uso em outros arquivos

 void led_set_state(bool state) { // Função que controla o estado do LED (ligado ou desligado)

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, state);  // Usa a função da biblioteca cyw43 para definir o estado do LED embutido
     // `CYW43_WL_GPIO_LED_PIN` é o identificador do pino correspondente ao LED da placa
     // `state` deve ser `true` (1) para ligar o LED ou `false` (0) para desligar
 }

 int driver_init() { // Função que inicializa a arquitetura CYW43 (necessária para controlar o LED)

    return cyw43_arch_init();

 }
