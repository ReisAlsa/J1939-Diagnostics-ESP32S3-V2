#include "SettingsManager.h"
#include <Preferences.h>

SettingsManager settingsManager; // Definição da instância global
Preferences preferences;

// Usamos um "namespace" para não misturar as nossas configurações com as de outras bibliotecas
const char* SETTINGS_NAMESPACE = "diag_tool";
const char* BRIGHTNESS_KEY = "brightness";
const char* SOUND_KEY = "sound";
const char* WIFI_SSID_KEY = "wifi_ssid";
const char* WIFI_PASS_KEY = "wifi_pass";

void SettingsManager::begin() {
    preferences.begin(SETTINGS_NAMESPACE, false); // false = modo leitura/escrita
}

void SettingsManager::saveBrightness(uint8_t brightness) {
    preferences.putUChar(BRIGHTNESS_KEY, brightness);
}

uint8_t SettingsManager::loadBrightness(uint8_t default_value) {
    return preferences.getUChar(BRIGHTNESS_KEY, default_value);
}

void SettingsManager::saveSoundState(bool enabled) {
    preferences.putBool(SOUND_KEY, enabled);
}

bool SettingsManager::loadSoundState(bool default_value) {
    return preferences.getBool(SOUND_KEY, default_value);
}

void SettingsManager::saveWiFiCredentials(const char* ssid, const char* password) {
    preferences.putString(WIFI_SSID_KEY, ssid);
    preferences.putString(WIFI_PASS_KEY, password);
}

void SettingsManager::loadWiFiCredentials(char* ssid, char* password, size_t len) {
    preferences.getString(WIFI_SSID_KEY, ssid, len);
    preferences.getString(WIFI_PASS_KEY, password, len);
}

void SettingsManager::saveString(const char* key, const char* value) {
    preferences.putString(key, value);
}

void SettingsManager::loadString(const char* key, char* value, size_t len, const char* default_value) {
    preferences.getString(key, value, len, default_value);
}

void SettingsManager::saveInt(const char* key, int32_t value) {
    preferences.putInt(key, value);
}

int32_t SettingsManager::loadInt(const char* key, int32_t default_value) {
    return preferences.getInt(key, default_value);
}