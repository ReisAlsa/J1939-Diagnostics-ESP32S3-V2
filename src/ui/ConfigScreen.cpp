#include "ConfigScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "LGFX_setup.h" // Para aceder ao tft
#include "common/SettingsManager.h"

void ConfigScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Menu de Configuracao");

    // Container para os controlos
    lv_obj_t* cont = lv_obj_create(screen_obj);
    lv_obj_set_size(cont, 400, 300);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(cont, 30, 0); // Aumenta o espaço entre os itens

    // --- Secção de Brilho ---
    
    // Label para o slider de brilho
    lv_obj_t* brightness_label = lv_label_create(cont);
    lv_label_set_text(brightness_label, "Ajuste de Brilho");
    lv_obj_set_style_text_font(brightness_label, &lv_font_montserrat_18, 0);

    // Slider de brilho
    lv_obj_t* brightness_slider = lv_slider_create(cont);
    lv_obj_set_width(brightness_slider, 300);
    lv_slider_set_range(brightness_slider, 10, 255); // Mínimo 10 para não desligar o ecrã
    lv_slider_set_value(brightness_slider, tft.getBrightness(), LV_ANIM_OFF);

    // Evento para quando o valor do slider muda
    lv_obj_add_event_cb(brightness_slider, [](lv_event_t* e) {
        lv_obj_t* slider = lv_event_get_target(e);
        int32_t value = lv_slider_get_value(slider);
        tft.setBrightness(value);
        // Guarda o valor na memória não-volátil.
        // Nota: Para evitar escritas excessivas na flash, uma estratégia
        // otimizada seria guardar o valor apenas quando o utilizador sai do menu.
        settingsManager.saveBrightness(value);
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // --- Secção de Som ---

    // Container para a linha do som (label + switch)
    lv_obj_t* sound_cont = lv_obj_create(cont);
    lv_obj_set_width(sound_cont, 300); // Mesma largura do slider para alinhar
    lv_obj_set_height(sound_cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(sound_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(sound_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_style(sound_cont, NULL, LV_PART_ANY | LV_STATE_ANY); // Remove estilos do container

    // Label para o som
    lv_obj_t* sound_label = lv_label_create(sound_cont);
    lv_label_set_text(sound_label, "Som");
    lv_obj_set_style_text_font(sound_label, &lv_font_montserrat_18, 0);

    // Switch para o som
    lv_obj_t* sound_switch = lv_switch_create(sound_cont);
    if (settingsManager.loadSoundState(true)) { // Por defeito, o som está ligado
        lv_obj_add_state(sound_switch, LV_STATE_CHECKED);
    }

    // Evento para quando o switch muda de estado
    lv_obj_add_event_cb(sound_switch, [](lv_event_t* e) {
        lv_obj_t* sw = lv_event_get_target(e);
        bool is_enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
        settingsManager.saveSoundState(is_enabled);
        // TODO: Adicionar aqui a lógica para ativar/desativar o som (ex: controlar um buzzer)
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // --- Secção de WiFi ---
    lv_obj_t* wifi_btn = lv_btn_create(cont);
    lv_obj_set_width(wifi_btn, 300);
    lv_obj_add_event_cb(wifi_btn, [](lv_event_t* e){
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("WiFiConfig");
    }, LV_EVENT_CLICKED, ui_manager);

    lv_obj_t* wifi_label = lv_label_create(wifi_btn);
    lv_label_set_text(wifi_label, "Configurar WiFi");
    lv_obj_center(wifi_label);

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
