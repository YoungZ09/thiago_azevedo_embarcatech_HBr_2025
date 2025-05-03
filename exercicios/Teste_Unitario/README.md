# Teste Unitário
---

## Proposta do Exercício

O ADC interno da Raspberry Pi Pico W pode ser usado para medir a temperatura através de um canal especial (sensor embutido). O valor lido do ADC (12 bits) deve ser convertido para graus Celsius usando a fórmula da documentação oficial:

![Fórmula para Conversão da Tensão do ADC em Celsius](./assets/formula_conversao.png)

Essa função converte a tensão lida do sensor de temperatura para Celsius, assumindo Vref = 3.3V e resolução de 12 bits.

### Tarefa

1. Implemente a função float adc_to_celsius(uint16_t adc_val);

2. Escreva uma função de teste unitário que verifica se a função retorna o valor correto (com margem de erro) para uma leitura simulada de ADC. Sugere-se o uso da biblioteca Unity para o teste unitário;

3. Use um teste com valor de ADC conhecido (ex.: para 0.706 V, a temperatura deve ser 27 °C).
---

## Objetivo Principal

O objetivo desta atividade é desenvolver e testar uma função que converte um valor lido do ADC interno da Raspberry Pi Pico W em temperatura (graus Celsius), utilizando a fórmula oficial da documentação da placa. Além disso, a atividade exige a criação de um teste unitário automatizado, utilizando a biblioteca Unity para garantir que a conversão esteja correta.

---

## Funcionamento

### Conversão de Temperatura

A função `adc_to_celsius(uint16_t adc_val)` realiza a conversão da leitura digital do ADC para temperatura, assumindo:

- Resolução do ADC: 12 bits (0–4095).
- Tensão de referência (Vref): 3.3 V.
- Fórmula de Conversão do sinal ADC para temperatura apresentada mais abaixo.

### Teste Unitário

- Um valor conhecido (ex.: `adc_val = 876`, correspondente a aproximadamente 0.706 V) é usado para verificar se a função retorna um valor próximo a 27.0 °C.
- A verificação é feita com `TEST_ASSERT_FLOAT_WITHIN`, aceitando uma margem de erro (ex: ±0.5 °C).
- O teste é exibido no terminal via USB serial, compatível com o PuTTY ou outras ferramentas seriais.

---

## Fórmula Utilizada

A fórmula abaixo foi utilizada no desenvolvimento do código deste exercício para realizar a conversão do valor de tensão lido pelo conversor analógico-digital, que está ligado (pelo canal 4) no sensor de temperatura interno do RP2040 da Raspberry Pi Pico W, para temperatura em graus Celsius:

> Temperature = 27.0 - (adc_voltage - 0.706)/0.001721;

> adc_voltage = (adc_val * 3.3/4095);

- 0.706 V: Tensão de referência para 27°C.
- 0.001721 V/°C: Coeficiente de variação da tensão com a temperatura.

Essa fórmula foi retirada da seguinte fonte:

- [Datasheet RP2040](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

---

## Bibliotecas do Pacote SDK da Raspberry Pi Pico utilizadas

- pico/stdlib.h: Fornece funções e tipos padrão (ex: uint16_t, stdio_init_all, sleep_ms).

## Biblioteca externa utilizada para o Desenvolvimento do Teste Unitário

- [Unity](https://github.com/ThrowTheSwitch/Unity)