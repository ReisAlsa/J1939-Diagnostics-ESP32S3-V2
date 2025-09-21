#include "UpdateScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "common/UpdateManager.h"

// Tarefa que executa a atualização em segundo plano para não bloquear a UI
void sd_update_task(void* param) {
    UpdateScreen* screen = static_cast<UpdateScreen*>(param);
    updateManager.performSdUpdate("/firmware.bin", [screen](int p, const char* m) { screen->update_progress(p, m); });
    vTaskDelete(NULL); // Elimina a tarefa quando terminar
}

// Tarefa que executa a atualização OTA em segundo plano
void ota_update_task(void* param) {
    UpdateScreen* screen = static_cast<UpdateScreen*>(param);
    // NOTA: Substitua este URL pelo URL real do seu ficheiro de firmware
    updateManager.performOtaUpdate("http://example.com/path/to/your/firmware.bin", [screen](int p, const char* m) { screen->update_progress(p, m); });
    vTaskDelete(NULL);
}

void UpdateScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    create_header_footer(screen_obj, "Atualizacao de Firmware");

    lv_obj_t* cont = lv_obj_create(screen_obj);
    lv_obj_set_size(cont, 400, 300);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    sd_btn = lv_btn_create(cont);
    lv_obj_set_size(sd_btn, 300, 60);
    lv_obj_t* sd_label = lv_label_create(sd_btn);
    lv_label_set_text(sd_label, "Atualizar via Cartao SD");
    lv_obj_center(sd_label);
    // Adiciona o evento para iniciar a atualização
    lv_obj_add_event_cb(sd_btn, [](lv_event_t* e) {
        UpdateScreen* screen = static_cast<UpdateScreen*>(lv_event_get_user_data(e));
        // Esconde os botões e mostra os elementos de progresso
        lv_obj_add_flag(screen->sd_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(screen->ota_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(screen->status_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(screen->progress_bar, LV_OBJ_FLAG_HIDDEN);
        // Inicia a tarefa de atualização
        xTaskCreate(sd_update_task, "sd_update_task", 8192, screen, 5, NULL);
    }, LV_EVENT_CLICKED, this);

    ota_btn = lv_btn_create(cont);
    lv_obj_set_size(ota_btn, 300, 60);
    lv_obj_t* ota_label = lv_label_create(ota_btn);
    lv_label_set_text(ota_label, "Atualizar via OTA (Internet)");
    lv_obj_center(ota_label);
    lv_obj_add_event_cb(ota_btn, [](lv_event_t* e) {
        UpdateScreen* screen = static_cast<UpdateScreen*>(lv_event_get_user_data(e));
        // Esconde os botões e mostra os elementos de progresso
        lv_obj_add_flag(screen->sd_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(screen->ota_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(screen->status_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(screen->progress_bar, LV_OBJ_FLAG_HIDDEN);
        // Inicia a tarefa de atualização OTA
        xTaskCreate(ota_update_task, "ota_update_task", 10240, screen, 5, NULL);
    }, LV_EVENT_CLICKED, this);

    // Elementos de feedback (inicialmente escondidos)
    status_label = lv_label_create(cont);
    lv_label_set_text(status_label, "A iniciar...");
    lv_obj_add_flag(status_label, LV_OBJ_FLAG_HIDDEN);

    progress_bar = lv_bar_create(cont);
    lv_obj_set_size(progress_bar, 300, 20);
    lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);
    lv_obj_add_flag(progress_bar, LV_OBJ_FLAG_HIDDEN);


    lv_obj_t *back_btn = lv_btn_create(screen_obj);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_LEFT, 20, -40);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e){
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("About");
    }, LV_EVENT_CLICKED, ui_manager);

    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT " Voltar");
    lv_obj_center(back_label);
}

void UpdateScreen::update_progress(int percentage, const char* message) {
    // Esta função pode ser chamada de outra tarefa, mas LVGL não é 100% thread-safe.
    // Para este caso, a atualização de widgets simples é geralmente segura.
    // Para aplicações mais complexas, seria necessário usar mutexes ou lv_async_call.
    lv_label_set_text(status_label, message);
    if (percentage >= 0) {
        lv_bar_set_value(progress_bar, percentage, LV_ANIM_ON);
    }
}
