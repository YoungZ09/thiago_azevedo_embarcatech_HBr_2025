// Diretivas de proteção contra múltiplas inclusões, garantindo que o conteúdo deste cabeçalho seja incluído apenas uma vez durante a compilação

// Verifica se a macro FUNCAO_TEMP_H já foi definida
#ifndef FUNCAO_TEMP_H 
// Define a macro FUNCAO_TEMP_H para evitar múltiplas inclusões
#define FUNCAO_TEMP_H

#include "pico/stdlib.h" // Inclui a biblioteca padrão do SDK da Raspberry Pi Pico

float adc_to_celsius(uint16_t); // Declara a função `adc_to_celsius`, que converte um valor de ADC (12 bits) em uma temperatura em graus Celsius

#endif // Fim da diretiva de proteção contra múltiplas inclusões