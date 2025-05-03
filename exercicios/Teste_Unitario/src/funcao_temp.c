#include "src/funcao_temp.h" // Inclui o arquivo de cabeçalho "funcao_temp.h", onde a função `adc_to_celsius` é declarada
// Isso garante que a assinatura da função esteja visível para outros arquivos que incluam o header

float adc_to_celsius(uint16_t adc_val) { // Define a função `adc_to_celsius`, que recebe como parâmetro o valor lido do ADC (de 0 a 4095) e retorna a temperatura correspondente em graus Celsius (tipo float)

    float voltage = adc_val * (3.3f / (1 << 12)); // Converte o valor do ADC para uma tensão em volts. `3.3f` é a tensão de referência (Vref) da Raspberry Pi Pico W. `1 << 12` equivale a 2^12 = 4096 (resolução de 12 bits)
    float temperature  = 27.0f - ((voltage - 0.706f) / 0.001721f); // Aplica a fórmula oficial da documentação da Raspberry Pi para conversão de tensão em temperatura. 0.706 V corresponde a 27 °C e 0.001721 V/°C é o coeficiente de variação da tensão com a temperatura
    return temperature; // Retorna o valor da temperatura calculada

}