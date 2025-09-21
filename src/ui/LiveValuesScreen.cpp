#include "LiveValuesScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "DiagnosticsScreen.h"
#include "core/DiagnosticsData.h"

// Inicialização do membro estático
LiveValueData LiveValuesScreen::selected_value;

const LiveValueData& LiveValuesScreen::get_selected_value() {
    return selected_value;
}

/**
 * @brief Função auxiliar para criar uma linha de valor em tempo real.
 * @param parent O objeto pai onde a linha será criada.
 * @param label_text O texto do rótulo (ex: "Rotação do Motor").
 * @return O ponteiro para o objeto lv_label que exibirá o valor, para que possa ser atualizado.
 */
static lv_obj_t* create_live_value_row(lv_obj_t* parent, const char* label_text) {
    lv_obj_t* row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Adiciona feedback visual ao clicar
    lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(row, lv_color_hex(0xcccccc), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(row, LV_OPA_20, LV_STATE_PRESSED);

    lv_obj_t* label = lv_label_create(row);
    lv_label_set_text(label, label_text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);

    lv_obj_t* value = lv_label_create(row);
    lv_label_set_text(value, "A ler...");
    lv_obj_set_style_text_color(value, lv_color_hex(0x8A8A8A), 0);
    lv_obj_set_style_text_font(value, &lv_font_montserrat_20, 0);

    return value; // Retorna o label do valor para atualização
}

/**
 * @brief Callback do temporizador para atualizar os valores na tela.
 */
static void timer_callback(lv_timer_t* timer) {
    // Recupera o ponteiro para a instância da tela
    LiveValuesScreen* screen = static_cast<LiveValuesScreen*>(timer->user_data);
    std::vector<lv_obj_t*>* labels = (std::vector<lv_obj_t*>*)lv_obj_get_user_data(screen->get_screen_obj());

    const EcuData& selected_ecu = DiagnosticsScreen::get_selected_ecu();
    auto live_values = read_live_values_from_ecu(selected_ecu.name);

    // Atualiza cada label com o novo valor
    for (size_t i = 0; i < live_values.size() && i < labels->size(); ++i) {
        lv_label_set_text_fmt((*labels)[i], "%.2f %s", live_values[i].value, live_values[i].unit.c_str());
    }
}

void LiveValuesScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);

    const EcuData& selected_ecu = DiagnosticsScreen::get_selected_ecu();
    lv_obj_t* header = create_header(screen_obj, ("Valores Atuais - " + selected_ecu.name).c_str());

    lv_obj_t* back_btn = create_back_button(header);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("EcuMenu");
    }, LV_EVENT_CLICKED, ui_manager);

    // Container para as informações
    lv_obj_t* container = lv_obj_create(screen_obj);
    lv_obj_set_size(container, 450, 350);
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 65);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(container, 20, 0);

    // Obtém a lista inicial de parâmetros para criar as linhas
    live_values_data = read_live_values_from_ecu(selected_ecu.name);
    if (live_values_data.empty()) {
        lv_obj_t* label = lv_label_create(container);
        lv_label_set_text(label, "Nenhum valor disponível para esta ECU.");
        lv_obj_center(label);
    } else {
        for (size_t i = 0; i < live_values_data.size(); ++i) {
            lv_obj_t* row = create_live_value_row(container, live_values_data[i].name.c_str());
            // O label do valor é o segundo filho do objeto 'row'
            lv_obj_t* value_label = lv_obj_get_child(row, 1);
            value_labels.push_back(value_label);

            // Adiciona o evento de clique para ir para o gráfico
            lv_obj_set_user_data(row, (void*)i); // Usa o índice como user_data
            lv_obj_add_event_cb(row, [](lv_event_t* e) {
                LiveValuesScreen* screen = static_cast<LiveValuesScreen*>(lv_event_get_user_data(e));
                uintptr_t index = (uintptr_t)lv_obj_get_user_data(lv_event_get_target(e));
                LiveValuesScreen::selected_value = screen->live_values_data[index];
                screen->ui_manager->switch_to("Graph");
            }, LV_EVENT_CLICKED, this);
        }
    }

    // Armazena o vetor de labels no objeto da tela para o timer poder aceder
    lv_obj_set_user_data(screen_obj, &value_labels);

    // Cria um temporizador para atualizar os valores a cada 1 segundo (1000 ms)
    update_timer = lv_timer_create(timer_callback, 1000, this);

    // Adiciona um evento para apagar o temporizador quando a tela for destruída
    lv_obj_add_event_cb(screen_obj, [](lv_event_t* e) {
        LiveValuesScreen* screen = static_cast<LiveValuesScreen*>(lv_event_get_user_data(e));
        if (screen->update_timer) {
            lv_timer_del(screen->update_timer);
            screen->update_timer = nullptr;
        }
    }, LV_EVENT_DELETE, this);
}