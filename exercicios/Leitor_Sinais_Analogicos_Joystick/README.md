# Leitura dos Sinais Analógicos do Joystick

---

## Proposta do Exercício

Faça um programa em C para ler os valores convertidos digitalmente do joystick da BitDogLab. Os valores podem ser mostrados no terminal ou então no display OLED.

---

## Objetivos Principais

- Ler os valores dos eixos X e Y do joystick analógico utilizando os conversores analógico-digitais (ADC) do Raspberry Pi Pico W presente na BitDogLab.
- Exibir esses valores no display OLED de forma organizada e contínua.
- Permitir uma interface visual simples para monitorar o comportamento do joystick.

---

## Funcionamento

1. O programa inicializa os periféricos: ADCs, GPIOs e o display OLED.
2. No loop principal, os valores analógicos dos eixos do joystick são lidos via ADC.
3. Esses valores são convertidos para texto e exibidos no display OLED com atualização a cada 100 ms.

--- 

## GPIOs Utilizadas

| Função                 | GPIO     | Descrição                            |
|------------------------|----------|--------------------------------------|
| Eixo X do Joystick     | GPIO 27  | Entrada analógica (ADC Canal 1)      |
| Eixo Y do Joystick     | GPIO 26  | Entrada analógica (ADC Canal 0)      |
| SDA - Display OLED     | GPIO 14  | Linha de dados I2C                   |
| SCL - Display OLED     | GPIO 15  | Linha de clock I2C                   |

---

## Bibliotecas do pacote SDK da Raspberry Pi Pico utilizadas

- pico/stdlib.h - Controle básico da placa
- hardware/adc.h - Controle de ADCs
- hardware/i2c.h - Comunicação com o display via I2C

## Biblioteca externa utilizada para Exibição e Manipulação de Dados no Display OLED

- [ssd1306](https://github.com/BitDogLab/BitDogLab-C/tree/main/ssd1306)