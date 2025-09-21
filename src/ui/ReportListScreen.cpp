#include "ReportListScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include <LittleFS.h>

// Inicialização do membro estático
std::string ReportListScreen::selected_report_filename;

const std::string& ReportListScreen::get_selected_report_filename() {
    return selected_report_filename;
}

void ReportListScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);

    lv_obj_t* header = create_header(screen_obj, "Relatórios Guardados");

    // Botão de voltar
    lv_obj_t* back_btn = create_back_button(header);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("MainMenu"); // Volta para o menu principal
    }, LV_EVENT_CLICKED, ui_manager);

    // Lista para os relatórios
    lv_obj_t* list = lv_list_create(screen_obj);
    lv_obj_set_size(list, 460, 350);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 65);

    // Itera sobre os ficheiros no LittleFS
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    bool reports_found = false;

    while(file){
        std::string filename = file.name();
        // Filtra apenas os ficheiros de relatório (ex: /report_*.txt)
        if (filename.rfind("/report_", 0) == 0 && filename.find(".txt") != std::string::npos) {
            reports_found = true;
            lv_obj_t* btn = lv_list_add_btn(list, LV_SYMBOL_FILE, filename.c_str());

            lv_obj_add_event_cb(btn, [](lv_event_t* e) {
                UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
                lv_obj_t* target_btn = lv_event_get_target(e);
                // Obtém o nome do ficheiro a partir do texto do botão
                const char* filename_c_str = lv_list_get_btn_text(lv_obj_get_parent(target_btn), target_btn);

                ReportListScreen::selected_report_filename = filename_c_str;
                uim->switch_to("ReportViewer");
            }, LV_EVENT_CLICKED, ui_manager);
        }
        file = root.openNextFile();
    }
    root.close();

    if (!reports_found) {
        lv_obj_t* label = lv_label_create(list);
        lv_label_set_text(label, "Nenhum relatório encontrado.");
        lv_obj_center(label);
    }
}