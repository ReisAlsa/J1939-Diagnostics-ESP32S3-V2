#include "AboutScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include <Arduino.h> // Para ESP.getEfuseMac()

void AboutScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Sobre");

    // Container para o texto
    lv_obj_t* cont = lv_obj_create(screen_obj);
    lv_obj_set_size(cont, lv_pct(90), lv_pct(60));
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(cont, 10, 0);

    // Obter ID do Hardware (MAC Address)
    uint64_t chipid = ESP.getEfuseMac();
    char chipid_str[18];
    snprintf(chipid_str, sizeof(chipid_str), "%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);

    // Criar os labels com as informações
    lv_obj_t* hw_id_label = lv_label_create(cont);
    lv_label_set_text_fmt(hw_id_label, "ID Hardware: %s", chipid_str);

    lv_obj_t* res_label = lv_label_create(cont);
    lv_label_set_text(res_label, "Resolucao do ecra: 480x480");

    lv_obj_t* credits_label = lv_label_create(cont);
    lv_label_set_text(credits_label, "Aparelho de diagnostico desenvolvido\ntotalmente por Ricardo Reis\ncom recursos a IA");
    lv_obj_set_style_text_align(credits_label, LV_TEXT_ALIGN_CENTER, 0);

    // Botão de atualização
    lv_obj_t *update_btn = lv_btn_create(cont);
    lv_obj_set_width(update_btn, 200);
    lv_obj_add_event_cb(update_btn, [](lv_event_t* e){
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("Update");
    }, LV_EVENT_CLICKED, ui_manager);
    lv_obj_t *update_label = lv_label_create(update_btn);
    lv_label_set_text(update_label, "Atualizar Firmware");
    lv_obj_center(update_label);

    // Botão de voltar
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