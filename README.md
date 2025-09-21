# Ferramenta de Diagnóstico J1939 para Veículos Pesados

## Visão Geral

Este projeto consiste no desenvolvimento de uma **ferramenta de diagnóstico profissional** para veículos pesados de passageiros, utilizando o protocolo de comunicação **SAE J1939**. O hardware base é o **Waveshare ESP32-S3-Touch-LCD-5**, uma plataforma poderosa que integra um microcontrolador ESP32-S3, um ecrã tátil de 5 polegadas e o hardware necessário para a comunicação CAN.

O software foi desenhado com uma arquitetura moderna e modular, focada na escalabilidade, manutenibilidade e performance, tornando-o uma base sólida para um produto comercial.

---

## Funcionalidades Principais

*   **Interface Gráfica Intuitiva:** Desenvolvida com a biblioteca **LVGL**, oferece uma experiência de utilizador fluida e profissional, com menus, teclado virtual e feedback visual em tempo real.

*   **Sistema de Diagnóstico Avançado:**
    *   **Deteção de ECUs:** Apresenta uma lista de centralinas (ECUs) detetadas na rede do veículo, com indicadores visuais de estado (vermelho para erros ativos, laranja para erros memorizados).
    *   **Menu de Funções por ECU:** Ao selecionar uma centralina, acede-se a um menu de contexto com opções como:
        *   `Identificação`: Exibe dados como número de peça, série e versão de software.
        *   `Ler Erros`: Mostra uma lista detalhada de DTCs (SPN, FMI e descrição).
        *   `Apagar Erros`: Envia o comando para limpar os erros memorizados, com um pedido de confirmação para segurança.
        *   `Valores Atuais`: Apresenta uma lista de parâmetros em tempo real (live data) que se atualiza continuamente.
    *   **Visualização de Dados em Gráfico:**
        *   Gráficos dinâmicos que se atualizam em tempo real ao selecionar um valor da lista.
        *   Suporte para **múltiplas séries** de dados no mesmo gráfico para comparação.
        *   Uso do **eixo Y secundário** para comparar valores com escalas muito diferentes (ex: RPM vs. Temperatura).
        *   Funcionalidades de **Zoom** e **Scroll Horizontal** para análise detalhada do histórico de dados.
        *   Botão para **Pausar e Resumir** a captura de dados em tempo real.

*   **Gestão Completa de Relatórios:**
    *   **Geração de Relatórios:** Cria relatórios de diagnóstico detalhados, incluindo dados do veículo, **timestamp preciso obtido via NTP**, e a lista completa de ECUs e seus respetivos erros.
    *   **Armazenamento e Visualização:** Guarda os relatórios na memória interna (LittleFS) e permite listá-los e visualizá-los diretamente no aparelho.
    *   **Exportação e Eliminação:** Permite **exportar relatórios para um cartão SD** e apagar relatórios da memória interna com confirmação.

*   **Base de Dados em Flash:** Utiliza o sistema de ficheiros **LittleFS** para armazenar configurações de veículos, permitindo que a lista de modelos suportados seja facilmente atualizada.

*   **Sistema de Licenciamento Seguro:**
    *   Ativação do aparelho através de uma chave de licença única.
    *   Validação baseada em **criptografia de chave pública/privada (RSA-2048)**, associando a licença ao ID de hardware do aparelho.

*   **Otimizações de Performance:**
    *   **Uso de PSRAM:** Toda a alocação de memória do LVGL é direcionada para os **8MB de PSRAM**, libertando a RAM interna do ESP32 para tarefas críticas.
    *   **Double Buffering:** Os buffers de ecrã do LVGL são alocados na PSRAM com uma estratégia de double buffering para maximizar a performance de renderização e eliminar "tearing".

*   **Atualizações de Firmware Remotas e Locais:**
    *   **OTA (Over-the-Air):** Capacidade de descarregar e instalar atualizações de firmware através de uma ligação WiFi.
    *   **Cartão SD:** Método fiável para atualizações offline.

*   **Gestão de Configurações Persistente:**
    *   Guarda as preferências do utilizador (brilho, som, credenciais WiFi) na memória flash não-volátil (NVS).

*   **Interface de Linha de Comandos (CLI):** Uma poderosa ferramenta de depuração e controlo via porta serial, permitindo executar comandos para testes, ativação de licenças e gestão do aparelho.

---

## Arquitetura do Software

O projeto segue princípios de design de software robustos para garantir a sua qualidade e longevidade:

*   **Modularidade:** O código está organizado em gestores (`Manager`), ecrãs (`Screen`) e um diretório `core` para lógica de negócio, cada um com uma responsabilidade única.
*   **Orientação a Objetos:** O uso de classes e da interface `IScreen` permite adicionar novos ecrãs e funcionalidades de forma limpa e com baixo acoplamento.
*   **C++ Moderno:** Utilização de funcionalidades como `std::unique_ptr`, `std::vector`, `std::function` e lambdas para um código mais seguro e expressivo.

---

## Tecnologias e Bibliotecas

*   **Hardware:** Waveshare ESP32-S3-Touch-LCD-5
*   **Framework:** Arduino (com ESP-IDF subjacente)
*   **Ferramentas de Build:** PlatformIO
*   **Bibliotecas Principais:** `LovyanGFX`, `LVGL`, `ArduinoJson`, `mbedTLS`, `SerialCmd`, `LittleFS`, `SD`, `WiFi`, `NTPClient`.

---

## Como Compilar e Usar

1.  **Instalar o Visual Studio Code** com a extensão **PlatformIO IDE**.
2.  Clonar este repositório.
3.  Abrir a pasta do projeto no VS Code. O PlatformIO irá instalar automaticamente todas as dependências.
4.  Conectar o aparelho via USB.
5.  Compilar e enviar o firmware usando os controlos do PlatformIO.

---

## Roadmap de Desenvolvimento

### Curto Prazo
-   [ ] **Implementação da Camada de Comunicação J1939:** Substituir as funções de simulação (`read_dtcs_from_ecu`, etc.) pela lógica real de comunicação CAN, utilizando uma biblioteca como `ESP32-CAN`.
-   [ ] **Obtenção de Dados Reais:** Implementar a leitura do VIN (PGN 65260) e outros dados do veículo para preencher automaticamente os relatórios.
-   [ ] **Base de Dados de DTCs:** Criar um sistema para mapear combinações de SPN/FMI a descrições detalhadas, tornando o diagnóstico mais preciso.

### Médio Prazo
-   [ ] **Testes de Atuadores:** Adicionar um novo menu para executar testes de componentes (ex: ativar ventoinha, testar luzes).
-   [ ] **Função de "Snapshot":** Implementar a capacidade de gravar um trecho de dados em tempo real (live data) para análise posterior.
-   [ ] **Suporte a Múltiplos Idiomas:** Refatorar a UI para carregar textos a partir de ficheiros de tradução.

### Longo Prazo
-   [ ] **Portal Web Complementar:** Desenvolver uma aplicação web para gestão de frotas, onde os relatórios exportados possam ser carregados e analisados.
-   [ ] **Suporte a Outros Protocolos:** Expandir a arquitetura para incluir outros protocolos de diagnóstico, como J1708 ou ISO 14229 (UDS).
-   [ ] **Integração com Cloud:** Sincronizar relatórios e configurações automaticamente com um serviço na nuvem.

---

## Sugestões para Futuras Contribuições

*   **Melhorar o Design da UI/UX:** Explorar temas personalizados para o LVGL, incluindo um modo escuro.
*   **Editor de Veículos:** Criar um ecrã dentro da aplicação para adicionar ou editar veículos na base de dados, sem depender de um ficheiro JSON.
*   **Gráficos Avançados:** Adicionar cursores ao gráfico para medir diferenças de tempo e valor entre dois pontos.
*   **Exportação em PDF:** Modificar a função de exportação para gerar relatórios em formato PDF em vez de texto simples.

---

## Autor

Este projeto foi desenvolvido por **Ricardo Reis**, com o apoio de ferramentas de assistência de código baseadas em IA para acelerar o desenvolvimento e garantir a qualidade do software.

