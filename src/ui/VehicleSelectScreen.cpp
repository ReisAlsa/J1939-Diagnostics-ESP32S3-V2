#include "VehicleSelectScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "common/DatabaseManager.h"

void VehicleSelectScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Selecionar Veiculo");

    lv_obj_t *list = lv_list_create(screen_obj);
    lv_obj_set_size(list, 380, 350);
    lv_obj_center(list);

    lv_list_add_text(list, "Selecione o Veiculo");

    // Carrega a lista de veículos da "base de dados"
    auto vehicles = databaseManager.getVehicleList();
    for (const auto& vehicleName : vehicles) {
        lv_obj_t* btn = lv_list_add_btn(list, LV_SYMBOL_DRIVE, vehicleName.c_str());
        lv_obj_add_event_cb(btn, [](lv_event_t* e) {
            UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
            const char* selectedVehicle = lv_list_get_btn_text(lv_obj_get_parent(lv_event_get_target(e)), lv_event_get_target(e));
            
            // Aqui pode passar o nome do veículo para o próximo ecrã ou para um gestor de estado
            LV_LOG_USER("Veiculo selecionado: %s", selectedVehicle);

            // Navega para o ecrã de diagnóstico em curso
            uim->switch_to("DiagnosticsInProgress");

        }, LV_EVENT_CLICKED, ui_manager);
    }

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
