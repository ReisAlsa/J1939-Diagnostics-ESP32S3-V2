#include "DiagnosticsScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"

void DiagnosticsScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Diagnostico");

    lv_obj_t *diag_label = lv_label_create(screen_obj);
    lv_label_set_text(diag_label, "Tela de Diagnostico J1939...");
    lv_obj_center(diag_label);

    lv_obj_t *back_btn = lv_btn_create(screen_obj);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_LEFT, 20, -40);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e){
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("MainMenu");
    }, LV_EVENT_CLICKED, ui_manager);

    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT " Voltar");
    lv_obj_center(back_label);
}
