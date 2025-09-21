#include "UpdateManager.h"
#include "SD.h"
#include "Update.h"
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "common/SettingsManager.h"


// Pinos do cartão SD para o ESP32-S3-Touch-LCD-5
#define SD_SCK  11
#define SD_MISO 13
#define SD_MOSI 12
#define SD_CS   10

UpdateManager updateManager;
SPIClass spi_sd(HSPI); // Usar o barramento HSPI para o cartão SD

void UpdateManager::performSdUpdate(const char* filename, ProgressCallback callback) {
    // 1. Inicializar o cartão SD
    spi_sd.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, spi_sd)) {
        callback(-1, "Falha ao montar cartao SD!");
        return;
    }
    callback(5, "Cartao SD encontrado.");
    delay(1000);

    // 2. Verificar se o ficheiro de atualização existe
    File updateBin = SD.open(filename);
    if (!updateBin) {
        char errorMsg[64];
        snprintf(errorMsg, sizeof(errorMsg), "Ficheiro '%s' nao encontrado!", filename);
        callback(-1, errorMsg);
        SD.end();
        return;
    }
    callback(10, "Ficheiro encontrado. A iniciar...");
    delay(1000);

    size_t updateSize = updateBin.size();
    if (updateSize == 0) {
        callback(-1, "Ficheiro de atualizacao vazio.");
        updateBin.close();
        SD.end();
        return;
    }

    // 3. Iniciar o processo de atualização
    if (!Update.begin(updateSize)) {
        callback(-1, "Espaco insuficiente para atualizar.");
        updateBin.close();
        SD.end();
        return;
    }

    // Define a função de callback para o progresso da biblioteca Update
    Update.onProgress([&](size_t progress, size_t total) {
        int percentage = (progress * 100) / total;
        // Mapeamos o progresso da atualização (0-100%) para a nossa barra (10-90%)
        int ui_percentage = 10 + (percentage * 80 / 100);
        callback(ui_percentage, "A atualizar...");
    });

    // 4. Escreve o conteúdo do ficheiro para a flash
    size_t bytesWritten = Update.writeStream(updateBin);
    updateBin.close();

    if (bytesWritten != updateSize) {
        callback(-1, "Erro durante a escrita do firmware.");
        SD.end();
        return;
    }

    // 5. Finaliza a atualização
    if (!Update.end(true)) { // true para definir a partição de boot e reiniciar
        callback(-1, "Erro ao finalizar a atualizacao.");
        SD.end();
        return;
    }

    callback(100, "Atualizacao completa! A reiniciar...");
    delay(2000);
    ESP.restart();
}

void UpdateManager::performOtaUpdate(const char* url, ProgressCallback callback) {
    // 1. Conectar ao WiFi
    callback(5, "A ligar ao WiFi...");
    char ssid[64] = {0};
    char pass[64] = {0};
    settingsManager.loadWiFiCredentials(ssid, pass, sizeof(ssid));

    if (strlen(ssid) == 0) {
        callback(-1, "WiFi nao configurado!");
        return;
    }

    WiFi.begin(ssid, pass);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 30) { // Timeout de ~15 segundos
        delay(500);
        retries++;
    }

    if (WiFi.status() != WL_CONNECTED) {
        callback(-1, "Falha ao ligar ao WiFi!");
        return;
    }
    callback(10, "Ligado ao WiFi. A obter ficheiro...");
    delay(1000);

    // 2. Iniciar o cliente HTTP
    HTTPClient http;
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        char errorMsg[64];
        snprintf(errorMsg, sizeof(errorMsg), "Erro HTTP: %d", httpCode);
        callback(-1, errorMsg);
        http.end();
        return;
    }

    int contentLength = http.getSize();
    if (contentLength <= 0) {
        callback(-1, "Tamanho de ficheiro invalido.");
        http.end();
        return;
    }

    // 3. Iniciar o processo de atualização
    if (!Update.begin(contentLength)) {
        callback(-1, "Espaco insuficiente para atualizar.");
        http.end();
        return;
    }

    Update.onProgress([&](size_t progress, size_t total) {
        int percentage = (progress * 100) / total;
        int ui_percentage = 10 + (percentage * 80 / 100);
        callback(ui_percentage, "A descarregar...");
    });

    // 4. Escreve o stream HTTP para a flash
    size_t bytesWritten = Update.writeStream(http.getStream());

    if (bytesWritten != contentLength) {
        callback(-1, "Erro durante o download.");
        http.end();
        return;
    }

    // 5. Finaliza a atualização
    if (!Update.end(true)) {
        callback(-1, "Erro ao finalizar a atualizacao.");
    } else {
        callback(100, "Atualizacao completa! A reiniciar...");
        delay(2000);
        ESP.restart();
    }

    http.end();
}