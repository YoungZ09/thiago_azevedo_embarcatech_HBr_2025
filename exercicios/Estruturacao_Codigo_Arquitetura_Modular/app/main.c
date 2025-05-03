#include "pico/stdlib.h" // Inclui a biblioteca padrão da Raspberry Pi Pico que fornece funções como `stdio_init_all()` e `sleep_ms()`
#include "include/hal_led.h" // Inclui o cabeçalho da HAL (camada de abstração de hardware) que expõe as funções `hal_led_init()` e `hal_led_toggle()` usadas no `main`


int main() {
    
    stdio_init_all();  // Inicializa todos os canais de entrada/saída padrão (ex: UART para debug)
    // Função necessária para que o sistema funcione corretamente com funções de I/O

    if (hal_led_init()) { // Inicializa a HAL do LED. Internamente, isso chama a função `driver_init()`
        return -1; // Se a inicialização falhar (retorno diferente de zero), o programa encerra com erro
    }

    while (true) { 
        
        hal_led_toggle(); // Alterna o estado do LED usando a função da HAL
        sleep_ms(500); // Aguarda 500 milissegundos antes de alternar o LED novamente. Isso cria o efeito de "piscar" com meio segundo ligado e meio segundo desligado
    }
}