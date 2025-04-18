#include <stdio.h> // Biblioteca padrão para entrada e saída, utilizada para printf
#include <string.h> // Biblioteca para manipulação de strings 
#include "pico/stdlib.h" // Biblioteca padrão para funções básicas do Pico, como GPIO e temporização
#include "hardware/adc.h" // Biblioteca para controle do ADC (Conversor Analógico-Digital)
#include "hardware/i2c.h" // Biblioteca para comunicação I2C
#include "inc/ssd1306.h" // Biblioteca para controle do display OLED

// Definições
#define ADC_TEMPERATURE_CHANNEL 4 // Canal ADC que corresponde ao sensor de temperatura interno
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

// Função para converter o valor lido do ADC para temperatura em graus Celsius
float adc_to_temperature(uint16_t adc_value) {
    // Constantes fornecidas no datasheet do RP2040
    const float conversion_factor = 3.3f / (1 << 12); // Conversão de 12 bits (0-4095) para 0-3.3V
    float voltage = adc_value * conversion_factor; // Converte o valor ADC para tensão
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;  // Equação fornecida para conversão
    return temperature; // Retorna o valor da temperatura em graus Celsius
}

// Função para inserir o valor da temperatura interna do RP2040 no display OLED
void message_display( float temperature) {
    clean_display_oled(); // Limpa o display OLED
    char sensor_temp[50]; // Declara um buffer para armazenar a string do valor da temperatura
    sprintf(sensor_temp, "%.2f°C", temperature); // Converte o valor da temperatura para string formatada
    show_message("Temperatura", 20, 10, true); // Exibe a mensagem "Temperatura" no display OLED
    show_message("Interna da MCU", 10, 25, true); // Exibe a mensagem "Interna da MCU" no display OLED
    show_message(sensor_temp, 33, 45, false); // Exibe a temperatura no display OLED
}

int main()
{
    stdio_init_all(); // Inicializa a comunicação serial para permitir o uso de printf
    config_display_oled(); // Chama a função que inicializa a comunicação I2C e configura o display OLED para exibir mensagens
    adc_init(); // Inicializa o módulo ADC do Raspberry Pi Pico

    // Seleciona o canal 4 do ADC (sensor de temperatura interno)
    adc_set_temp_sensor_enabled(true); // Habilita o sensor de temperatura interno
    adc_select_input(ADC_TEMPERATURE_CHANNEL); // Seleciona o canal do sensor de temperatura

    while (true) {

        uint16_t adc_value = adc_read(); // Lê o valor do ADC no canal selecionado (sensor de temperatura)

        float temperature_celsius = adc_to_temperature(adc_value); // Converte o valor do ADC para temperatura em graus Celsius

        message_display(temperature_celsius); // Exibe o valor da temperatura interna do RP2040 no display OLED

        sleep_ms(1000); // Aguarda 1 segundo entre as leituras
    }
}
