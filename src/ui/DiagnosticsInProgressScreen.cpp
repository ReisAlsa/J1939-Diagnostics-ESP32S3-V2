#include "DiagnosticsInProgressScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "common/LicenseManager.h"

void DiagnosticsInProgressScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Diagnostico em Curso");

    // --- LÓGICA DE UTILIZAÇÃO ---
    // Ao entrar neste ecrã, consideramos que uma utilização foi gasta.
    licenseManager.recordUsage();
    LV_LOG_INFO("Uma utilizacao de diagnostico registada.");

    // Este label pode ser atualizado com o nome do veículo selecionado
    lv_obj_t *diag_label = lv_label_create(screen_obj);
    lv_label_set_text(diag_label, "A iniciar comunicacao com as centralinas...\n(Aqui entraria a logica J1939)");
    lv_obj_set_style_text_align(diag_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(diag_label);

    lv_obj_t *back_btn = lv_btn_create(screen_obj);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_LEFT, 20, -40);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e){
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("VehicleSelect"); // Volta para a seleção de veículo
    }, LV_EVENT_CLICKED, ui_manager);

    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT " Cancelar");
    lv_obj_center(back_label);
}
