#include "LicenseManager.h"
#include "SettingsManager.h"
#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "mbedtls/md.h"
#include "mbedtls/pk.h"
#include "mbedtls/base64.h"

LicenseManager licenseManager;

// Chaves para guardar o estado da licença na memória NVS
const char* LICENSE_KEY = "license_key";
const char* USES_LEFT_KEY = "uses_left";
const char* USES_SINCE_CHECK_KEY = "uses_since_check";

// --- Chave Pública RSA ---
// Substitua pelo conteúdo da sua chave pública gerada com OpenSSL.
// Certifique-se de adicionar '\n' no final de cada linha e um '\0' no final.
const char PUBLIC_KEY[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prE/xV2p8gYd2E\n"
    "d/Z6c8y/HZ8t3t2+4d1g2f3g4h5i6j7k8l9m0n1o2p3q4r5s6t7u8v9w0x1y2z3a\n"
    "4b5c6d7e8f9g0h1i2j3k4l5m6n7o8p9q0r1s2t3u4v5w6x7y8z9a0b1c2d3e4f5g\n"
    "6h7i8j9k0l1m2n3o4p5q6r7s8t9u0v1w2x3y4z5a6b7c8d9e0f1g2h3i4j5k6l7m\n"
    "8n9o0p1q2r3s4t5u6v7w8x9y0z1a2b3c4d5e6f7g8h9i0j1k2l3m4n5o6p7q8r9s\n"
    "0t1u2v3w4x5y6z7a8b9c0d1e2f3g4h5i6j7k8l9m0n1o2p3q4r5s6t7u8v9w0x1y\n"
    "2z==\n" // Exemplo - use a sua chave real
    "-----END PUBLIC KEY-----\n";

void LicenseManager::begin() {
    // A inicialização é feita através do SettingsManager
}

bool LicenseManager::validateAndStoreLicense(const std::string& key) {
    // --- LÓGICA DE VALIDAÇÃO DA CHAVE ---
    // Esta é uma simulação. Numa aplicação real, usaria criptografia (ex: RSA, AES)
    // A chave de licença agora é: Base64(dados_licenca.assinatura_digital)
    // Onde dados_licenca = "HWID:YYYYMMDD:USES"

    // 1. Descodificar a chave de Base64
    unsigned char decoded_buf[512];
    size_t decoded_len;
    if (mbedtls_base64_decode(decoded_buf, sizeof(decoded_buf), &decoded_len, (const unsigned char*)key.c_str(), key.length()) != 0) {
        LV_LOG_ERROR("Falha ao descodificar a chave Base64.");
        return false;
    }

    // A assinatura está nos últimos 256 bytes (para RSA-2048)
    if (decoded_len <= 256) {
        LV_LOG_ERROR("Comprimento da chave descodificada invalido.");
        return false;
    }
    size_t data_len = decoded_len - 256;
    const unsigned char* signature = decoded_buf + data_len;

    // 2. Preparar para verificação
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);

    // Carregar a chave pública
    if (mbedtls_pk_parse_public_key(&pk, (const unsigned char*)PUBLIC_KEY, sizeof(PUBLIC_KEY)) != 0) {
        LV_LOG_ERROR("Falha ao carregar a chave publica.");
        mbedtls_pk_free(&pk);
        return false;
    }

    // Calcular o hash SHA-256 dos dados da licença
    unsigned char hash[32];
    mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), decoded_buf, data_len, hash);

    // 3. Verificar a assinatura
    if (mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, hash, sizeof(hash), signature, 256) != 0) {
        LV_LOG_ERROR("Assinatura da licenca invalida!");
        mbedtls_pk_free(&pk);
        return false;
    }

    LV_LOG_INFO("Assinatura da licenca verificada com sucesso!");
    mbedtls_pk_free(&pk);

    // 4. Se a assinatura é válida, extrair e verificar os dados
    std::string license_data(reinterpret_cast<char*>(decoded_buf), data_len);
    
    uint64_t chipid_raw = ESP.getEfuseMac();
    char chipid_str[18];
    snprintf(chipid_str, sizeof(chipid_str), "%04X%08X", (uint16_t)(chipid_raw >> 32), (uint32_t)chipid_raw);

    // Assumindo formato: HWID:YYYYMMDD:USES
    size_t first_colon = license_data.find(':');
    size_t second_colon = license_data.find(':', first_colon + 1);

    if (first_colon == std::string::npos || second_colon == std::string::npos) return false;

    std::string hwid_from_license = license_data.substr(0, first_colon);
    if (hwid_from_license != chipid_str) {
        LV_LOG_ERROR("ID de Hardware da licenca nao corresponde ao do aparelho.");
        return false; // Em produção, esta verificação é crucial
    }

    std::string uses_str = license_data.substr(second_colon + 1);
    int uses = std::stoi(uses_str);

    // 5. Se tudo estiver correto, guarda os dados
    settingsManager.saveString(LICENSE_KEY, license_data.c_str()); // Guarda os dados, não a chave completa
    settingsManager.saveInt(USES_LEFT_KEY, uses);
    settingsManager.saveInt(USES_SINCE_CHECK_KEY, 0);

    return true;
}

LicenseStatus LicenseManager::checkLicenseStatus() {
    char license_key[64];
    settingsManager.loadString(LICENSE_KEY, license_key, sizeof(license_key));

    if (strlen(license_key) == 0) {
        return LicenseStatus::NOT_ACTIVATED;
    }

    int uses_left = settingsManager.loadInt(USES_LEFT_KEY, 0);
    if (uses_left <= 0) {
        return LicenseStatus::NO_USES_LEFT;
    }

    int uses_since_check = settingsManager.loadInt(USES_SINCE_CHECK_KEY, 0);
    if (uses_since_check >= 30) {
        if (WiFi.status() != WL_CONNECTED) {
            return LicenseStatus::REQUIRES_ONLINE_CHECK;
        }

        // Força a verificação da data online
        WiFiUDP ntpUDP;
        NTPClient timeClient(ntpUDP, "pool.ntp.org");
        timeClient.begin();
        if (!timeClient.update()) {
            return LicenseStatus::REQUIRES_ONLINE_CHECK; // Não conseguiu obter a hora
        }

        time_t now = timeClient.getEpochTime();
        struct tm* timeinfo = localtime(&now);
        
        // Extrair data de validade da chave
        size_t first_colon = std::string(license_key).find(':');
        std::string expiry_str = std::string(license_key).substr(first_colon + 1, 8);
        int expiry_year = std::stoi(expiry_str.substr(0, 4));
        int expiry_month = std::stoi(expiry_str.substr(4, 2));
        int expiry_day = std::stoi(expiry_str.substr(6, 2));

        if ((timeinfo->tm_year + 1900 > expiry_year) ||
            (timeinfo->tm_year + 1900 == expiry_year && timeinfo->tm_mon + 1 > expiry_month) ||
            (timeinfo->tm_year + 1900 == expiry_year && timeinfo->tm_mon + 1 == expiry_month && timeinfo->tm_mday > expiry_day)) {
            return LicenseStatus::EXPIRED;
        }

        // Se a verificação online for bem-sucedida, reinicia a contagem
        settingsManager.saveInt(USES_SINCE_CHECK_KEY, 0);
    }

    return LicenseStatus::VALID;
}

void LicenseManager::recordUsage() {
    int uses_left = settingsManager.loadInt(USES_LEFT_KEY, 0);
    int uses_since_check = settingsManager.loadInt(USES_SINCE_CHECK_KEY, 0);

    if (uses_left > 0) {
        settingsManager.saveInt(USES_LEFT_KEY, uses_left - 1);
        settingsManager.saveInt(USES_SINCE_CHECK_KEY, uses_since_check + 1);
    }
}