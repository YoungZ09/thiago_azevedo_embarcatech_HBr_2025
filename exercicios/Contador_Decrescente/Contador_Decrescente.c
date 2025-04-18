#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/time.h"
#include "inc/ssd1306.h"

#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define OLED_SDA 14
#define OLED_SCL 15
#define DEBOUNCE_DELAY_MS 60

int countdown = 0;
volatile int clicks_b = 0;
volatile bool restart_countdown = false;
volatile bool click_b_enable = false;
volatile uint64_t last_press_time_a;
volatile uint64_t last_press_time_b;

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

void update_display(int count, int clicks) {
    clean_display_oled();
    char count_buf[50];
    char clicks_buf[50];
    sprintf(count_buf, "Contador: %d", count);
    sprintf(clicks_buf, "Cliques: %d", clicks);
    show_message(count_buf, 16, 26, true);
    show_message(clicks_buf, 24, 45, false);
}

void gpio_callback_button(uint gpio, uint32_t events)  {
    int current_time = to_ms_since_boot(get_absolute_time());
    if (gpio == BUTTON_A_PIN) {
        if (current_time - last_press_time_a > DEBOUNCE_DELAY_MS) {
            restart_countdown = true;
            last_press_time_a = current_time;
        }    
    }
    else if (gpio == BUTTON_B_PIN) {
        if(click_b_enable && current_time - last_press_time_b > DEBOUNCE_DELAY_MS){
            clicks_b++;
            last_press_time_b = current_time;
        }   
    }
}

void config_button()
{
    gpio_init(BUTTON_A_PIN);             // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_A_PIN);          // Habilita um pull-up interno no pino do botão A, definindo que o nível lógico baixo indica que o botão está pressionado
    gpio_init(BUTTON_B_PIN);             // Inicializa o pino do botão B
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_B_PIN);          // Habilita um pull-up interno no pino do botão B, definindo que o nível lógico baixo indica que o botão está pressionado
}

void start_screen() {
    clean_display_oled();
    char welcome_buf[50];
    char start_buf[50];
    sprintf(welcome_buf, "Bem-Vindo");
    sprintf(start_buf, "Aperte A para iniciar");
    show_message("Bem-Vindo", 26, 20, true);
    show_message("Aperte A", 28, 32, true);
    show_message("Para iniciar", 16, 53, false);
}

int main()
{
    stdio_init_all();
    config_display_oled();
    config_button();
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_button);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);
    start_screen();

    while (true) {
        if(restart_countdown) {
            restart_countdown = false;
            countdown = 9;
            clicks_b = 0;
            click_b_enable = true;

            while (countdown > 0 && !restart_countdown) {
                update_display(countdown, clicks_b);
                sleep_ms(1000);
                countdown--;
            }
            click_b_enable = false;
            update_display(countdown, clicks_b);
        }  
    }
}
