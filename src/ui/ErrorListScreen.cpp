#include "ErrorListScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "DiagnosticsScreen.h" // Para obter a ECU selecionada
#include "core/DiagnosticsData.h"
#include <sstream>

/**
 * @brief Simula a leitura de DTCs de uma ECU.
 * No futuro, esta função será substituída pela lógica de comunicação J1939.
 * @param ecu_name O nome da ECU para "ler" os erros.
 * @return Um vetor de DtcData com os erros encontrados.
 */
std::vector<DtcData> read_dtcs_from_ecu(const std::string& ecu_name) {
    // Lógica de exemplo baseada no nome da ECU
    if (ecu_name.find("Motor") != std::string::npos) {
        return {
            {true, 100, 1, "Pressão do Óleo Baixa"},
            {true, 110, 3, "Temperatura do Motor Alta"},
            {false, 91, 8, "Posição do Acelerador Inválida"},
            {false, 629, 12, "Falha Interna da ECU"},
            {false, 102, 2, "Pressão do Turbo Inconsistente"},
        };
    }
    if (ecu_name.find("Travões") != std::string::npos) {
        return {
            {false, 70, 2, "Sinal do Sensor de Velocidade da Roda Dianteira Esquerda"},
            {false, 71, 2, "Sinal do Sensor de Velocidade da Roda Dianteira Direita"},
        };
    }
    // Retorna vazio se não houver erros ou a ECU não estiver na simulação
    return {};
}

/**
 * @brief Simula o envio do comando para apagar os DTCs de uma ECU.
 * No futuro, esta função será substituída pela lógica de comunicação J1939 (DM3).
 * @param ecu_name O nome da ECU para "apagar" os erros.
 * @return true se o comando foi "enviado" com sucesso.
 */
bool clear_dtcs_from_ecu(const std::string& ecu_name) {
    LV_LOG_USER("Comando para apagar erros (DM3) enviado para a ECU: %s", ecu_name.c_str());
    // Em uma implementação real, você enviaria a mensagem J1939 DM3
    // e esperaria pela confirmação. Por agora, vamos sempre retornar sucesso.
    return true;
}

/**
 * @brief Simula a leitura dos dados de identificação de uma ECU.
 * No futuro, esta função será substituída pela lógica de comunicação J1939 (ex: DM19).
 * @param ecu_name O nome da ECU para "ler" os dados.
 * @return Uma estrutura EcuIdData com os dados encontrados.
 */
EcuIdData read_id_from_ecu(const std::string& ecu_name) {
    if (ecu_name.find("Motor") != std::string::npos) {
        return {"A0014468840", "5210012345", "22.1A"};
    }
    if (ecu_name.find("Travões") != std::string::npos) {
        return {"A0054461234", "8870054321", "10.5C"};
    }
    // Retorna dados genéricos se a ECU não estiver na simulação
    return {"N/A", "N/A", "N/A"};
}

/**
 * @brief Simula a leitura de valores em tempo real de uma ECU.
 * No futuro, esta função será substituída pela lógica de comunicação J1939 (ex: DM1).
 * @param ecu_name O nome da ECU para "ler" os dados.
 * @return Um vetor de LiveValueData com os dados encontrados.
 */
std::vector<LiveValueData> read_live_values_from_ecu(const std::string& ecu_name) {
    if (ecu_name.find("Motor") != std::string::npos) {
        // Gera valores aleatórios para simular dados em tempo real
        return {
            {"Rotação do Motor", 800.0f + (esp_random() % 100), "RPM"},
            {"Temperatura do Motor", 85.0f + (esp_random() % 5), "°C"},
            {"Posição do Acelerador", 15.0f + (esp_random() % 10), "%"},
            {"Pressão do Turbo", 1.1f + (float)(esp_random() % 100) / 1000.0f, "bar"},
        };
    }
    return {}; // Retorna vazio para outras ECUs
}

void ErrorListScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);

    const EcuData& selected_ecu = DiagnosticsScreen::get_selected_ecu();
    lv_obj_t* header = create_header(screen_obj, ("Erros - " + selected_ecu.name).c_str());

    lv_obj_t* back_btn = create_back_button(header);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("EcuMenu");
    }, LV_EVENT_CLICKED, ui_manager);

    lv_obj_t* list = lv_list_create(screen_obj);
    lv_obj_set_size(list, 460, 350);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 65);

    // Simula a leitura dos erros
    auto dtcs = read_dtcs_from_ecu(selected_ecu.name);

    if (dtcs.empty()) {
        lv_obj_t* label = lv_label_create(list);
        lv_label_set_text(label, "Nenhum código de erro encontrado.");
        lv_obj_center(label);
        return;
    }

    // Adiciona os erros à lista
    for (const auto& dtc : dtcs) {
        std::stringstream ss;
        ss << "SPN " << dtc.spn << ", FMI " << dtc.fmi << ": " << dtc.description;

        const char* icon = dtc.is_active ? LV_SYMBOL_BELL : LV_SYMBOL_WARNING;
        lv_obj_t* btn = lv_list_add_btn(list, icon, ss.str().c_str());
        lv_obj_set_style_text_color(lv_obj_get_child(btn, 0), dtc.is_active ? lv_color_hex(0xFF0000) : lv_color_hex(0xFFA500), 0);
    }
}