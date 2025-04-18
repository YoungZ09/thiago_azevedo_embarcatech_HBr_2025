#include <stdio.h> // Biblioteca para funções de entrada e saída de dados (printf ou scanf por exemplo)
#include <string.h> // Biblioteca para manipulação de strings
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/gpio.h" // Biblioteca para controle de GPIO
#include "hardware/i2c.h" // Biblioteca para comunicação I2C
#include "pico/time.h" // Biblioteca para manipulação de tempo
#include "inc/ssd1306.h" // Biblioteca para controle do display OLED

#define BUTTON_A_PIN 5 // Pino do botão A
#define BUTTON_B_PIN 6 // Pino do botão B
#define OLED_SDA 14 // Pino SDA do display OLED
#define OLED_SCL 15 // Pino SCL do display OLED
#define DEBOUNCE_DELAY_MS 60 // Tempo de debounce de 60 milissegundos

int countdown = 0; // Contador de tempo regressivo
volatile int clicks_b = 0; // Contador de pressionamentos do botão B (volátil para acesso seguro em interrupções)
volatile bool restart_countdown = false; // Flag para indicar que o botão A foi pressionado (volátil para acesso seguro em interrupções)
volatile bool click_b_enable = false; // Flag para indicar que os cliques no botão B estão disponíveis, pois a contagem do tempo está acontecendo (volátil para acesso seguro em interrupções)
volatile uint64_t last_press_time_a; // Variável para armazenar o último tempo de pressionamento do botão A (volátil para acesso seguro em interrupções)
volatile uint64_t last_press_time_b; // Variável para armazenar o último tempo de pressionamento do botão B (volátil para acesso seguro em interrupções)

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
    render_on_display(ssd, &frame_area); // Atualiza o display   
}

// Função para configurar o display OLED
void config_display_oled()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000); // Inicializa a comunicação I2C no barramento "i2c1" com a frequência definida para o display OLED
    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C); // Configura o pino de dados (SDA) do display OLED para funcionar como I2C
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C); // Configura o pino de clock (SCL) do display OLED para funcionar como I2C
    gpio_pull_up(OLED_SDA); // Habilita um pull-up interno no pino SDA, garantindo níveis lógicos corretos na comunicação I2C
    gpio_pull_up(OLED_SCL); // Habilita um pull-up interno no pino SCL, garantindo estabilidade no sinal de clock do I2C

    ssd1306_init(); // Inicializa o driver do display OLED SSD1306, preparando-o para receber comandos e exibir informações

    frame_area.end_column = ssd1306_width - 1; // Define o fim da área de renderização na última coluna do display (largura máxima do SSD1306)
    frame_area.end_page = ssd1306_n_pages - 1; // Define o fim da área de renderização na última página do display (altura máxima em páginas de 8 pixels)

    calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer necessário para renderizar a área configurada do display

    clean_display_oled(); // Limpa o display OLED, garantindo que nenhuma informação residual seja exibida na inicialização
}

// Função para exibir mensagem no display OLED
// O primeiro parâmetro é um ponteiro para uma string que contém o texto a ser exibido
// O segundo parâmetro é a coordenada horizontal (posição X) onde o texto será desenhado no display
// O terceiro parâmetro é a coordenada vertical (posição Y) onde o texto será desenhado no display
// O quarto parâmetro é uma variável booleana que indica se há mais mensagens a serem renderizadas
void show_message(char *mensagem, uint8_t x, uint8_t y, bool have_more_message)
{

    // Escreve a string da variável "mensagem" no buffer do display OLED (ssd) na posição (x, y)
    ssd1306_draw_string(ssd, x, y, mensagem);
    // Verifica se existe mais mensagens a serem renderizadas no display. Se não, significa que esta é a última mensagem a ser exibida antes da atualização do display
    if (!have_more_message)
    {
        render_on_display(ssd, &frame_area); // Atualiza o display OLED com o conteúdo do buffer, tornando a mensagem visível na tela
    }
}

// Função para inserir as informações da contagem regressiva e o número de cliques do botão B no display OLED
void update_display(int count, int clicks) {
    clean_display_oled(); // Limpa o display OLED
    char count_buf[50]; // Declara um buffer para armazenar a contagem regressiva
    char clicks_buf[50]; // Declara um buffer para armazenar o número de cliques do botão B
    sprintf(count_buf, "Contador: %d", count); // Formata as informações da contagem regressiva para exibição no display OLED
    sprintf(clicks_buf, "Cliques: %d", clicks); // Formata as informações da contagem de cliques do botão B para exibição no display OLED
    show_message(count_buf, 16, 26, true); // Exibe a contagem regressiva no display OLED
    show_message(clicks_buf, 24, 45, false); // Exibe a contagem de cliques do botão B no display OLED
}

// Função de callback para interrupção do botão A e do botão B
void gpio_callback_button(uint gpio, uint32_t events)  {
    int current_time = to_ms_since_boot(get_absolute_time()); // Obtém o tempo atual em milissegundos
    if (gpio == BUTTON_A_PIN) { // Verifica se o botão A é o que foi pressionado (nível baixo)
        if (current_time - last_press_time_a > DEBOUNCE_DELAY_MS) { // Verifica se o tempo desde o último pressionamento do botão A é maior que o tempo de debounce
            restart_countdown = true; // Marca que o botão A foi pressionado
            last_press_time_a = current_time; // Atualiza o tempo do último pressionamento do botão A
        }    
    }
    else if (gpio == BUTTON_B_PIN) { // Verifica se o botão B é o que foi pressionado (nível baixo)
        if(click_b_enable && current_time - last_press_time_b > DEBOUNCE_DELAY_MS) { // Verifica se pode contar o clique do botão B e se o tempo desde o último pressionamento do botão B é maior que o tempo de debounce
            clicks_b++; // Incrementa o contador de cliques do botão B
            last_press_time_b = current_time; // Atualiza o tempo do último pressionamento do botão B
        }   
    }
}

// Função para configurar o botão A e o botão B
void config_button()
{
    gpio_init(BUTTON_A_PIN); // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_A_PIN); // Habilita um pull-up interno no pino do botão A, definindo que o nível lógico baixo indica que o botão está pressionado
    gpio_init(BUTTON_B_PIN); // Inicializa o pino do botão B
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_B_PIN); // Habilita um pull-up interno no pino do botão B, definindo que o nível lógico baixo indica que o botão está pressionado
}

// Função para inserir uma instrução na tela inicial
void start_screen() {
    clean_display_oled(); // Limpa o display OLED
    show_message("Bem-Vindo", 26, 20, true); // Exibe a mensagem "Bem Vindo" no display OLED
    show_message("Aperte A", 28, 32, true); // Exibe a mensagem "Aperte A" no display OLED
    show_message("Para iniciar", 16, 53, false); // Exibe a mensagem "Para iniciar" no display OLED
}

int main()
{
    stdio_init_all(); // Inicializa a entrada e saída padrão do sistema, permitindo a comunicação com o terminal via USB ou UART
    config_display_oled(); // Chama a função que inicializa a comunicação I2C e configura o display OLED para exibir mensagens
    config_button(); // Chama a função que configura os botões A e B de entrada com um pull-up interno

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_button); // Habilita interrupção para o botão A e define a função de callback

    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true); // Habilita interrupção para o botão B (callback já definido)

    start_screen(); // Exibe tela inicial com instruções

    while (true) {

        if(restart_countdown) { // Se o botão A tiver sido pressionado
            restart_countdown = false; // A flag que indica que o botão A foi pressionado reseta
            countdown = 9; // A contagem regressiva reinicia a partir de 9
            clicks_b = 0; // A contagem dos cliques do botão B é zerada
            click_b_enable = true; // Habilita a contagem de cliques do botão B durante a contagem regressiva

            while (countdown > 0 && !restart_countdown) { // Enquanto a contagem regressiva é maior que 0 e o botão A não é pressionado
                update_display(countdown, clicks_b); // Atualiza os valores da contagem regressiva e do número de cliques do botão B no display
                sleep_ms(1000); // Aguarda 1 segundo para atualizar os valores novamente
                countdown--; // Decrementa o valor da contagem regressiva
            }
            click_b_enable = false; // Desabilita a contagem de cliques após fim da contagem regressiva
            update_display(countdown, clicks_b); // Atualiza o display com os valores finais da contagem regressiva e da contagem de cliques do botão B
        }  
    }
}
