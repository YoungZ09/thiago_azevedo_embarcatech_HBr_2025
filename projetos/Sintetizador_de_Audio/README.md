
# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: Thiago Young de Azevedo

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, Junho de 2025

---
# Sintetizador de Áudio

## 🎯 Objetivos Principais do Projeto

Este projeto tem como objetivo o desenvolvimento de um **sintetizador de áudio embarcado**, utilizando a placa **BitDogLab** com o microcontrolador **Raspberry Pi Pico W**. O sistema deve ser capaz de:

- **Gravar** um trecho de áudio com o microfone acoplado na placa via ADC.
- **Armazenar** os dados de áudio em um buffer na memória.
- **Reproduzir** o áudio armazenado em dois buzzers via modulação PWM.
- **Exibir** a forma de onda do áudio captado no **display OLED SSD1306**.
- Utilizar **botões físicos** para controle e **LED RGB** para feedback visual do estado do sistema.

---

## ⚙️ Funcionamento Resumido

- O **botão A (GPIO 5)** inicia a **gravação** do áudio por 4 segundos, amostrando o sinal do microfone com uma taxa de **11.025 Hz**.
- O áudio captado é armazenado no buffer `audio_buffer[]`, e uma **forma de onda** correspondente é desenhada no **display OLED**.
- O **botão B (GPIO 6)** inicia a **reprodução** do áudio usando dois buzzers (PWM) presentes nas **GPIOs 10 e 21**, respeitando a mesma taxa de amostragem.
- Um **LED RGB** indica o estado atual:
  - **Vermelho aceso**: Gravação em andamento.
  - **Verde aceso**: Reprodução em andamento.

---

## 🧩 GPIOs Utilizadas

| Componente          | GPIO  | Função / Descrição                         |
|---------------------|-------|--------------------------------------------|
| Microfone (ADC)     | 28    | Canal ADC2 – entrada analógica             |
| Botão A (Gravação)  | 5     | Entrada digital com pull-up                |
| Botão B (Reprodução)| 6     | Entrada digital com pull-up                |
| Buzzer A            | 21    | Saída PWM                                  |
| Buzzer B            | 10    | Saída PWM                                  |
| LED Verde           | 11    | Saída digital                              |
| LED Vermelho        | 13    | Saída digital                              |
| Display OLED SDA    | 14    | Comunicação I2C (dados)                    |
| Display OLED SCL    | 15    | Comunicação I2C (clock)                    |

---

## 📚 Bibliotecas Utilizadas

### 📦 Bibliotecas padrão e de hardware da Raspberry Pi Pico:

- `<stdio.h>` – Entrada/saída padrão (ex: `printf`)
- `<string.h>` – Manipulação de memória (ex: `memset`)
- `"pico/stdlib.h"` – Funções básicas do SDK Pico
- `"hardware/gpio.h"` – Controle de GPIOs (botões, LEDs)
- `"hardware/adc.h"` – Leitura de entrada analógica via ADC
- `"hardware/pwm.h"` – Geração de sinal PWM
- `"hardware/i2c.h"` – Comunicação I2C para o display OLED

### 📦 Biblioteca externa referente ao display OLED SSD1306 (inserida no projeto):

- [ssd1306](https://github.com/BitDogLab/BitDogLab-C/tree/main/ssd1306)

---

## Vídeo Demonstrativo

O link abaixo é referente a um vídeo demonstrando o funcionamento completo do sintetizador de áudio embarcado na Bitdoglab, realizando a gravação e reprodução do áudio captado pelo microfone e, além disso, é apresentado a forma de onda original deste sinal no display OLED.

Link: https://www.youtube.com/watch?v=4VoJ2sfPg-U


---

## 📜 Licença
GNU GPL-3.0.

