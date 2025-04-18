#include <stdio.h> // Biblioteca para funções de entrada e saída de dados (printf ou scanf por exemplo)
#include <string.h> // Biblioteca para manipulação de strings
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/adc.h" // Biblioteca para manipulação de ADCs
#include "hardware/i2c.h" // Biblioteca para comunicação I2C
#include "inc/ssd1306.h" // Biblioteca para controle do display OLED

#define JOYSTICK_X_PIN 27 // Pino do Eixo X do Joystick
#define JOYSTICK_Y_PIN 26 // Pino do Eixo Y do Joystick
#define ADC_CHANNEL_X 1 // Canal do ADC para o Eixo X
#define ADC_CHANNEL_Y 0 // Canal do ADC para o Eixo Y
#define OLED_SDA 14 // Pino SDA do display OLED
#define OLED_SCL 15 // Pino SCL do display OLED

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

 // Função para inserir os valores dos Eixos X e Y do Joystick no display OLED
void message_display( uint x_axis, uint y_axis) {
    clean_display_oled(); // Limpa o display OLED
    char x_value[50]; // Declara um buffer para armazenar a string do valor do Eixo X
    char y_value[50]; // Declara um buffer para armazenar a string do valor do Eixo Y
    sprintf(x_value, "Eixo X: %d", x_axis); // Converte o valor numérico do Eixo X para string formatada
    sprintf(y_value, "Eixo Y: %d", y_axis); // Converte o valor numérico do Eixo Y para string formatada
    show_message("Valores do", 24, 6, true); // Exibe a mensagem "Valores do" no display OLED
    show_message("Joystick:", 32, 22, true); // Exibe a mensagem "Joystick" no display OLED
    show_message(x_value, 16, 32, true); // Exibe o valor do Eixo X no display OLED
    show_message(y_value, 16, 49, false); // Exibe o valor do Eixo Y no display OLED
}

int main()
{
    stdio_init_all(); // Inicializa a entrada e saída padrão do sistema, permitindo a comunicação com o terminal via USB ou UART
    config_display_oled(); // Chama a função que inicializa a comunicação I2C e configura o display OLED para exibir mensagens
    adc_init(); // Inicializa o conversor analógico-digital (ADC)
    adc_gpio_init(JOYSTICK_X_PIN); // Configura o pino GPIO 27 (Eixo X do Joystick) como entrada analógica
    adc_gpio_init(JOYSTICK_Y_PIN); // Configura o pino GPIO 26 (Eixo Y do Joystick) como entrada analógica

    while (true) {
        
        adc_select_input(ADC_CHANNEL_X); // Seleciona o canal ADC 1 (conectado ao Eixo X do Joystick)
        uint x_val = adc_read(); // Lê o valor convertido do ADC para o Eixo X (0 a 4095)

        adc_select_input(ADC_CHANNEL_Y); // Seleciona o canal ADC 0 (conectado ao Eixo Y do Joystick)
        uint y_val = adc_read(); // Lê o valor convertido do ADC para o Eixo Y (0 a 4095)

        message_display(x_val, y_val); // Exibe os valores lidos e convertidos dos Eixos X e Y no display OLED

        sleep_ms(100); // Pausa o programa por 100 milissegundos antes de ler novamente
    }
}
