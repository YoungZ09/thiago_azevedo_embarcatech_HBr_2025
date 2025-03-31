#include <stdio.h>              // Biblioteca para funções de entrada e saída de dados (printf ou scanf por exemplo)
#include <string.h>             // Biblioteca para manipulação de strings
#include "pico/stdlib.h"        // Biblioteca padrão do Raspberry Pi Pico
#include "inc/ssd1306.h"        // Biblioteca para controle do display OLED
#include "hardware/i2c.h"       // Biblioteca para comunicação I2C
#include "hardware/clocks.h"    // Biblioteca para manipulação de clocks
#include "hardware/pwm.h"       // Biblioteca para manipulação de PWM
#include "hardware/rtc.h"       // Biblioteca para manipulação do RTC (Relógio de Tempo Real)
#include "pico/util/datetime.h" // Biblioteca para manipulação de datas e horas

// Definição dos pinos utilizados
#define BUTTON_A_PIN 5        // Pino do botão A
#define RED_LED 13            // Pino da luz vermelha do LED RGB
#define BUZZER_A_PIN 21       // Pino do buzzer A
#define OLED_SDA 14           // Pino SDA do display OLED
#define OLED_SCL 15           // Pino SCL do display OLED
#define BUZZER_FREQUENCY 5000 // Frequência do buzzer A em Hz

uint8_t ssd[ssd1306_buffer_length]; // Buffer global para a configuração e manipulação do display OLED

struct render_area frame_area = { // Estrutura global para a configuração da área de renderização do display OLED
    start_column : 0,
    end_column : 0,
    start_page : 0,
    end_page : 0
};

static volatile bool fired = false; // Variável global que serve para indicar se o alarme disparou ou não

// Variáveis globais para configuração do tempo do alarme. Nesse caso, o alarme vai disparar de 30s em 30s
// É possível alterar os valores dessas 3 variáveis para que o intervalo de disparo do alarme seja maior
volatile uint8_t intervalo_s = 30; // Variável global para a configuração do alarme envolvendo o RTC. Usada para configurar os segundos do alarme
volatile uint8_t intervalo_m = 0;  // Variável global para a configuração do alarme envolvendo o RTC. Usada para configurar os minutos do alarme
volatile uint8_t intervalo_h = 0;  // Variável global para a configuração do alarme envolvendo o RTC. Usada para configurar as horas do alarme

datetime_t start_datetime = { // Variável global para a configuração do RTC. Configura uma data e horário inicial para o RTC
    .year = 2025,             // Define o ano do RTC para 2025. (Definir o ano atual nessa variável)
    .month = 02,              // Define o mês do RTC para fevereiro (Definir o mês atual nessa variável)
    .day = 21,                // Define o dia do RTC para 21 (Definir o dia atual do mês nessa variável)
    .dotw = 5,                // Dia da semana (0 = domingo) (Definir o dia atual da semana nessa variável)
    .hour = 15,               // Define as horas do RTC para 15h (Definir as horas atuais nessa variável)
    .min = 00,                // Define os minutos do RTC para 00m (Definir os minutos atuais nessa variável)
    .sec = 00};               // Define os segundos do RTC para 00s (Definir os segundos atuais nessa variável)

// Função para limpar o display OLED
void clean_display_oled()
{
    memset(ssd, 0, ssd1306_buffer_length); // Zera o buffer do display
    render_on_display(ssd, &frame_area);   // Atualiza o display
}

// Callback executado quando o alarme é disparado
static void alarm_callback()
{
    datetime_t next_alarm = {0};   // Declara uma variável do tipo estrutura e inicializa ela vazia
    rtc_get_datetime(&next_alarm); // Obtém a data e hora atuais (referentes ao alarme que disparou) e insere essas informações na variável declarada acima

    // Ajusta os segundos do próximo alarme
    next_alarm.sec += intervalo_s;

    if (next_alarm.sec >= 60) // Se o valor de segundos configurado para o próximo alarme for mais que 60s
    {
        next_alarm.sec -= 60; // Tira 60s da variável de segundos
        next_alarm.min += 1;  // Adiciona 1min na variável de minutos
    }

    // Ajusta os minutos do próximo alarme
    next_alarm.min += intervalo_m;

    if (next_alarm.min >= 60) // Se o valor de minutos configurado para o próximo alarme for mais que 60min
    {
        next_alarm.min -= 60; // Tira 60min da variável de minutos
        next_alarm.hour += 1; // Adiciona 1h na variável de horas
    }

    // Ajusta as horas do próximo alarme
    next_alarm.hour += intervalo_h;

    if (next_alarm.hour >= 24) // Se o valor de horas configurado para o próximo alarme for mais que 24h
    {
        next_alarm.hour -= 24; // Tira 24h da variável de horas
        next_alarm.day += 1;   // Adiciona 1 dia na variável de dias
    }

    rtc_set_alarm(&next_alarm, &alarm_callback); // Configura o próximo alarme que chamará a função de callback quando disparar

    clean_display_oled(); // Limpa o display
    fired = true;         // Marca que o alarme foi disparado
}

// Função para configurar o LED RGB (só a luz vermelha)
void config_led()
{
    gpio_init(RED_LED);              // Inicializa o pino do LED
    gpio_set_dir(RED_LED, GPIO_OUT); // Define o pino como saída
    gpio_put(RED_LED, 0);            // Inicia o LED desligado
}

// Função para configurar o buzzer A
void config_buzzer()
{
    gpio_init(BUZZER_A_PIN);                        // Inicializa o pino do buzzer A
    gpio_set_dir(BUZZER_A_PIN, GPIO_OUT);           // Define o pino do buzzer A como saída
    gpio_set_function(BUZZER_A_PIN, GPIO_FUNC_PWM); // Configura o pino para operar no modo PWM

    uint slice_num = pwm_gpio_to_slice_num(BUZZER_A_PIN); // Obtém o número do slice PWM associado ao pino do buzzer A

    pwm_config config = pwm_get_default_config();                                      // Obtém a configuração padrão do PWM
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Configura a divisão do clock para gerar a frequência do PWM correta para o buzzer A
    pwm_init(slice_num, &config, true);                                                // Inicializa o PWM com a configuração definida e o ativa

    pwm_set_gpio_level(BUZZER_A_PIN, 0); // Garante que o buzzer A inicie desligado (duty cycle = 0)
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

// Função para configurar o botão A
void config_botao()
{
    gpio_init(BUTTON_A_PIN);             // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(BUTTON_A_PIN);          // Habilita um pull-up interno no pino do botão A, definindo que o nível lógico baixo indica que o botão está pressionado
}

// Função para configurar o RTC
void config_rtc()
{
    rtc_init();                        // Inicializa o RTC
    rtc_set_datetime(&start_datetime); // Configura o RTC com a data e horário inicial
}

// Função para configurar e iniciar o alarme no RTC
void start_alarm()
{
    datetime_t alarm = {                                           // Declara e inicializa uma estrutura datetime_t para armazenar a data e hora do alarme
                        .year = 2025,                              // Define o ano do alarme para 2025 (Definir o ano atual nessa variável)
                        .month = 02,                               // Define o mês do alarme para fevereiro (Definir o mês atual nessa variável)
                        .day = 21,                                 // Define o dia do alarme para 21 (Definir o dia atual do mês nessa variável)
                        .dotw = 5,                                 // Dia da semana (0 = Domingo) (Definir o dia atual da semana nessa variável)
                        .hour = start_datetime.hour + intervalo_h, // Define a hora do alarme somando ao horário inicial o intervalo de horas configurado
                        .min = start_datetime.min + intervalo_m,   // Define os minutos do alarme somando ao minuto inicial o intervalo de minutos configurado
                        .sec = start_datetime.sec + intervalo_s};  // Define os segundos do alarme somando ao segundo inicial o intervalo de segundos configurado

    rtc_set_alarm(&alarm, &alarm_callback); // Configura o RTC para disparar o alarme na data e hora definidas e chama a função `alarm_callback` quando o alarme for acionado
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

// Função para ativar o LED vermelho e o buzzer A por um tempo determinado
void turn_on_led_buzzer(uint16_t duration_ms)
{
    gpio_put(RED_LED, 1);                                 // Liga o LED vermelho mandando um sinal de nível lógico alto para o pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_A_PIN); // Obtém o número do "slice" do PWM associado ao pino do buzzer A

    // Define o nível de saída PWM para o buzzer A
    // O valor 2048 representa 50% do ciclo de trabalho (duty cycle) em uma escala de 0 a 4096 (12 bits)
    pwm_set_gpio_level(BUZZER_A_PIN, 2048);

    // Aguarda o tempo especificado por "duration_ms", mantendo o LED e o buzzer A ativados
    sleep_ms(duration_ms);

    pwm_set_gpio_level(BUZZER_A_PIN, 0); // Define o nível de saída do PWM como 0, interrompendo o sinal para o buzzer A
    gpio_put(RED_LED, 0);                // Desliga o LED vermelho, enviando um sinal de nível lógico baixo para o pino

    sleep_ms(100); // Aguarda 100 ms antes de retornar ao código principal
}

// Função principal
int main()
{
    // Define os valores iniciais do intervalo do alarme
    intervalo_s = 30; // O alarme tocará a cada 30 segundos
    intervalo_m = 0;  // Nenhum minuto extra será adicionado
    intervalo_h = 0;  // Nenhuma hora extra será adicionada

    stdio_init_all();      // Inicializa a entrada e saída padrão do sistema, permitindo a comunicação com o terminal via USB ou UART.
    config_led();          // Chama a função que configura o LED como saída e garante que ele inicie apagado
    config_buzzer();       // Chama a função que configura o buzzer A no modo PWM, permitindo o controle da frequência do som
    config_display_oled(); // Chama a função que inicializa a comunicação I2C e configura o display OLED para exibir mensagens
    config_botao();        // Chama a função que configura o botão A de entrada com um pull-up interno
    config_rtc();          // Chama a função que inicializa o RTC (Relógio de Tempo Real) e define a data e hora inicial do sistema
    start_alarm();         // Chama a função que configura o primeiro alarme baseado na data e hora atual mais o intervalo definido

    while (true)
    {
        if (fired) // Verifica se o alarme foi disparado
        {
            turn_on_led_buzzer(500);                      // Liga o LED e aciona o buzzer A por 500ms para alertar o usuário sobre o horário do medicamento
            show_message("Hora de tomar o", 2, 25, true); // Exibe a mensagem "Hora de tomar o" no display OLED
            show_message("Remedio", 35, 40, false);       // Exibe a mensagem "Remedio" no display OLED
            if (gpio_get(BUTTON_A_PIN) == 0)              // Verifica se o botão foi pressionado, considerando que ele está configurado como pull-up
            {
                char intervalo_prox_alarme[256];                                                            // Declara um buffer para armazenar o tempo restante para o próximo alarme disparar
                fired = false;                                                                              // Reseta essa variável pois o alarme foi reconhecido
                clean_display_oled();                                                                       // Limpa o display OLED
                sprintf(intervalo_prox_alarme, "%02dh %02dm %02ds", intervalo_h, intervalo_m, intervalo_s); // Formata uma string indicando em quanto tempo o próximo alarme será ativado
                show_message("Proximo alarme", 8, 15, true);                                                // Exibe a mensagem "Proximo alarme" no display OLED
                show_message("Daqui:", 43, 25, true);                                                       // Exibe a mensagem "Daqui:" no display OLED
                show_message(intervalo_prox_alarme, 20, 40, false);                                         // Exibe o tempo restante para o próximo alarme no display OLED através do buffer "intervalo_prox_alarme"
                sleep_ms(3000);                                                                             // Aguarda 3 segundos para que o usuário veja a mensagem no display
                clean_display_oled();                                                                       // Limpa o display OLED novamente antes de voltar ao loop principal
            }
        }
        else // Se o alarme não disparou
        {
            datetime_t t = {0};                                          // Declara uma estrutura chamada "t" que inicializa vazia
            rtc_get_datetime(&t);                                        // Preenche essa estrutura com a data e hora atuais do RTC
            char date_buf[256];                                          // Declara um buffer para armazenar a data do RTC como string
            char time_buf[256];                                          // Declara um buffer para armazenar o horário do RTC como string
            sprintf(date_buf, "%02d/%02d/%04d", t.day, t.month, t.year); // Formata as informações da data para exibição no display OLED
            sprintf(time_buf, "%02d:%02d:%02d", t.hour, t.min, t.sec);   // Formata as informações do horário para exibição no display OLED
            show_message("Data:", 45, 10, true);                         // Exibe a mensagem "Data:" no display OLED
            show_message(date_buf, 22, 20, true);                        // Exibe a data no display OLED
            show_message("Horario:", 32, 36, true);                      // Exibe a mensagem "Horario:" no display OLED
            show_message(time_buf, 27, 40, false);                       // Exibe o horário no display OLED
            sleep_ms(1000);                                              // Aguarda 1 segundo antes de atualizar a exibição do relógio, evitando mudanças muito rápidas
        }
        sleep_ms(10); // Aguarda 10ms antes de continuar o loop, garantindo que o processador não fique sobrecarregado com execuções desnecessárias
    }
}
