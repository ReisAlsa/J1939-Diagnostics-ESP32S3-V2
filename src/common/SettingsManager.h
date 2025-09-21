#pragma once
#include <cstdint>

class SettingsManager {
public:
    void begin();
    void saveBrightness(uint8_t brightness);
    uint8_t loadBrightness(uint8_t default_value = 255);
    void saveSoundState(bool enabled);
    bool loadSoundState(bool default_value = true);
    void saveWiFiCredentials(const char* ssid, const char* password);
    void loadWiFiCredentials(char* ssid, char* password, size_t len);
    void saveString(const char* key, const char* value);
    void loadString(const char* key, char* value, size_t len, const char* default_value = "");
    void saveInt(const char* key, int32_t value);
    int32_t loadInt(const char* key, int32_t default_value = 0);
};

extern SettingsManager settingsManager;