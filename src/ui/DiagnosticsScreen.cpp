#include "DiagnosticsScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"

// Inicialização dos membros estáticos
EcuData DiagnosticsScreen::selected_ecu;
std::vector<EcuData> DiagnosticsScreen::detected_ecus;

const EcuData& DiagnosticsScreen::get_selected_ecu() {
    return selected_ecu;
}

const std::vector<EcuData>& DiagnosticsScreen::get_detected_ecus() {
    return detected_ecus;
}

void DiagnosticsScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    lv_obj_t* header = create_header(screen_obj, "Diagnóstico - Centralinas");
    lv_obj_t* footer = create_footer(screen_obj);

    // Botão de voltar no cabeçalho
    lv_obj_t* back_btn = create_back_button(header);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("VehicleSelect"); // Volta para a seleção de veículo
    }, LV_EVENT_CLICKED, ui_manager);

    // Botão "Criar Relatório" no rodapé
    lv_obj_t* report_btn = create_footer_button(footer, "Criar Relatório", LV_SYMBOL_FILE);
    lv_obj_add_event_cb(report_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("Report");
    }, LV_EVENT_CLICKED, ui_manager);

    // Cria a lista para as centralinas
    ecu_list = lv_list_create(screen_obj);
    lv_obj_set_size(ecu_list, 450, 350);
    lv_obj_align(ecu_list, LV_ALIGN_TOP_MID, 0, 65);

    // --- DADOS DE EXEMPLO ---
    // No seu código real, esta lista virá da comunicação com o veículo.
    detected_ecus = {
        {"Motor (EDC)", 2, 5},
        {"Travões (ABS)", 0, 3},
        {"Transmissão", 0, 0},
        {"Painel Instrumentos", 1, 1},
        {"Tacógrafo", 0, 0},
        {"Suspensão (ECAS)", 0, 1}
    };

    // Adiciona cada centralina detectada à lista na UI
    for (const auto& ecu : detected_ecus) {
        add_ecu_to_list(ecu);
    }
}

void DiagnosticsScreen::add_ecu_to_list(const EcuData& ecu) {
    // Adiciona um novo botão à lista para representar a centralina
    lv_obj_t* btn = lv_list_add_btn(ecu_list, NULL, ecu.name.c_str());
    lv_obj_add_event_cb(btn, [](lv_event_t* e) {
        // Obtém os dados do evento
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        const EcuData* ecu_data = static_cast<const EcuData*>(lv_obj_get_user_data(lv_event_get_target(e)));

        // Guarda a ECU selecionada e muda de ecrã
        DiagnosticsScreen::selected_ecu = *ecu_data;
        uim->switch_to("EcuMenu");
    }, LV_EVENT_CLICKED, ui_manager);
    lv_obj_set_user_data(btn, (void*)&ecu); // Associa os dados da ECU ao botão
    lv_obj_set_style_pad_right(btn, 10, 0); // Espaço para os ícones

    // Container para alinhar os indicadores à direita
    lv_obj_t* indicators_container = lv_obj_create(btn);
    lv_obj_remove_style_all(indicators_container); // Remove estilos padrão
    lv_obj_set_size(indicators_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(indicators_container, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_layout(indicators_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(indicators_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(indicators_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_gap(indicators_container, 10, 0);

    int total_errors = ecu.active_errors + ecu.stored_errors;

    // Se não houver erros, não adiciona indicadores
    if (total_errors == 0) {
        return;
    }

    // Label para a contagem de erros
    lv_obj_t* count_label = lv_label_create(indicators_container);
    lv_label_set_text_fmt(count_label, "(%d)", total_errors);
    lv_obj_set_style_text_color(count_label, lv_color_hex(0x8A8A8A), 0); // Cinza

    // Label para o ícone de estado
    lv_obj_t* icon_label = lv_label_create(indicators_container);

    // Define o ícone e a cor com base no tipo de erro
    if (ecu.active_errors > 0) {
        // Erros Ativos -> Ícone vermelho
        lv_label_set_text(icon_label, LV_SYMBOL_BELL); // Ícone de sino/alerta
        lv_obj_set_style_text_color(icon_label, lv_color_hex(0xFF0000), 0); // Vermelho
    } else { // Apenas erros memorizados (stored_errors > 0)
        // Erros Memorizados -> Ícone laranja
        lv_label_set_text(icon_label, LV_SYMBOL_WARNING); // Ícone de aviso
        lv_obj_set_style_text_color(icon_label, lv_color_hex(0xFFA500), 0); // Laranja
    }
}
