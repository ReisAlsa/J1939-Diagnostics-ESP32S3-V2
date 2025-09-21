#include "CLIManager.h"
#include <SerialCmd.h>
#include "SettingsManager.h"
#include "LGFX_setup.h"
#include "ui/UIManager.h"
#include "common/LicenseManager.h"

extern UIManager uiManager; // Acesso ao gestor de UI

CLIManager cliManager;
SerialCmd serialCmd;

// --- Funções de Callback para os Comandos ---

// Função chamada quando um comando não é reconhecido
void unrecognized(const char* command) {
    Serial.print("Comando nao reconhecido: ");
    Serial.println(command);
}

// Comando 'help'
void showHelp() {
    Serial.println("--- Ajuda CLI ---");
    Serial.println("help - Mostra esta ajuda");
    Serial.println("reboot - Reinicia o aparelho");
    Serial.println("brightness <0-255> - Ajusta o brilho do ecra");
    Serial.println("screen <nome> - Muda para o ecra especificado (ex: MainMenu, Config)");
    Serial.println("license <chave> - Ativa uma nova licenca");
    Serial.println("-----------------");
}

// Comando 'reboot'
void rebootDevice() {
    Serial.println("A reiniciar...");
    delay(100);
    ESP.restart();
}

// Comando 'brightness'
void setBrightness() {
    char* arg = serialCmd.readStringArg();
    if (arg) {
        int brightness = atoi(arg);
        tft.setBrightness(brightness);
        settingsManager.saveBrightness(brightness);
        Serial.print("Brilho ajustado para: ");
        Serial.println(brightness);
    } else {
        Serial.println("Erro: Forneca um valor de brilho (0-255).");
    }
}

// Comando 'screen'
void switchScreen() {
    char* arg = serialCmd.readStringArg();
    if (arg) {
        uiManager.switch_to(arg);
        Serial.print("A mudar para o ecra: ");
        Serial.println(arg);
    } else {
        Serial.println("Erro: Forneca o nome do ecra.");
    }
}

// Comando 'license'
void setLicense() {
    char* arg = serialCmd.readStringArg();
    if (arg) {
        if (licenseManager.validateAndStoreLicense(arg)) {
            Serial.println("Licenca ativada com sucesso! A reiniciar...");
            delay(1000);
            ESP.restart();
        } else {
            Serial.println("Erro: Chave de licenca invalida.");
        }
    } else {
        Serial.println("Erro: Forneca a chave de licenca.");
    }
}

void CLIManager::begin() {
    serialCmd.begin(Serial); // Usa a porta Serial principal
    serialCmd.addCmd("help", showHelp);
    serialCmd.addCmd("reboot", rebootDevice);
    serialCmd.addCmd("brightness", setBrightness);
    serialCmd.addCmd("screen", switchScreen);
    serialCmd.addCmd("license", setLicense);
    serialCmd.setDefaultHandler(unrecognized);
    Serial.println("CLI inicializada. Digite 'help' para ver os comandos.");
}

void CLIManager::handle() {
    serialCmd.readSerial();
}