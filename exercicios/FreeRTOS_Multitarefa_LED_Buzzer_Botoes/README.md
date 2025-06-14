
# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: Thiago Young de Azevedo

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, Junho de 2025

---
## Projeto FreeRTOS na BitDogLab – Multitarefa com LED, Buzzer e Botões
---
## Proposta da Atividade

Esta atividade prática propõe o desenvolvimento de um sistema embarcado multitarefa utilizando o FreeRTOS com a placa BitDogLab (baseada no Raspberry Pi Pico W). O sistema deve controlar, de forma concorrente, três periféricos: um LED RGB, um buzzer e dois botões físicos, utilizando a linguagem C e o ambiente de desenvolvimento VSCode com pico-sdk e FreeRTOS integrados.

---
## Objetivos Principais

- Compreender e aplicar os conceitos de multitarefa utilizando o FreeRTOS.
- Criar tarefas independentes para controle de periféricos com `vTaskCreate()`.
- Controlar a execução das tarefas com `vTaskSuspend()` e `vTaskResume()`.
- Utilizar `vTaskDelay()` para temporização e controle de escalonamento.
- Manipular GPIOs de entrada e saída com o SDK da Raspberry Pi Pico.

---
## Funcionamento Resumido do Código

O sistema implementado cria e executa três tarefas simultâneas:

- **Tarefa LED RGB**: alterna ciclicamente entre as cores vermelho, verde e azul a cada 500 ms.
- **Tarefa Buzzer**: emite um beep curto (100 ms) a cada segundo (900 ms de silêncio).
- **Tarefa Botões**: realiza polling dos botões A e B a cada 100 ms, com debounce, permitindo:
  - Suspender e retomar a tarefa do LED RGB (botão A).
  - Suspender e retomar a tarefa do buzzer (botão B).

As tarefas são executadas com a mesma prioridade, utilizando o escalonador do FreeRTOS para alternar sua execução de maneira justa e ordenada.

---
## GPIOs Utilizadas

| Periférico | GPIO | Descrição            |
|------------|------|----------------------|
| Botão A      | 5  | Entrada digital com pull-up interno |
| Botão B      | 6  | Entrada digital com pull-up interno |
| LED Verde    | 11 | Saída digital |
| LED Azul     | 12 | Saída digital |
| LED Vermelho | 13 | Saída digital |
| Buzzer (PWM) | 21 | Saída PWM para gerar som |

---
## Respostas às Questões do Enunciado

### 1. O que acontece se todas as tarefas tiverem a mesma prioridade?

Quando as tarefas têm a mesma prioridade, o FreeRTOS realiza o escalonamento por tempo (round-robin), alternando entre as tarefas com base em seus estados (executando, bloqueada, suspensa). Cada tarefa espera com `vTaskDelay()`, o que permite que o escalonador entregue o controle da CPU para as demais de forma ordenada.

### 2. Qual tarefa consome mais tempo da CPU?

A tarefa que mais ativa o processador com frequência é a tarefa dos botões, pois realiza polling a cada 100 ms. No entanto, ela tem uma execução curta. Já as tarefas do LED e do buzzer têm períodos mais longos e são em grande parte bloqueadas com `vTaskDelay()`. Em geral, o consumo de CPU é bem balanceado neste projeto.

### 3. Quais seriam os riscos de usar polling sem prioridades?

O uso de polling constante sem `vTaskDelay()` (ou com baixa prioridade) pode levar ao uso excessivo da CPU, fazendo com que o escalonador não tenha tempo para alternar entre tarefas. Isso pode causar travamento, baixa responsividade do sistema e ineficiência energética, especialmente em sistemas embarcados com recursos limitados.

---
## Bibliotecas Utilizadas

- `FreeRTOS.h`: núcleo do RTOS (tipos e funções base).
- `task.h`: funções de criação e gerenciamento de tarefas.
- `hardware/gpio.h`: controle de GPIOs (entrada/saída).
- `hardware/pwm.h`: controle de sinais PWM.
- `hardware/clocks.h`: leitura do clock do sistema.
- `pico/stdlib.h`: biblioteca padrão do pico-sdk (GPIO, tempo, UART).
- `pico/time.h`: temporização baseada em software.
- `pico/types.h`: tipos padrão do SDK.
- `stdio.h`: entrada/saída padrão (debug com `printf`).

---
## 📜 Licença
GNU GPL-3.0.
