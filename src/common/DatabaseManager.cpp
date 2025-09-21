#include "DatabaseManager.h"
#include "LittleFS.h"
#include <ArduinoJson.h> // Adicione 'bblanchon/ArduinoJson' aos lib_deps no platformio.ini

DatabaseManager databaseManager;

bool DatabaseManager::begin() {
    if (!LittleFS.begin(true)) { // true = formatar se a montagem falhar
        LV_LOG_ERROR("Falha ao montar o sistema de ficheiros LittleFS");
        return false;
    }
    LV_LOG_INFO("LittleFS montado com sucesso.");

    // Exemplo: Criar um ficheiro de veículos se não existir
    if (!LittleFS.exists("/vehicles.json")) {
        LV_LOG_INFO("Ficheiro /vehicles.json não encontrado. A criar um ficheiro de exemplo.");
        File file = LittleFS.open("/vehicles.json", "w");
        if (file) {
            file.print("[\"Mercedes Conecto\",\"Irizar i4 MERCEDES 3 eixos\",\"Irizar I4 MAN GNC\",\"ZHONG TONG\",\"YUTONG E9\"]");
            file.close();
        }
    }
    return true;
}

std::vector<std::string> DatabaseManager::getVehicleList() {
    std::vector<std::string> vehicles;
    File file = LittleFS.open("/vehicles.json");
    if (!file) return vehicles;

    JsonDocument doc;
    deserializeJson(doc, file);
    for (JsonVariant v : doc.as<JsonArray>()) {
        vehicles.push_back(v.as<std::string>());
    }
    file.close();
    return vehicles;
}