#ifndef DIAGNOSTICS_SCREEN_H
#define DIAGNOSTICS_SCREEN_H

#include "IScreen.h"
#include <vector>
#include <string>

// Estrutura para armazenar os dados de uma centralina (ECU)
struct EcuData {
    std::string name;
    int active_errors;
    int stored_errors;
};

class DiagnosticsScreen : public IScreen {
public:
    void create(UIManager* uim) override;

    // Método estático para obter a ECU que foi selecionada
    static const EcuData& get_selected_ecu();
    // Método estático para obter a lista completa de ECUs detetadas
    static const std::vector<EcuData>& get_detected_ecus();

private:
    void add_ecu_to_list(const EcuData& ecu);
    lv_obj_t* ecu_list;
    static std::vector<EcuData> detected_ecus; // Partilhado com outros ecrãs
    static EcuData selected_ecu; // Armazena temporariamente a ECU clicada
};

#endif // DIAGNOSTICS_SCREEN_H
