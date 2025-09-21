#include "MainMenuScreen.h"
#include "UIManager.h"
#include "LGFX_setup.h" // Para aceder ao tft
#include <Arduino.h>    // Para aceder às funções do ESP32 como deep sleep
#include "common/UIComponents.h"

void MainMenuScreen::create(UIManager* uim) {
    ui_manager = uim; // Salva o ponteiro do UIManager
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Menu Principal");

    // Container para os botões
    lv_obj_t* btn_container = lv_obj_create(screen_obj);
    lv_obj_set_size(btn_container, 300, 360);
    lv_obj_center(btn_container);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_container, 0, 0);

    // Define os itens do menu no vector membro da classe
    menu_items = {
        {"Diagnostico",       [this](lv_event_t* e) { this->ui_manager->switch_to("VehicleSelect"); }}, // Alterado para ir para a seleção de veículo
        {"Configuracao",      [this](lv_event_t* e) { this->ui_manager->switch_to("Config"); }},
        {"Sobre",             [this](lv_event_t* e) { this->ui_manager->switch_to("About"); }},
        {"Desligar",          [](lv_event_t* event) {
            // Define os botões para a caixa de diálogo
            static const char* btns[] = {"Sim", "Nao", ""};

            // Cria a caixa de mensagem de confirmação
            lv_obj_t * mbox = lv_msgbox_create(NULL, "Desligar", "Tem a certeza que deseja desligar?", btns, true);

            // Adiciona um evento para quando um botão é pressionado
            lv_obj_add_event_cb(mbox, [](lv_event_t* e) {
                lv_obj_t* obj = lv_event_get_current_target(e);
                const char* btn_text = lv_msgbox_get_active_btn_text(obj);

                if (btn_text != NULL && strcmp(btn_text, "Sim") == 0) {
                    // Se "Sim" for pressionado, desliga o aparelho
                    LV_LOG_USER("Shutdown confirmed. Entering deep sleep.");

                    // 1. Apaga o ecrã para uma transição suave
                    tft.setBrightness(0);
                    // 2. Coloca o painel do ecrã em modo de suspensão
                    tft.sleep();
                    // 3. Entra em modo deep sleep. O aparelho só acordará com um reset.
                    esp_deep_sleep_start();
                }
                // Se "Não" for pressionado, a caixa de diálogo fecha-se automaticamente.
            }, LV_EVENT_VALUE_CHANGED, NULL);
            lv_obj_center(mbox);
        }}
    };

    // Cria os botões dinamicamente a partir do vector
    for (auto& item : menu_items) { // Usar referência não-constante
        lv_obj_t* btn = lv_btn_create(btn_container);
        lv_obj_set_size(btn, 250, 50);

        // Adiciona o evento usando uma lambda que captura o ponteiro para o item do menu
        lv_obj_add_event_cb(btn, [](lv_event_t* e) {
            // Recupera o ponteiro para o MenuItem e chama sua função 'action'
            MenuItem* menu_item = static_cast<MenuItem*>(lv_event_get_user_data(e));
            if (menu_item) {
                menu_item->action(e);
            }
        }, LV_EVENT_CLICKED, &item); // Passa o endereço do item como user_data

        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, item.label.c_str());
        lv_obj_center(label);
    }
}
