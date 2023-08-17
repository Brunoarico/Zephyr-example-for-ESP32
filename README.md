# Exemplo Zephyr RTOS

Este é um trecho de código de exemplo usando o Zephyr RTOS. Ele demonstra o uso de threads e controle de GPIO.
Este codigo é projetado para funcinar em uma placa de desenvolvimento `ESP32 DOIT DevKit V1`.

Instruções de como configurar o ambiente para executar o código estão disponíveis no [Embarcados](https://embarcados.com.br/zephyr-rtos-no-esp32-primeiros-passos/) e no [Zephyr Project](https://docs.zephyrproject.org/latest/getting_started/index.html).


## Constantes Globais e Definições

- `ON` e `OFF` são constantes para representar estados do LED.
- O uso das diretivas de compilação condicional `ENABLE_SEM` e `EQUAL_PRIORITY` permite uma configuração flexível.
- As prioridades (`A_PRIORITY` e `B_PRIORITY`) são definidas de forma diferente com base na presença da diretiva `EQUAL_PRIORITY`.
- `DT_DRV_COMPAT` especifica a compatibilidade do driver GPIO.
- `SLEEP_TIME` define o tempo de espera das threads.
- `LED_PIN` especifica o pino GPIO para controlar o LED.
- `STACKSIZE` define o tamanho da pilha da thread.
- `MATRIX_SIZE` define o tamanho das matrizes usadas na simulação.

## Variáveis Globais

- `led`: Um ponteiro para o dispositivo LED.

## Definições de Threads

- Stacks e estruturas de dados são definidos para as threads `threadA_data` e `threadB_data`.

## Definições de Semáforos (Opcional)

- Semáforos `on_sem` e `off_sem` são definidos se `ENABLE_SEM` estiver definido. Eles são usados para sincronização.

## Função: matrixMulSum()

- Esta função simula processamento intensivo de CPU usando multiplicação de matrizes.
- Duas matrizes são geradas aleatoriamente e multiplicadas para produzir uma matriz de resultado.
- A função utiliza geração de números aleatórios usando `sys_rand32_get()`.

## Função: configLed()

- Configura o pino GPIO do LED.
- O driver GPIO é configurado com base em `DT_NODELABEL(gpio0)` na árvore de dispositivos.

## Função: worker()

- Esta é a função de trabalho para as threads A e B.
- Ela controla o estado do LED com base na ação fornecida.
- Simula processamento intensivo de CPU chamando a função `matrixMulSum()`.
- Imprime informações da thread, incluindo iteração e tempo de execução.
- Usa as funções `k_sem_take()` e `k_sem_give()` para sincronização se `ENABLE_SEM` estiver definido.

## Função: configThreads()

- Configura e cria as threads A e B.
- As threads são criadas usando `k_thread_create()` com atributos e prioridades específicas.
- Nomes das threads são definidos usando `k_thread_name_set()`.

## Função: main()

- A função principal configura o LED e as threads.
- As threads são iniciadas usando `k_thread_start()`.
- Semáforos (opcional) são dados se `ENABLE_SEM` estiver definido.


# prj.conf

O arquivo de configuração permite que você personalize as opções e recursos para atender às necessidades do seu projeto.

## Opções de Configuração

- `CONFIG_GPIO=y`: Habilita o suporte à GPIO, permitindo controlar dispositivos de Entrada/Saída Geral.

- `CONFIG_SCHED_DUMB=y`: Habilita o escalonador "Dumb" (bobo), que é um escalonador simples e não-preemptivo. Ele atribui tempo de execução fixo para cada thread e não permite que threads de prioridades mais altas interrompam as de prioridades mais baixas.

- `CONFIG_SCHED_SCALABLE=y`: Habilita o escalonador "Scalable" (escalável), que é um escalonador baseado em prioridades (Red/black tree ready queue) com preempção. Ele suporta uma ampla faixa de prioridades e permite escalonamento preemptivo. 

- `CONFIG_SCHED_MULTIQ=y`: Habilita o escalonador "MultiQ", que é um escalonador de múltiplas filas baseado em prioridades com preempção. Ele oferece uma abordagem de filas múltiplas para melhorar a justiça e evitar bloqueios de prioridades.

- `CONFIG_SCHED_DEADLINE=y`: Habilita o escalonador "Deadline" (prazo final), que implementa o escalonamento EDF (Earliest Deadline First) ou "Primeiro Prazo Mais Próximo". Threads têm prazos associados e o escalonador tenta garantir que todas as threads cumpram seus prazos.

- `CONFIG_THREAD_NAME=y`: Habilita o suporte a nomes de threads, permitindo que você nomeie suas threads para facilitar a depuração e identificação.

- `CONFIG_SCHED_CPU_MASK=y`: Habilita a capacidade de definir máscaras de CPU para threads. Isso permite controlar em quais CPUs específicas as threads podem ser executadas, o que é útil para sistemas multi-core.

Para mais informações sobre os escalonadores, veja [Zephyr RTOS Scheduling](https://docs.zephyrproject.org/latest/kernel/services/scheduling/index.html). 