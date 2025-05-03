#include "include/hal_led.h" // Inclui o cabeçalho da HAL (Hardware Abstraction Layer) que declara as funções `hal_led_toggle` e `hal_led_init`, e também já inclui o driver `led_embutido.h`

bool flag_on = false; // Define uma variável global `flag_on` que armazena o estado atual do LED
// Essa variável ajuda a alternar o estado do LED sem depender de hardware externo

void hal_led_toggle() { // Função da HAL para alternar (toggle) o estado do LED embutido
// Essa função será chamada pela aplicação e usa a camada driver para mudar o LED

    if (flag_on) { // Se o LED estiver ligado

        flag_on = false;  // Atualiza o estado da flag para desligado
        led_set_state(false); // Chama a função do driver para apagar o LED

    }

    else if (!flag_on) { // Se o LED estiver desligado

        flag_on = true; // Atualiza o estado da flag para ligado
        led_set_state(true); // Chama a função do driver para acender o LED

    }
}

int hal_led_init() { // Função da HAL para inicializar o controle do LED. Ela simplesmente repassa a responsabilidade para o driver
    
    return driver_init(); // Chama a função de inicialização do driver (que por sua vez chama cyw43_arch_init)
    
 }