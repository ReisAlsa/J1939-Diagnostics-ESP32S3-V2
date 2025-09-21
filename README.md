# Ferramenta de Diagnóstico J1939 para Veículos Pesados

![Screenshot do Ecrã Principal](https://via.placeholder.com/480x480.png?text=Ecrã+Principal+do+Aparelho)

## Visão Geral

Este projeto consiste no desenvolvimento de uma **ferramenta de diagnóstico profissional** para veículos pesados de passageiros, utilizando o protocolo de comunicação **SAE J1939**. O hardware base é o **Waveshare ESP32-S3-Touch-LCD-5**, uma plataforma poderosa que integra um microcontrolador ESP32-S3, um ecrã tátil de 5 polegadas e o hardware necessário para a comunicação CAN.

O software foi desenhado com uma arquitetura moderna e modular, focada na escalabilidade, manutenibilidade e segurança, tornando-o uma base sólida para um produto comercial.

---

## Funcionalidades Principais

*   **Interface Gráfica Intuitiva:** Desenvolvida com a biblioteca **LVGL**, oferece uma experiência de utilizador fluida e profissional, com menus, teclado virtual e feedback visual em tempo real.
*   **Fluxo de Diagnóstico Completo:**
    1.  Seleção de veículo a partir de uma base de dados dinâmica.
    2.  Ecrã de diagnóstico em curso, preparado para exibir dados em tempo real das centralinas (ECUs).
*   **Base de Dados em Flash:** Utiliza o sistema de ficheiros **LittleFS** para armazenar configurações de veículos, permitindo que a lista de modelos suportados seja facilmente atualizada.
*   **Sistema de Licenciamento Seguro:**
    *   Ativação do aparelho através de uma chave de licença única.
    *   Validação baseada em **criptografia de chave pública/privada (RSA-2048)**, associando a licença ao ID de hardware do aparelho.
    *   Controlo de acesso por **data de validade** e **número de utilizações**, com verificação periódica online para maior segurança.
*   **Atualizações de Firmware Remotas e Locais:**
    *   **OTA (Over-the-Air):** Capacidade de descarregar e instalar atualizações de firmware através de uma ligação WiFi.
    *   **Cartão SD:** Método fiável para atualizações offline.
*   **Gestão de Configurações Persistente:**
    *   Guarda as preferências do utilizador (brilho, som, credenciais WiFi) na memória flash não-volátil (NVS).
*   **Interface de Linha de Comandos (CLI):** Uma poderosa ferramenta de depuração e controlo via porta serial, permitindo executar comandos para testes, ativação de licenças e gestão do aparelho.

---

## Arquitetura do Software

O projeto segue princípios de design de software robustos para garantir a sua qualidade e longevidade.

*   **Modularidade:** O código está organizado em gestores (`Manager`) e ecrãs (`Screen`), cada um com uma responsabilidade única:
    *   `UIManager`: Orquestra a navegação entre os diferentes ecrãs.
    *   `SettingsManager`: Centraliza a leitura e escrita de configurações na NVS.
    *   `DatabaseManager`: Abstrai o acesso à base de dados de veículos no LittleFS.
    *   `LicenseManager`: Encapsula toda a lógica de validação e gestão de licenças.
    *   `UpdateManager`: Gere os processos de atualização de firmware (OTA e SD).
    *   `CLIManager`: Implementa a interface de linha de comandos.
*   **Orientação a Objetos:** O uso de classes e da interface `IScreen` permite adicionar novos ecrãs e funcionalidades de forma limpa e com baixo acoplamento.
*   **C++ Moderno:** Utilização de funcionalidades como `std::unique_ptr`, `std::vector`, `std::function` e lambdas para um código mais seguro e expressivo.
*   **Multitarefa:** Processos longos, como as atualizações de firmware, são executados em tarefas separadas (FreeRTOS tasks) para não bloquear a interface do utilizador.

---

## Tecnologias e Bibliotecas

*   **Hardware:** Waveshare ESP32-S3-Touch-LCD-5
*   **Framework:** Arduino (com ESP-IDF subjacente)
*   **Ferramentas de Build:** PlatformIO
*   **Bibliotecas Principais:**
    *   `LovyanGFX`: Driver de alta performance para o ecrã e painel tátil.
    *   `LVGL (Light and Versatile Graphics Library)`: Para a criação da interface gráfica.
    *   `ArduinoJson`: Para manipulação de ficheiros de configuração em formato JSON.
    *   `mbedTLS`: Para as operações de criptografia do sistema de licenciamento.
    *   `SerialCmd`: Para a implementação da CLI.
    *   `Preferences`, `LittleFS`, `Update`, `WiFi`, `HTTPClient`: Bibliotecas padrão do ecossistema ESP32.

---

## Como Compilar e Usar

1.  **Instalar o Visual Studio Code** com a extensão **PlatformIO IDE**.
2.  Clonar este repositório.
3.  Abrir a pasta do projeto no VS Code. O PlatformIO irá instalar automaticamente todas as dependências listadas no `platformio.ini`.
4.  Conectar o aparelho via USB.
5.  Compilar e enviar o firmware usando os controlos do PlatformIO.

### Geração de Licenças

As chaves de licença são geradas através do script `generate_license.py`. Este script requer a chave privada (`private_key.pem`) correspondente à chave pública embutida no firmware.

```bash
# Instalar dependências
pip install cryptography

# Executar o gerador
python generate_license.py
```

---

## Autor

Este projeto foi desenvolvido integralmente por **Ricardo Reis**, com o apoio de ferramentas de assistência de código baseadas em IA para acelerar o desenvolvimento e garantir a qualidade do software.
