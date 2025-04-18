# Monitor da Temperatura Interna da MCU

---

## Proposta do Exercício

Faça um programa em C para ler a temperatura interna do RP2040. Converta a leitura do ADC em um valor em ºC.

---

## Objetivos Principais

- Ler a temperatura interna da MCU (RP2040) utilizando o canal ADC 4.
- Converter o valor do ADC para a escala de graus Celsius com base na fórmula recomendada no datasheet do RP2040.
- Exibir a temperatura formatada no display OLED SSD1306 conectado via interface I2C.

---

## Funcionamento

1. Inicialização dos periféricos (ADC, I2C e display OLED).
2. Leitura da temperatura interna da MCU via ADC (canal 4).
3. Conversão do valor ADC para temperatura em Celsius.
4. Exibição dos dados em tempo real no display OLED (atualizado a cada 1 segundo).

---

## GPIOs Utilizadas

| Função                      | GPIO    | Descrição                              |
|-----------------------------|---------|----------------------------------------|
| SDA (dados I2C - OLED)      | GPIO 14 | Linha de dados do barramento I2C       |
| SCL (clock I2C - OLED)      | GPIO 15 | Linha de clock do barramento I2C       |
| Sensor de Temperatura ADC   | Interno | Canal ADC 4 (sem GPIO externa associada) |

---

## Fórmula utilizada

A fórmula abaixo foi utilizada no desenvolvimento do código deste exercício para realizar a conversão do valor de tensão lido pelo conversor analógico-digital, que está ligado (pelo canal 4) no sensor de temperatura interno do RP2040 da Raspberry Pi Pico W, para temperatura em graus Celsius:

> Temperature = 27.0 - (adc_voltage - 0.706)/0.001721;

- 0.706 V: Tensão de referência para 27°C
- 0.001721 V/°C: Coeficiente de variação da tensão com a temperatura

Essa fórmula foi retirada da seguinte fonte:

- [Datasheet RP2040](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

---

## Bibliotecas do pacote SDK da Raspberry Pi Pico utilizadas

- pico/stdlib.h - Controle básico da placa
- hardware/adc.h - Controle de ADCs
- hardware/i2c.h - Comunicação com o display via I2C

## Biblioteca externa utilizada para Exibição e Manipulação de Dados no Display OLED

- [ssd1306](https://github.com/BitDogLab/BitDogLab-C/tree/main/ssd1306)