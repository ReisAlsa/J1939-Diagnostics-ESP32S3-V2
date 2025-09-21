#pragma once
#include "IScreen.h"

class UpdateScreen : public IScreen {
public:
    void create(UIManager* uim) override;

    // Método público para a tarefa de fundo poder atualizar a UI
    void update_progress(int percentage, const char* message);

private:
    // Elementos da UI para feedback
    lv_obj_t* status_label = nullptr;
    lv_obj_t* progress_bar = nullptr;
    lv_obj_t* sd_btn = nullptr;
    lv_obj_t* ota_btn = nullptr;
};
