// Bibliotecas necessárias
#include <string.h>                 // Para funções de string como strlen()
#include <time.h>                   // Funções de tempo
#include <stdint.h>                 // Biblioteca que permite o uso de tipos inteiros com tamanho fixo
#include "pico/stdlib.h"            // Biblioteca padrão do Pico (GPIO, tempo, etc.)
#include "pico/cyw43_arch.h"        // Driver WiFi para Pico W
#include "include/wifi_conn.h"      // Funções personalizadas de conexão WiFi
#include "include/mqtt_comm.h"      // Funções personalizadas para MQTT
#include "include/xor_cipher.h"     // Funções de cifra XOR

int main() {
    // Inicializa todas as interfaces de I/O padrão (USB serial, etc.)
    stdio_init_all();
    
    // Conecta à rede WiFi - Etapa 1
    // Parâmetros: Nome da rede (SSID) e senha da rede
    connect_to_wifi("Nome da Rede", "Senha da Rede");

    // Configura o cliente MQTT - Etapas 2 e 4 
    // Parâmetros: ID do cliente, IP do broker, usuário, senha
    mqtt_setup("bitdog1", "IP do Broker", "Thiago", "senha123");

    //Descomente a seguinte linha do código para usar a placa como subscriber - Etapa 6
    //mqtt_comm_subscribe("escola/sala1/temperatura");

    // Loop principal do programa
    while (true) {
        // Cria mensagem no formato JSON com timestamp - Etapa 6
        char mensagem[128]; // Buffer para armazenar o valor da mensagem original junto com o valor do tempo em que ela foi publicada
        snprintf(mensagem, sizeof(mensagem), "{\"valor\":26.5,\"ts\":%lu}", time(NULL));

        size_t len = strlen(mensagem); // Tamanho da mensagem em bytes

        // Criptografa a mensagem usando XOR com chave 42 - Etapa 5
        uint8_t criptografada[128]; // Buffer para mensagem criptografada com timestamp 
        xor_encrypt((uint8_t *)mensagem, criptografada, len, 42);

        // Publica a mensagem sem criptografia com timestamp - Etapas 3 e 6
        //mqtt_comm_publish("escola/sala1/temperatura", (uint8_t *)mensagem, len);

        // Publica a mensagem criptografada com timestamp - Etapas 5 e 6
        mqtt_comm_publish("escola/sala1/temperatura", criptografada, len);

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