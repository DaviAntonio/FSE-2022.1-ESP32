# ESP32 MQTT Thingsboard

Projeto final da disciplina Fundamentos de Sistemas Embarcados, semestre 2022.1,
da Universidade de Brasília, campus Gama.

Este projeto comunica-se por MQTT com um servidor Thingsboard, reportando como
telemetria dados de temperatura em graus Celsius, umidade relativa, e o estado
do botão do kit de desenvolvimento. Também são comunicados como atributos os
avisos de temperatura alta, temperatura baixa, umidade alta e umidade baixa. Os
limites para mudança dos atributos estão disponíveis em código.

## Hardware

O projeto consiste em:

- uma placa de desenvolvimento compatível com a DEVKIT V1, com um módulo
ESP-WROOM-32
- um módulo com o sensor de temperatura e umidade relativa DHT11
- um módulo LED RGB
- um módulo de *buzzer* passivo com disparo em nível lógico baixo

Os seguintes pinos são usados:

| Função         | Modo de funcionamento | Periférico               | Interrupção         | Número no ESP-WROOM-32 | Descrição na placa de desenvolvimento |
|----------------|-----------------------|--------------------------|---------------------|------------------------|---------------------------------------|
| botão da placa | entrada               | GPIO                     | *wakeup*,*any edge* | 0                      | BOOT                                  |
| LED da placa   | saída                 | LEDC, TIMER 0, CHANNEL 0 | Não                 | 2                      | D2                                    |
| DHT11          | entrada e saída       | GPIO                     | Não                 | 16                     | RX2                                   |
| LED vermelho   | saída                 | LEDC, TIMER 0, CHANNEL 1 | Não                 | 17                     | TX2                                   |
| LED verde      | saída                 | LEDC, TIMER 0, CHANNEL 2 | Não                 | 18                     | D18                                   |
| LED azul       | saída                 | LEDC, TIMER 0, CHANNEL 3 | Não                 | 19                     | D19                                   |
| *Buzzer*       | saída                 | LEDC, TIMER 1, CHANNEL 4 | Não                 | 33                     | D33                                   |

## Requisitos para compilação

O projeto depende de uma instalação válida dos seguintes softwares:

- `esp-idf v4.4.2-288-gc070c58ef3`

### Instruções de compilação

1. Clone o projeto

	`git clone https://github.com/DaviAntonio/FSE-2022.1-ESP32.git`

2. Atualize os módulos

	`git submodule update --init --recursive`

3. Configure o projeto

	`idf.py menuconfig`

4. Compile os códigos-fonte

	`idf.py build`

5. Grave o resultado na memória *flash* da ESP32

	`idf.py flash`

6. Execute e monitore

	`idf.py monitor`

#### Opções do Kconfig

As seguintes opções podem ser configuradas usando-se o `menuconfig`:

##### Menu "Project's Wifi Configuration"

- `ESP_WIFI_SSID`, o SSID da rede usada para se conectar à Internet
- `ESP_WIFI_PASSWORD`, a senha da rede
- `ESP_MAXIMUM_RETRY`, número máximo de tentativas de conexão

##### Menu "MQTT Configuration"

- `MQTT_SERVER_URI`, o endereço usado para se conectar ao intermediário
(*broker*) MQTT usado pelo Thingsboard
- `MQTT_USERNAME`, o *token* usado para se comunicar com o *broker* MQTT do
Thingsboard

##### Menu "Operation Mode"

- `BATTERY_MODE`, caso marcada, o microcontrolador entrará no modo *Light Sleep*
periodicamente (entre 10 e 20 segundos). É possível retirá-lo do modo de
suspensão ativando o botão da placa.

## Copyright

Este projeto usa códigos modificados escritos por Renato Coral.

A comunicação com o DHT11 usa a biblioteca ESP32-DHT11, licenciada sob a MIT por
Michele Biondi.

Há, no projeto, o uso da esp-idf, bem como das bibliotecas de código livre ou
aberto incorporadas nela.

Os demais códigos estão licenciados por Davi Antônio e Wellington sob a AGPLv3.