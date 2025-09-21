#pragma once
#include "IScreen.h"

class WiFiConfigScreen : public IScreen {
public:
    void create(UIManager* uim) override;
private:
    lv_obj_t* kb = nullptr; // Ponteiro para o teclado
};
#include "WiFiConfigScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "common/SettingsManager.h"

static void textarea_event_handler(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_target(e);
    WiFiConfigScreen* screen = static_cast<WiFiConfigScreen*>(lv_event_get_user_data(e));
    // Aceder ao teclado através de um método público, se necessário, ou passar como user_data
    // lv_keyboard_set_textarea(screen->get_keyboard(), ta);
}

void WiFiConfigScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Configuracao WiFi");

    lv_obj_t* cont = lv_obj_create(screen_obj);
    lv_obj_set_width(cont, lv_pct(90));
    lv_obj_set_height(cont, 200);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 70);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    // Carrega credenciais guardadas
    char ssid[64] = {0};
    char pass[64] = {0};
    settingsManager.loadWiFiCredentials(ssid, pass, sizeof(ssid));

    // Campo para SSID
    lv_obj_t* ssid_label = lv_label_create(cont);
    lv_label_set_text(ssid_label, "Nome da Rede (SSID)");
    lv_obj_t* ssid_ta = lv_textarea_create(cont);
    lv_textarea_set_one_line(ssid_ta, true);
    lv_obj_set_width(ssid_ta, lv_pct(100));
    lv_textarea_set_text(ssid_ta, ssid);

    // Campo para Password
    lv_obj_t* pass_label = lv_label_create(cont);
    lv_label_set_text(pass_label, "Palavra-passe");
    lv_obj_t* pass_ta = lv_textarea_create(cont);
    lv_textarea_set_one_line(pass_ta, true);
    lv_textarea_set_password_mode(pass_ta, true);
    lv_obj_set_width(pass_ta, lv_pct(100));
    lv_textarea_set_text(pass_ta, pass);

    // Teclado
    kb = lv_keyboard_create(screen_obj);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN); // Esconde o teclado inicialmente

    // Eventos para mostrar o teclado
    lv_obj_add_event_cb(ssid_ta, [](lv_event_t* e){
        lv_keyboard_t* kb = (lv_keyboard_t*)lv_event_get_user_data(e);
        lv_keyboard_set_textarea(kb, (lv_obj_t*)lv_event_get_target(e));
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }, LV_EVENT_FOCUSED, kb);

    lv_obj_add_event_cb(pass_ta, [](lv_event_t* e){
        lv_keyboard_t* kb = (lv_keyboard_t*)lv_event_get_user_data(e);
        lv_keyboard_set_textarea(kb, (lv_obj_t*)lv_event_get_target(e));
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }, LV_EVENT_FOCUSED, kb);

    // Evento para guardar e esconder o teclado
    lv_obj_add_event_cb(kb, [ssid_ta, pass_ta](lv_event_t* e){
        lv_keyboard_t* kb = (lv_keyboard_t*)lv_event_get_target(e);
        uint16_t btn_id = lv_keyboard_get_selected_btn(kb);
        if(btn_id == LV_KEYBOARD_BTN_MAP_LEN-1 || btn_id == LV_KEYBOARD_BTN_MAP_LEN-2) { // Ready or Close
            settingsManager.saveWiFiCredentials(lv_textarea_get_text(ssid_ta), lv_textarea_get_text(pass_ta));
            lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_state(lv_keyboard_get_textarea(kb), LV_STATE_FOCUSED);
        }
    }, LV_EVENT_READY, NULL);

    // Botão de voltar
    lv_obj_t *back_btn = lv_btn_create(screen_obj);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_LEFT, 20, -40);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e){
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("Config");
    }, LV_EVENT_CLICKED, ui_manager);

    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT " Voltar");
    lv_obj_center(back_label);
}
