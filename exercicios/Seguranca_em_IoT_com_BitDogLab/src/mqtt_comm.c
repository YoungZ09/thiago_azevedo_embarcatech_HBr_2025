#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP
#include "include/mqtt_comm.h"    // Header file com as declarações locais
#include "lwipopts.h"             // Configurações customizadas do lwIP. Esse header foi retirado do seguinte link: https://github.com/BitDogLab/BitDogLab-C/blob/main/wifi_button_and_led/lwipopts.h 
#include "include/xor_cipher.h"   // Funções de cifra XOR
#include <stdint.h>               // Biblioteca que permite o uso de tipos inteiros com tamanho fixo
#include <stdlib.h>               // Biblioteca padrão para funções utilitárias como alocação de memória
#include <string.h>               // Para funções de string como strlen()


/* Variável global estática para armazenar a instância do cliente MQTT
 * 'static' limita o escopo deste arquivo */
static mqtt_client_t *client;

static uint32_t ultima_timestamp_recebida = 0; // Armazena o último timestamp válido da última mensagem recebida para detectar e ignorar mensagens repetidas 

// Callback chamado automaticamente quando uma nova publicação MQTT é detectada em um tópico assinado
// Esta função é usada apenas para notificar que uma nova mensagem chegou
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    printf("Mensagem recebida no tópico: %s (tamanho total: %u)\n", topic, tot_len);  // Exibe no terminal o nome do tópico onde a mensagem foi publicada e o tamanho total da mensagem recebida
}

// Callback chamado automaticamente quando os dados de uma mensagem MQTT publicada são recebidos
// Esse é o ponto onde o conteúdo da mensagem é acessado e tratado (diferente do callback anterior que só detecta a publicação)
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {

    // Caso queira receber uma mensagem sem criptografia, utilize apenas as linhas 27, 29, 31, 33, 47, 48, 52 a 55 e 65 a 71
    //char mensagem[128]; // Buffer para armazenar a mensagem sem criptografia no formato de string

    //if (len >= sizeof(mensagem)) len = sizeof(mensagem) - 1; // Garante que não haverá overflow caso a mensagem seja maior que o buffer

    //memcpy(mensagem, data, len); // Copia os dados recebidos (data) para o buffer local (mensagem)

    //mensagem[len] = '\0'; // Adiciona caractere nulo ao final para garantir que a string seja válida

    // Caso queira receber uma mensagem criptografada, utilize apenas as linhas 36, 38, 41, 43, 47, 48, 58 a 61 e 65 a 71
    uint8_t decifrada[128]; // Buffer para armazenar a mensagem descriptografada

    if (len >= sizeof(decifrada)) len = sizeof(decifrada) - 1; // Garante que não haverá overflow caso a mensagem seja maior que o buffer

    // Descriptografar com XOR
    xor_encrypt(data, decifrada, len, 42);

    decifrada[len] = '\0'; // Adiciona caractere nulo ao final para garantir que a string seja válida

    // Parse do JSON da mensagem (análise da mensagem no formato JSON)
    // Declaração de variáveis para armazenar os dados extraídos do JSON
    float valor; // valor da leitura (ex: temperatura)
    uint32_t nova_ts; // timestamp da mensagem recebida

    // Faz o parse da string JSON recebida, extraindo "valor" e "ts"
    // Retorna o número de variáveis preenchidas (deve ser 2: valor e timestamp)
    //if (sscanf(mensagem, "{\"valor\":%f,\"ts\":%lu}", &valor, &nova_ts) != 2) {
        //printf("Erro no parse da mensagem no formato JSON: %s\n", mensagem); // Caso o formato da mensagem esteja incorreto, exibe a mensagem original e sai da função
        //return;
    //}

    // Faz o parse da string JSON recebida, extraindo "valor" e "ts"
    if (sscanf((char *)decifrada, "{\"valor\":%f,\"ts\":%lu}", &valor, &nova_ts) != 2) {
        printf("Erro no parse da mensagem no formato JSON: %s\n", decifrada); // Caso o formato da mensagem esteja incorreto, exibe erro
        return;
    }

    // Verificação contra replay
     // Se o timestamp for mais recente que o último armazenado, a mensagem é aceita
    if (nova_ts > ultima_timestamp_recebida) {
        ultima_timestamp_recebida = nova_ts; // Atualiza o último timestamp recebido
        printf("Nova leitura: %.2f (ts: %lu)\n", valor, nova_ts); // Exibe a nova leitura válida
        
    } else {
        printf("Replay detectado (ts: %lu <= %lu)\n", nova_ts, ultima_timestamp_recebida); // Caso o timestamp seja repetido ou antigo, considera replay e ignora
    }
}

// Função de callback chamada após uma tentativa de inscrição (subscribe) em um tópico MQTT
static void mqtt_sub_request_cb(void *arg, err_t result) {
     // Verifica se o resultado da tentativa de inscrição no tópico foi bem-sucedido
    if (result == ERR_OK) {
        printf("Inscrição no tópico feita com sucesso.\n"); // Exibe mensagem indicando que a inscrição no tópico MQTT foi realizada com sucesso
    } else {
        printf("Erro ao se inscrever no tópico: %d\n", result);  // Caso ocorra erro na inscrição, exibe o código do erro retornado
    }
}

/* Callback de conexão MQTT - chamado quando o status da conexão muda
 * Parâmetros:
 *   - client: instância do cliente MQTT
 *   - arg: argumento opcional (não usado aqui)
 *   - status: resultado da tentativa de conexão */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT com sucesso!\n");

        // Configura os callbacks do subscriber para mensagens recebidas
        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL); 

    } else {
        printf("Falha ao conectar ao broker, código: %d\n", status);
    }
}

/* Função para configurar e iniciar a conexão MQTT
 * Parâmetros:
 *   - client_id: identificador único para este cliente
 *   - broker_ip: endereço IP do broker como string (ex: "192.168.1.1")
 *   - user: nome de usuário para autenticação (pode ser NULL)
 *   - pass: senha para autenticação (pode ser NULL) */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass) {
    ip_addr_t broker_addr;  // Estrutura para armazenar o IP do broker
    
    // Converte o IP de string para formato numérico
    if (!ip4addr_aton(broker_ip, &broker_addr)) {
        printf("Erro no IP\n");
        return;
    }

    // Cria uma nova instância do cliente MQTT
    client = mqtt_client_new();
    if (client == NULL) {
        printf("Falha ao criar o cliente MQTT\n");
        return;
    }

    // Configura as informações de conexão do cliente
    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id,  // ID do cliente
        .client_user = user,     // Usuário (opcional)
        .client_pass = pass      // Senha (opcional)
    };

    // Inicia a conexão com o broker
    // Parâmetros:
    //   - client: instância do cliente
    //   - &broker_addr: endereço do broker
    //   - 1883: porta padrão MQTT
    //   - mqtt_connection_cb: callback de status
    //   - NULL: argumento opcional para o callback
    //   - &ci: informações de conexão
    mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
}

// Função para inscrever o cliente MQTT em um tópico específico para receber mensagens publicadas por outros dispositivos
void mqtt_comm_subscribe(const char *topic) {

    // Verifica se o cliente MQTT foi inicializado corretamente
    if (client == NULL) {
        printf("Cliente MQTT não inicializado\n");
        return;
    }

    // Registra os callbacks de publicação e dados recebidos
    // - mqtt_incoming_publish_cb: chamado ao receber o cabeçalho da mensagem (tópico, tamanho etc.)
    // - mqtt_incoming_data_cb: chamado ao receber o conteúdo (payload) da mensagem
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);

    // Inscreve-se no tópico especificado
    mqtt_subscribe(client, topic, 0, mqtt_sub_request_cb, NULL);
}

/* Callback de confirmação de publicação
 * Chamado quando o broker confirma recebimento da mensagem (para QoS > 0)
 * Parâmetros:
 *   - arg: argumento opcional
 *   - result: código de resultado da operação */
static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("Publicação MQTT enviada com sucesso!\n");
    } else {
        printf("Erro ao publicar via MQTT: %d\n", result);
    }
}

/* Função para publicar dados em um tópico MQTT
 * Parâmetros:
 *   - topic: nome do tópico (ex: "sensor/temperatura")
 *   - data: payload da mensagem (bytes)
 *   - len: tamanho do payload */
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len) {
    if (client == NULL) {
        printf("Cliente MQTT não inicializado\n");
        return;
    }
    
    // Envia a mensagem MQTT
    err_t status = mqtt_publish(
        client,              // Instância do cliente
        topic,               // Tópico de publicação
        data,                // Dados a serem enviados
        len,                 // Tamanho dos dados
        0,                   // QoS 0 (nenhuma confirmação)
        0,                   // Não reter mensagem
        mqtt_pub_request_cb, // Callback de confirmação
        NULL                 // Argumento para o callback
    );

    if (status != ERR_OK) {
        printf("Publicação MQTT falhou ao ser enviada: %d\n", status);
    }
}