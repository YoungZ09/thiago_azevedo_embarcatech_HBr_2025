#include "unity.h" // Inclui a biblioteca Unity, usada para testes unitários em C. Fornece macros como TEST_ASSERT_FLOAT_WITHIN, UNITY_BEGIN, RUN_TEST etc
#include "src/funcao_temp.h" // Inclui o cabeçalho da função que será testada: adc_to_celsius()


void setUp() { // Função chamada automaticamente antes de cada teste

}

void tearDown() { // Função chamada automaticamente após cada teste

}


void test_adc_to_celsius_known_value() { // Função de teste unitário. O Unity executará essa função e verificará se a saída da função testada está dentro do esperado

    uint16_t adc_val = 876; // Valor simulado de ADC correspondente a aproximadamente 0.706 V, que deve resultar em 27 °C
    float expected_temp = 27.0f; // Temperatura esperada para esse valor de ADC (baseada na documentação da Pico)
    float error_margin = 0.5f; // Margem de erro aceitável para o teste, considerando pequenas variações de cálculo
    float actual = adc_to_celsius(adc_val); // Chama a função a ser testada com o valor conhecido e armazena o resultado

    TEST_ASSERT_FLOAT_WITHIN(error_margin, expected_temp, actual); // Verifica se o valor retornado (`actual`) está dentro da margem de erro de 0.5 em relação a 27.0

}

int main()
{
    stdio_init_all(); // Inicializa os canais padrão de entrada/saída, necessário na Raspberry Pi Pico para uso de printf, especialmente em comunicação via UART
    printf("Iniciando teste\n"); // Mensagem exibida ao iniciar o teste (opcional, para feedback no terminal/serial)
    sleep_ms(3000); // Espera 3 segundos para garantir que a saída anterior seja exibida corretamente antes de iniciar o teste

    UNITY_BEGIN(); // Inicializa o sistema de testes do Unity
    RUN_TEST(test_adc_to_celsius_known_value); // Executa a função de teste definida acima
    sleep_ms(6000); // Espera 6 segundos após a execução do teste para garantir que o resultado seja visível antes do encerramento
    return UNITY_END(); // Finaliza o teste e retorna o número de falhas encontradas

    }
