#pragma once

#include <string>

enum class LicenseStatus {
    VALID,
    INVALID,
    EXPIRED,
    NO_USES_LEFT,
    REQUIRES_ONLINE_CHECK,
    NOT_ACTIVATED
};

class LicenseManager {
public:
    void begin();

    // Valida e guarda uma nova chave de licença
    bool validateAndStoreLicense(const std::string& key);

    // Verifica o estado atual da licença guardada
    LicenseStatus checkLicenseStatus();

    // Decrementa o número de utilizações
    void recordUsage();
};

extern LicenseManager licenseManager;