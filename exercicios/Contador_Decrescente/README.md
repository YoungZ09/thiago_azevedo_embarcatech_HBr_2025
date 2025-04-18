# Contador Decrescente com Registro de Eventos por Interrupção

---

## Proposta do Exercício

Faça um programa, em linguagem C, que implemente um contador decrescente controlado por interrupção, com o seguinte comportamento:

Toda vez que o Botão A (GPIO5) for pressionado:

- O contador decrescente reinicia em 9 e o valor da contagem é mostrado no display oled.
- O sistema entra em modo de contagem regressiva ativa, decrementando o contador de 1 em 1 a cada segundo até chegar em zero.
- Durante essa contagem (ou seja, de 9 até 0), o programa deve registrar quantas vezes o Botão B (GPIO6) foi pressionado. O valor deste registro de eventos de botão pressionado também deve ser mostrado no display OLED. 
- Quando o contador atingir zero, o sistema congela e ignora temporariamente os cliques no Botão B (eles não devem ser acumulados fora do intervalo ativo).

O sistema permanece parado após a contagem, exibindo:

- O valor 0 no contador.
- A quantidade final de cliques no Botão B registrados durante o período de 9 segundos (contagem regressiva).

Somente ao pressionar novamente o Botão A, o processo todo se reinicia:

- O contador volta para 9.
- O número de cliques do Botão B é zerado.
- A contagem recomeça do início.

---

## Objetivos Principais

- Desenvolver um sistema que utilize interrupções externas via GPIOs para interação com o usuário.
- Implementar um contador regressivo que atualiza automaticamente o tempo no display a cada segundo.
- Contabilizar os cliques do botão B apenas durante a contagem.
- Exibir as informações de forma clara e dinâmica no display OLED utilizando a biblioteca SSD1306.
- Garantir a estabilidade das leituras dos botões com tratamento de debounce por tempo.

---

## Funcionamento

1. Ao ligar o sistema, uma tela inicial de "boas-vindas" é exibida no display OLED.
2. O usuário pressiona o Botão A para iniciar a contagem regressiva de 9 a 0.
3. Durante esse tempo, os cliques no Botão B são registrados.
4. Ao final da contagem, a exibição dos dados é atualizada com o número final de cliques.
5. O processo pode ser reiniciado a qualquer momento pressionando novamente o Botão A.

---

## GPIOs Utilizadas

| Função             | GPIO | Descrição                     |
|--------------------|------|-------------------------------|
| Botão A            | 5    | Reinicia a contagem regressiva |
| Botão B            | 6    | Conta os cliques durante a contagem |
| Display OLED - SDA | 14   | Comunicação I2C (dados)        |
| Display OLED - SCL | 15   | Comunicação I2C (clock)        |
---

## Bibliotecas do pacote SDK da Raspberry Pi Pico utilizadas

- pico/stdlib.h - Controle básico da placa
- hardware/gpio.h - Controle de GPIOs e interrupções
- hardware/i2c.h - Comunicação com o display via I2C
- pico/time.h - Manipulação de tempo

## Biblioteca externa utilizada para Exibição e Manipulação de Dados no Display OLED

- [ssd1306](https://github.com/BitDogLab/BitDogLab-C/tree/main/ssd1306)
