#pragma once

#include <functional>

class UpdateManager {
public:
    // Callback para reportar o progresso: void(int percentagem, const char* mensagem)
    using ProgressCallback = std::function<void(int, const char*)>;

    void performSdUpdate(const char* filename, ProgressCallback callback);
    void performOtaUpdate(const char* url, ProgressCallback callback);
};

extern UpdateManager updateManager;
