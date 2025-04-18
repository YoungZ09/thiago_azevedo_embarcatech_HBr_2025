#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define ADC_CHANNEL_X 1
#define ADC_CHANNEL_Y 0
#define OLED_SDA 14
#define OLED_SCL 15

uint8_t ssd[ssd1306_buffer_length];

struct render_area frame_area = { 
    start_column : 0,
    end_column : 0,
    start_page : 0,
    end_page : 0
};

void clean_display_oled()
{
    memset(ssd, 0, ssd1306_buffer_length); 
    render_on_display(ssd, &frame_area);   
}

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

void message_display( uint x_axis, uint y_axis) {
    clean_display_oled();
    char x_value[50];
    char y_value[50];
    sprintf(x_value, "Eixo X: %d", x_axis);
    sprintf(y_value, "Eixo Y: %d", y_axis);
    show_message("Valores do", 24, 6, true);
    show_message("Joystick:", 32, 22, true);
    show_message(x_value, 16, 32, true);
    show_message(y_value, 16, 49, false);
}

int main()
{
    stdio_init_all();
    config_display_oled();
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    while (true) {
        
        adc_select_input(ADC_CHANNEL_X);
        uint x_val = adc_read();

        adc_select_input(ADC_CHANNEL_Y);
        uint y_val = adc_read();

        message_display(x_val, y_val);

        sleep_ms(100);
    }
}
