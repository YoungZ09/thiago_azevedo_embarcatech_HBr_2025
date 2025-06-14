#include "FreeRTOS.h" // Biblioteca principal do FreeRTOS (tipos, macros, configurações)
#include "task.h" // Fornece funções de criação, controle e gerenciamento de tarefas
#include "hardware/pwm.h" // Controle de sinais PWM da Raspberry Pi Pico
#include "hardware/gpio.h" // Funções de controle de GPIOs (entrada/saída)
#include "hardware/clocks.h" // Usada para obter a frequência do clock do sistema (para PWM)
#include "pico/stdlib.h" // Biblioteca padrão para GPIO, UART, SPI, I2C etc.
#include "pico/time.h" // Funções de tempo baseadas em software (como delays)
#include "pico/types.h" // Tipos de dados padrão do SDK Pico
#include <stdio.h> // Funções padrão de entrada/saída, como printf

// GPIOs conforme mapeamento da BitDogLab
#define GREEN_LED      11 // GPIO do LED verde
#define BLUE_LED       12 // GPIO do LED azul
#define RED_LED        13 // GPIO do LED vermelho
#define BUZZER         21 // GPIO conectado ao buzzer A
#define BUTTON_A       5 // GPIO do botão A
#define BUTTON_B       6 // GPIO do botão B
#define BUZZER_FREQUENCY 5000 // Frequência do PWM para o buzzer A (5 kHz)

// Handles das tarefas
TaskHandle_t handle_led_task = NULL; // Handle da tarefa de LED RGB
TaskHandle_t handle_buzzer_task = NULL; // Handle da tarefa do Buzzer


// ----- Tarefa do LED RGB -----
void led_rgb_task(void *pvParameters) {

    const uint LED_PINS[3] = {RED_LED, GREEN_LED, BLUE_LED}; // Array com os GPIOs das cores
    int idx = 0; // Índice para alternar entre as cores

    while (1) {

        for (int i = 0; i < 3; i++) { 
            gpio_put(LED_PINS[i], 0); // Desliga todos os LEDs
        }

        gpio_put(LED_PINS[idx], 1); // Liga a cor atual
        idx = (idx + 1) % 3; // Avança para a próxima cor

        vTaskDelay(pdMS_TO_TICKS(500)); // Espera 500ms
    }
}

// ----- Tarefa do Buzzer com PWM -----
void buzzer_task(void *pvParameters) {

    while (1) {
        // Liga PWM
        pwm_set_gpio_level(BUZZER, 255); // Liga o buzzer com duty cycle (baixo, som fraco)
        vTaskDelay(pdMS_TO_TICKS(100)); // Espera 100ms (som curto)      

        // Desliga PWM
        pwm_set_gpio_level(BUZZER, 0); // Desliga o buzzer
        vTaskDelay(pdMS_TO_TICKS(900)); // Espera 900ms (silêncio)
    }
}

// ----- Tarefa de Leitura do Estado dos Botões com Debounce -----
void buttons_task(void *pvParameters) {

    bool led_paused = false; // Estado da tarefa LED RGB (ativa/inativa)
    bool buzzer_paused = false; // Estado da tarefa Buzzer (ativa/inativa)

    while (1) {
        // Leitura do Botão A (controla LED RGB)
        if (gpio_get(BUTTON_A) == 0) { // Botão A pressionado
            vTaskDelay(pdMS_TO_TICKS(50));  // Debounce
            if (gpio_get(BUTTON_A) == 0) { // Botão A ainda pressionado 
                if (!led_paused) { // LED RGB alternando as cores
                    vTaskSuspend(handle_led_task); // Pausa LED RGB
                    led_paused = true;
                } else {
                    vTaskResume(handle_led_task); // Retoma LED RGB
                    led_paused = false;
                }
              }
            }

        // Leitura do Botão B (controla Buzzer)
        if (gpio_get(BUTTON_B) == 0) { // Botão B pressionado
            vTaskDelay(pdMS_TO_TICKS(50));  // Debounce
            if (gpio_get(BUTTON_B) == 0) { // Botão B ainda pressionado
                if (!buzzer_paused) { // Buzzer ainda bipando
                    vTaskSuspend(handle_buzzer_task); // Pausa buzzer
                    buzzer_paused = true;
                } else {
                    vTaskResume(handle_buzzer_task); // Retoma buzzer
                    buzzer_paused = false;
                }
              }
            }

          vTaskDelay(pdMS_TO_TICKS(100));  // Taxa de polling dos botões
        }
      }

// ----- Setup inicial -----
void setup_gpios() {
    // LED RGB
    gpio_init(GREEN_LED); // Inicializa GPIO do LED verde
    gpio_set_dir(GREEN_LED, GPIO_OUT);  // Define como saída
    gpio_put(GREEN_LED, 0); // Começa desligado

    gpio_init(BLUE_LED); // Inicializa GPIO do LED azul
    gpio_set_dir(BLUE_LED, GPIO_OUT); // Define como saída
    gpio_put(BLUE_LED, 0); // Começa desligado

    gpio_init(RED_LED); // Inicializa GPIO do LED vermelho
    gpio_set_dir(RED_LED, GPIO_OUT); // Define como saída 
    gpio_put(RED_LED, 0); // Começa desligado

    // Botões com pull-up
    gpio_init(BUTTON_A); // Inicializa GPIO do Botão A
    gpio_set_dir(BUTTON_A, GPIO_IN); // Define como entrada
    gpio_pull_up(BUTTON_A); // Define com pull-up interno

    gpio_init(BUTTON_B); // Inicializa GPIO do Botão B
    gpio_set_dir(BUTTON_B, GPIO_IN); // Define como entrada
    gpio_pull_up(BUTTON_B); // Define com pull-up interno

    // Configuração do PWM para o buzzer
    gpio_set_function(BUZZER, GPIO_FUNC_PWM); // Define função PWM no pino
    uint slice_num_buzzer = pwm_gpio_to_slice_num(BUZZER); // Obtém o número do slice PWM
    uint32_t clock = clock_get_hz(clk_sys); // Frequência do clock do sistema
    uint32_t wrap = clock / BUZZER_FREQUENCY - 1; // Valor de wrap (para controlar frequência)
    pwm_config config = pwm_get_default_config(); // Obtém configuração padrão
    pwm_config_set_wrap(&config, wrap); // Define o wrap calculado
    pwm_init(slice_num_buzzer, &config, true); // Inicializa o PWM com config
    pwm_set_gpio_level(BUZZER, 0); // Inicia desligado (nível 0)
}

int main() {

    stdio_init_all(); // Inicializa UART padrão (opcional para debug)

    setup_gpios(); // Configura GPIOs e PWM

    // Criação das tarefas com suas prioridades e handles
    xTaskCreate(led_rgb_task, "LED_RGB", 256, NULL, 1, &handle_led_task);
    xTaskCreate(buzzer_task, "BUZZER", 256, NULL, 1, &handle_buzzer_task);
    xTaskCreate(buttons_task, "BUTTONS", 256, NULL, 1, NULL);

    vTaskStartScheduler(); // Inicia o agendador do FreeRTOS

    while (1) {} // Loop infinito (não deve ser alcançado)
    return 0;
}