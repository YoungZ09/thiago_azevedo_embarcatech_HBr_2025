// Bibliotecas necessárias
#include <string.h>                 // Para funções de string como strlen()
#include "pico/stdlib.h"            // Biblioteca padrão do Pico (GPIO, tempo, etc.)
#include "pico/cyw43_arch.h"        // Driver WiFi para Pico W
#include "include/wifi_conn.h"      // Funções personalizadas de conexão WiFi
#include "include/mqtt_comm.h"      // Funções personalizadas para MQTT
#include "include/xor_cipher.h"     // Funções de cifra XOR
#include <time.h>                   // Funções de tempo
#include <stdint.h>                 // Biblioteca que permite o uso de tipos inteiros com tamanho fixo
#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP

char buffer[128]; // Buffer para armazenar o valor da mensagem original junto com o valor do tempo em que ela foi publicada

uint32_t ultima_timestamp_recebida = 0; // Usando uint32_t para maior clareza

// Doc: https://www.nongnu.org/lwip/2_1_x/group__mqtt.html#gafec7e75fe6a746eef9ca411463446c81
// Exemplo simplificado de callback dos dados MQTT recebidos, ver link da documentação para mais informações
void on_message(char* topic, char* msg) {
    // 1. Parse do JSON (exemplo simplificado)
    uint32_t nova_timestamp;
    float valor;
    if (sscanf(msg, "{\"valor\":%f,\"ts\":%lu}", &valor, &nova_timestamp) != 2) {
        printf("Erro no parse da mensagem!\n");
        return;
    }

    // 2. Verificação de replay
    if (nova_timestamp > ultima_timestamp_recebida) {
        ultima_timestamp_recebida = nova_timestamp;
        printf("Nova leitura: %.2f (ts: %lu)\n", valor, nova_timestamp);
        
    } else {
        printf("Replay detectado (ts: %lu <= %lu)\n", 
               nova_timestamp, ultima_timestamp_recebida);
    }
}

int main() {
    // Inicializa todas as interfaces de I/O padrão (USB serial, etc.)
    stdio_init_all();
    
    // Conecta à rede WiFi - Etapa 1
    // Parâmetros: Nome da rede (SSID) e senha
    connect_to_wifi("Nome da Rede", "Senha da Rede");
    // Testei a conectividade com o Wi-fi usando o nome e senha da rede privada da minha residência e deu certo

    // Configura o cliente MQTT - Etapa 2 
    // Parâmetros: ID do cliente, IP do broker, usuário, senha
    mqtt_setup("bitdog1", "192.168.15.14", "Thiago", "senha123");

    // Mensagem original a ser enviada
    const char *mensagem = "26.5";

    // Buffer para mensagem criptografada (16 bytes) - Etapa 5
    //uint8_t criptografada[16];

    // Criptografa a mensagem usando XOR com chave 42 - Etapa 5
    //xor_encrypt((uint8_t *)mensagem, criptografada, strlen(mensagem), 42);

    // Loop principal do programa
    while (true) {
        // Publica a mensagem original (não criptografada) - Etapa 3
        //mqtt_comm_publish("escola/sala1/temperatura", mensagem, strlen(mensagem));
        
        // Publica a mensagem criptografada - Etapa 5
        //mqtt_comm_publish("escola/sala1/temperatura", criptografada, strlen(mensagem));
        
        // Buffer contendo informações como o valor da mensagem original e o timestamp referente a esta mensagem - Etapa 6
        sprintf(buffer, "{\"valor\":26.5,\"ts\":%lu}", time(NULL));

        // Publica a mensagem original com o valor de timestamp - Etapa 6
        mqtt_comm_publish("escola/sala1/temperatura", buffer, strlen(buffer));

        // Realiza a leitura do valor presente no tópico específico do broker MQTT - Etapa 6
        //mqtt_subscribe(client, "escola/sala1/temperatura", 0, on_message, "escola/sala1/temperatura");

        // Aguarda 5 segundos antes da próxima publicação
        sleep_ms(5000);
    }
    return 0;
}

/* 
 * Comandos de terminal para testar o MQTT:
 * 
 * Inicia o broker MQTT com logs detalhados:
 * mosquitto -c mosquitto.conf -v
 * 
 * Assina o tópico de temperatura (recebe mensagens):
 * mosquitto_sub -h localhost -p 1883 -t "escola/sala1/temperatura" -u "aluno" -P "senha123"
 * 
 * Publica mensagem de teste no tópico de temperatura:
 * mosquitto_pub -h localhost -p 1883 -t "escola/sala1/temperatura" -u "aluno" -P "senha123" -m "26.6"
 */