# Estruturação de Código para Arquitetura Modular
---

## Proposta do Exercício

A seguir, você encontra um código que inicializa o LED embutido da Raspberry Pi Pico W e o faz piscar indefinidamente:

```
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        return -1;
    }

    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(500);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(500);
    }
}
```
### Tarefa

Reestruture esse código seguindo a arquitetura modular recomendada:

- drivers/: código de controle direto de hardware (ex: LED embutido).
- hal/: camada de abstração que expõe funções simples e reutilizáveis.
- app/: lógica da aplicação principal (fluxo de controle, interface com usuário).
- include/: cabeçalhos necessários.
- CMakeLists.txt: organize a compilação conforme necessário.

### Objetivos Específicos

1. Crie um driver led_embutido.c que use diretamente a API cyw43_arch.

2. Implemente um hal_led.c com a função hal_led_toggle() que abstraia o driver.

3. Escreva um main.c simples no diretório app/ que apenas use a função da HAL para piscar o LED.

### Estrutura Esperada do Projeto

```
projeto/
├── app/
│   └── main.c
├── drivers/
│   └── led_embutido.c
├── hal/
│   └── hal_led.c
├── include/
│   ├── led_embutido.h
│   └── hal_led.h
└── CMakeLists.txt
```

---

## Objetivo Principal

O objetivo principal desta atividade é reestruturar um código simples de piscar o LED embutido da Raspberry Pi Pico W utilizando uma arquitetura modular. O propósito é aplicar boas práticas de engenharia de software embarcado, organizando o projeto em módulos bem definidos.

---

## Funcionamento

1. A aplicação (main.c) inicia os periféricos com hal_led_init(), que chama driver_init() para configurar a arquitetura CYW43.

2. Dentro de um loop infinito, a aplicação chama hal_led_toggle() a cada 500 ms.

3. A função hal_led_toggle() alterna o estado do LED com base em uma variável de controle (flag_on).

4. A função led_set_state() no driver define o valor lógico do pino correspondente ao LED embutido usando a API do SDK.

---

## Bibliotecas do Pacote SDK da Raspberry Pi Pico utilizadas

- pico/stdlib.h: funções utilitárias e tipos padrão (ex: bool, sleep_ms, stdio_init_all).

- pico/cyw43_arch.h: funções para inicializar e controlar o módulo CYW43 da Pico W, incluindo o LED embutido (cyw43_arch_init, cyw43_arch_gpio_put).
