#include "EcuIdScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "DiagnosticsScreen.h" // Para obter a ECU selecionada
#include "core/DiagnosticsData.h"

/**
 * @brief Função auxiliar para criar uma linha de informação (label + valor).
 */
static void create_info_row(lv_obj_t* parent, const char* label_text, const char* value_text) {
    lv_obj_t* row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* label = lv_label_create(row);
    lv_label_set_text(label, label_text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);

    lv_obj_t* value = lv_label_create(row);
    lv_label_set_text(value, value_text);
    lv_obj_set_style_text_color(value, lv_color_hex(0x8A8A8A), 0);
    lv_obj_set_style_text_font(value, &lv_font_montserrat_20, 0);
}

void EcuIdScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);

    const EcuData& selected_ecu = DiagnosticsScreen::get_selected_ecu();
    lv_obj_t* header = create_header(screen_obj, ("Identificação - " + selected_ecu.name).c_str());

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

    // Simula a leitura dos dados e os exibe
    EcuIdData id_data = read_id_from_ecu(selected_ecu.name);

    create_info_row(container, "Número de Peça:", id_data.part_number.c_str());
    create_info_row(container, "Número de Série:", id_data.serial_number.c_str());
    create_info_row(container, "Versão de Software:", id_data.software_version.c_str());
}