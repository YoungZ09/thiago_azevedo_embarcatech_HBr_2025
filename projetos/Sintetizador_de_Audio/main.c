#include <stdio.h> // Biblioteca padrão do C
#include <string.h> // Biblioteca para manipulação de strings
#include "pico/stdlib.h" // Biblioteca padrão pico
#include "hardware/gpio.h" // Biblioteca de GPIOs
#include "hardware/adc.h" // Biblioteca do ADC
#include "hardware/pwm.h" // Biblioteca do PWM
#include "include/ssd1306.h" // Biblioteca para controle do display OLED
#include "hardware/i2c.h" // Biblioteca para comunicação I2C

// Definições dos pinos conforme o mapeamento
#define MIC_ADC_PIN 28          // GPIO28 = ADC2 - Microfone
#define BUTTON_RECORD 5         // Botão A
#define BUTTON_PLAY 6           // Botão B
#define BUZZER_PIN_A 21         // Buzzer A (PWM)
#define BUZZER_PIN_B 10         // Buzzer B (PWM)
#define LED_GREEN 11            // LED RGB - Verde
#define LED_RED 13              // LED RGB - Vermelho
#define OLED_SDA 14           // Pino SDA do display OLED
#define OLED_SCL 15           // Pino SCL do display OLED

// Buffer de gravação
#define SAMPLE_RATE 11025 // Define a taxa de amostragem em 11025 amostras por segundo
#define DURATION_SEC 4 // Define a duração da gravação em segundos
#define BUFFER_SIZE (SAMPLE_RATE * DURATION_SEC) // Calcula o tamanho total do buffer (número de amostras). Aqui: 11025 * 4 = 44100 amostras
uint16_t audio_buffer[BUFFER_SIZE]; // Cria um array de 44100 amostras de 16 bits para armazenar o áudio gravado
uint8_t ssd[ssd1306_buffer_length]; // Buffer global para a configuração e manipulação do display OLED

struct render_area frame_area = { // Estrutura global para a configuração da área de renderização do display OLED
    start_column : 0,
    end_column : 0,
    start_page : 0,
    end_page : 0
};

// Função para limpar o display OLED
void clean_display_oled()
{
    memset(ssd, 0, ssd1306_buffer_length); // Zera o buffer do display
    render_on_display(ssd, &frame_area);   // Atualiza o display
}

// === Inicialização do ADC para o microfone ===
void config_adc() { 
    adc_init(); // Inicializa o módulo ADC (Conversor Analógico-Digital)
    adc_gpio_init(MIC_ADC_PIN); // Configura o GPIO28 como entrada analógica (ADC)
    adc_select_input(2); // Seleciona o canal ADC2, que corresponde ao GPIO28
}

// === Inicialização dos GPIOs para botões e LEDs ===
void config_gpio() {
    // Botões

    //Inicializa o botão A como entrada com pull-up interno (fica em nível lógico alto quando não pressionado)
    gpio_init(BUTTON_RECORD);
    gpio_set_dir(BUTTON_RECORD, GPIO_IN);
    gpio_pull_up(BUTTON_RECORD);

    //Inicializa o botão B como entrada com pull-up interno (fica em nível lógico alto quando não pressionado)
    gpio_init(BUTTON_PLAY);
    gpio_set_dir(BUTTON_PLAY, GPIO_IN);
    gpio_pull_up(BUTTON_PLAY);

    // LEDs

    //Inicializa o LED verde como saída e o mantém desligado (nível lógico 0)
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);

    //Inicializa o LED vermelho como saída e o mantém desligado (nível lógico 0)
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);
}

// === Inicialização de PWM para buzzers ===
void config_pwm(uint buzzer_pin) { // Função recebe o número do GPIO do buzzer a configurar
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM); // Configura o pino para a função PWM (saída de onda quadrada)
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin); // Obtém o "slice" do PWM correspondente ao pino. Cada pino de PWM pertence a um dos 8 "slices"
    pwm_set_wrap(slice_num, 4095); // Define o valor máximo do contador PWM (4095), o que configura a resolução para 12 bits
    pwm_set_enabled(slice_num, true); // Habilita o PWM no slice correspondente
}

// Função para configurar o display OLED
void config_display_oled()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);   // Inicializa a comunicação I2C no barramento "i2c1" com a frequência definida para o display OLED
    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C); // Configura o pino de dados (SDA) do display OLED para funcionar como I2C
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C); // Configura o pino de clock (SCL) do display OLED para funcionar como I2C
    gpio_pull_up(OLED_SDA);                     // Habilita um pull-up interno no pino SDA, garantindo níveis lógicos corretos na comunicação I2C
    gpio_pull_up(OLED_SCL);                     // Habilita um pull-up interno no pino SCL, garantindo estabilidade no sinal de clock do I2C

    ssd1306_init(); // Inicializa o driver do display OLED SSD1306, preparando-o para receber comandos e exibir informações

    frame_area.end_column = ssd1306_width - 1; // Define o fim da área de renderização na última coluna do display (largura máxima do SSD1306)
    frame_area.end_page = ssd1306_n_pages - 1; // Define o fim da área de renderização na última página do display (altura máxima em páginas de 8 pixels)

    calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer necessário para renderizar a área configurada do display

    clean_display_oled(); // Limpa o display OLED, garantindo que nenhuma informação residual seja exibida na inicialização
}

void display_waveform() { // Função que desenha a forma de onda do áudio capturado no display OLED

    int step = BUFFER_SIZE / ssd1306_width; // Define um passo de leitura proporcional entre o tamanho do audio_buffer (ex: 44100 amostras) e a largura do display OLED (ssd1306_width, que é 128 pixels)
    // Isso permite desenhar 1 ponto por coluna, selecionando 1 amostra a cada "step" posições do buffer

    for (int x = 0; x < ssd1306_width; x++) { // Laço que percorre as colunas do display (de 0 a 127) para desenhar a forma de onda
        int index = x * step; // Calcula o índice no "audio_buffer" correspondente à coluna x atual, aplicando o passo calculado anteriormente
        if (index >= BUFFER_SIZE) break; // Garante que o índice não ultrapasse o tamanho do buffer, evitando acesso fora dos limites da memória

        int value = audio_buffer[index] * (ssd1306_height - 1) / 4095; // Reduz o valor da amostra (0–4095) para caber na altura do display OLED, que é de 64 pixels (0–63), mantendo proporção

        int y_start = ssd1306_height - value; // Calcula a coordenada y inicial para desenhar a linha. Como o OLED tem o eixo Y invertido (0 = topo), invertemos o valor para que sons mais fortes fiquem visíveis como picos voltados para cima

        for (int y = y_start; y < ssd1306_height; y++) { // Inicia um laço vertical que vai da posição "y_start" até o fim da tela (63), desenhando uma coluna de pixels proporcional à amplitude da amostra 
            ssd1306_set_pixel(ssd, x, y, true); // Usa a função da biblioteca "ssd1306" para acender um pixel no ponto (x, y) no framebuffer local "ssd[]". O terceiro argumento "true" indica que o pixel será aceso (cor branca)
        }
    }

    render_on_display(ssd, &frame_area); // Envia todo o conteúdo do framebuffer "ssd[]" para o display OLED, utilizando a área de renderização "frame_area" previamente configurada. Isso atualiza visualmente o display com a forma de onda

}

void record_audio() { // Função responsável por capturar o áudio do microfone e armazená-lo no buffer

    printf("Gravando áudio...\n"); // Imprime uma mensagem no console indicando que a gravação foi iniciada

    gpio_put(LED_RED, 1);  // Acende o LED vermelho para indicar gravação

    for (int i = 0; i < BUFFER_SIZE; i++) { // laço "for" que percorre todo o "audio_buffer", cuja capacidade foi definida com base na taxa de amostragem (SAMPLE_RATE) e duração da gravação (DURATION_SEC)
        
        uint16_t sample = adc_read(); // Lê uma amostra de 12 bits do ADC (valor entre 0 e 4095) correspondente ao sinal do microfone conectado ao GPIO28 (canal ADC2)

        audio_buffer[i] = sample; // Armazena o valor lido do ADC na posição i do buffer de áudio

        sleep_us(90.7); // Aguarda aproximadamente 90,7 microssegundos antes de capturar a próxima amostra, para atingir uma taxa de amostragem de cerca de 11.025Hz (1 segundo / 11025 amostras)
    }

    gpio_put(LED_RED, 0);  // Desliga o LED vermelho indicando que a gravação terminou

    printf("Gravação finalizada!\n"); // Imprime no console que a gravação foi concluída

    display_waveform(); // Chama a função para representar a onda do sinal de áudio captado pelo microfone no display OLED
}

void play_audio() { // Função responsável por reproduzir o áudio previamente gravado, usando PWM nos buzzers

    printf("Reproduzindo áudio...\n"); // Imprime uma mensagem no terminal indicando o início da reprodução do áudio

    gpio_put(LED_GREEN, 1);  // Acende LED verde durante reprodução

    for (int i = 0; i < BUFFER_SIZE; i++) { // laço "for" que percorre todas as amostras armazenadas no "audio_buffer"

        uint16_t duty_cycle = audio_buffer[i]; // Captura o valor da amostra do buffer e usa esse valor diretamente como duty cycle para o PWM. Isso funciona porque tanto o ADC quanto o PWM estão usando 12 bits (mesmo intervalo de 0 a 4095) de resolução

        // Define o nível de PWM (duty cycle) para os dois buzzers, fazendo com que vibrem de acordo com a amplitude da amostra, reproduzindo o som
        pwm_set_gpio_level(BUZZER_PIN_A, duty_cycle);
        pwm_set_gpio_level(BUZZER_PIN_B, duty_cycle);

        sleep_us(90.7); // Espera 90.7us para manter taxa de 11025Hz
    }

    // Após reprodução, desliga os buzzers (duty cycle = 0)
    pwm_set_gpio_level(BUZZER_PIN_A, 0);
    pwm_set_gpio_level(BUZZER_PIN_B, 0);

    gpio_put(LED_GREEN, 0);  // Apaga LED verde indicando que a reprodução terminou

    printf("Reprodução finalizada!\n"); // Exibe uma mensagem no terminal informando que a reprodução foi concluída
}

int main() {
  stdio_init_all(); // Inicializa a comunicação padrão (como printf() via USB para depuração)
  config_adc(); // Chama a função de configuração de ADC para o microfone
  config_gpio(); // Chama a função de configuração de GPIO para os botões e LEDS
  config_pwm(BUZZER_PIN_A); // Chama a função de configuração de PWM para o buzzer A
  config_pwm(BUZZER_PIN_B); // Chama a função de configuração de PWM para o buzzer B
  config_display_oled(); // Chama a função de configuração do display OLED

  printf("Sistema iniciado!\n"); // Mensagem que será exibida no terminal quando o sistema for iniciado

  while (true) {

    // Verifica se o botão A (gravação) foi pressionado
        if (!gpio_get(BUTTON_RECORD)) {  // Botão com pull-up, pressionado = 0
            sleep_ms(50);  // Pausa o programa por 50 milissegundos para evitar leituras falsas causadas pelo efeito de bouncing
            if (!gpio_get(BUTTON_RECORD)) { // Verifica novamente se o botão ainda está pressionado após o tempo de debounce
                record_audio(); // Chama a função "record_audio()" para iniciar a gravação de áudio com o microfone e armazenar os dados no buffer
            }
        }

        // Verifica se o botão B (reprodução) foi pressionado
        if (!gpio_get(BUTTON_PLAY)) { // Botão com pull-up, pressionado = 0
            sleep_ms(50);  // Pausa por 50 milissegundos para realizar o debounce
            if (!gpio_get(BUTTON_PLAY)) { // Confirma se o botão ainda está pressionado após o tempo de debounce
                play_audio(); // Chama a função "play_audio()" para reproduzir o conteúdo presente e armazenado no "audio_buffer" pelos dois buzzers
            }
        }
        sleep_ms(10); // Pausa o loop principal por 10 milissegundos antes de continuar a próxima iteração. Isso reduz o uso de CPU
    }

  return 0;
}