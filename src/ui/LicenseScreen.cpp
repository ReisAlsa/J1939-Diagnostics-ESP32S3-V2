#include "LicenseScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "common/LicenseManager.h"

void LicenseScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Ativacao de Licenca");

    lv_obj_t* cont = lv_obj_create(screen_obj);
    lv_obj_set_width(cont, lv_pct(90));
    lv_obj_set_height(cont, 250);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    status_label = lv_label_create(cont);
    lv_obj_set_width(status_label, lv_pct(100));
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);

    // Mostra o estado atual da licença
    LicenseStatus status = licenseManager.checkLicenseStatus();
    switch (status) {
        case LicenseStatus::NOT_ACTIVATED:
            lv_label_set_text(status_label, "Aparelho nao ativado.\nPor favor, insira a sua chave de licenca.");
            break;
        case LicenseStatus::EXPIRED:
            lv_label_set_text(status_label, "A sua licenca expirou.");
            break;
        case LicenseStatus::NO_USES_LEFT:
            lv_label_set_text(status_label, "Numero de utilizacoes esgotado.");
            break;
        case LicenseStatus::REQUIRES_ONLINE_CHECK:
            lv_label_set_text(status_label, "E necessario ligar a internet para\nverificar a validade da licenca.");
            break;
        default:
             lv_label_set_text(status_label, "Erro de licenca desconhecido.");
            break;
    }

    // Campo para a chave de licença
    lv_obj_t* key_ta = lv_textarea_create(cont);
    lv_textarea_set_one_line(key_ta, true);
    lv_obj_set_width(key_ta, lv_pct(100));
    lv_textarea_set_placeholder_text(key_ta, "XXXX-XXXX-XXXX");

    // Teclado
    kb = lv_keyboard_create(screen_obj);
    lv_keyboard_set_textarea(kb, key_ta);

    // Botão de Ativar
    lv_obj_t* activate_btn = lv_btn_create(cont);
    lv_obj_set_width(activate_btn, 200);
    lv_obj_t* activate_label = lv_label_create(activate_btn);
    lv_label_set_text(activate_label, "Ativar");
    lv_obj_center(activate_label);

    lv_obj_add_event_cb(activate_btn, [this, key_ta](lv_event_t* e) {
        const char* key = lv_textarea_get_text(key_ta);
        if (licenseManager.validateAndStoreLicense(key)) {
            lv_label_set_text(status_label, "Licenca ativada com sucesso!\nA reiniciar...");
            lv_timer_create([](lv_timer_t* t){ ESP.restart(); }, 2000, NULL);
        } else {
            lv_label_set_text(status_label, "Chave de licenca invalida. Tente novamente.");
        }
    }, LV_EVENT_CLICKED, NULL);

    // Se o problema for apenas a falta de internet, mostra um botão para ir às configurações
    if (status == LicenseStatus::REQUIRES_ONLINE_CHECK) {
        lv_obj_t* wifi_btn = lv_btn_create(cont);
        lv_obj_set_width(wifi_btn, 200);
        lv_obj_t* wifi_label = lv_label_create(wifi_btn);
        lv_label_set_text(wifi_label, "Configurar WiFi");
        lv_obj_center(wifi_label);
        lv_obj_add_event_cb(wifi_btn, [this](lv_event_t* e){
            this->ui_manager->switch_to("WiFiConfig");
        }, LV_EVENT_CLICKED, NULL);
    }
}