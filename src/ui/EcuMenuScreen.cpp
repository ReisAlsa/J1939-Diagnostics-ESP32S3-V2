#include "EcuMenuScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "DiagnosticsScreen.h" // Para obter a ECU selecionada
#include "core/DiagnosticsData.h"

void EcuMenuScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);

    // Obtém os dados da ECU que foi selecionada no ecrã anterior
    const EcuData& selected_ecu = DiagnosticsScreen::get_selected_ecu();

    // Cabeçalho com o nome da ECU
    lv_obj_t* header = create_header(screen_obj, selected_ecu.name.c_str());

    // Botão de voltar no cabeçalho
    lv_obj_t* back_btn = create_back_button(header);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("Diagnostics"); // Volta para a lista de centralinas
    }, LV_EVENT_CLICKED, ui_manager);

    // Container para a lista de opções
    lv_obj_t* list = lv_list_create(screen_obj);
    lv_obj_set_size(list, 450, 350);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 65);

    // Adiciona os botões de opção
    lv_list_add_btn(list, LV_SYMBOL_FILE, "Identificação");
    lv_list_add_btn(list, LV_SYMBOL_EYE_OPEN, "Ler Erros");
    lv_list_add_btn(list, LV_SYMBOL_TRASH, "Apagar Erros");
    lv_list_add_btn(list, LV_SYMBOL_SETTINGS, "Valores Atuais");

    // Adiciona um evento genérico para os botões da lista (placeholder)
    lv_obj_add_event_cb(list, [](lv_event_t* e) {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t* obj = lv_event_get_target(e);
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));

        if (code == LV_EVENT_CLICKED) {
            const char* option_text = lv_list_get_btn_text(lv_obj_get_parent(obj), obj);
            LV_LOG_USER("Opção clicada: %s", option_text);

            if (strcmp(option_text, "Ler Erros") == 0) {
                uim->switch_to("ErrorList");
            } else if (strcmp(option_text, "Apagar Erros") == 0) {
                // Cria uma caixa de diálogo para confirmação
                static const char* btns[] = {"Sim", "Nao", ""};
                lv_obj_t* mbox = lv_msgbox_create(NULL, "Apagar Erros", "Tem a certeza que deseja apagar os erros memorizados?", btns, true);
                
                // Adiciona um evento para quando um botão da msgbox é pressionado
                lv_obj_add_event_cb(mbox, [](lv_event_t* e_mbox) {
                    lv_obj_t* obj = lv_event_get_current_target(e_mbox);
                    const char* btn_text = lv_msgbox_get_active_btn_text(obj);

                    if (btn_text != NULL && strcmp(btn_text, "Sim") == 0) {
                        // Se "Sim" for pressionado, chama a função para apagar os erros
                        const EcuData& ecu = DiagnosticsScreen::get_selected_ecu();
                        if (clear_dtcs_from_ecu(ecu.name)) {
                            create_info_msgbox(lv_scr_act(), "Sucesso", "Comando para apagar erros enviado.");
                        } else {
                            create_info_msgbox(lv_scr_act(), "Erro", "Falha ao enviar o comando.");
                        }
                    }
                }, LV_EVENT_VALUE_CHANGED, NULL);
                lv_obj_center(mbox);
            } else if (strcmp(option_text, "Identificação") == 0) {
                uim->switch_to("EcuId");
            } else if (strcmp(option_text, "Valores Atuais") == 0) {
                uim->switch_to("LiveValues");
            }
        }
    }, LV_EVENT_CLICKED, ui_manager);
}