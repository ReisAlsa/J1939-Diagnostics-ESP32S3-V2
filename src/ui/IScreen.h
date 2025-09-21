#pragma once
#include <lvgl.h>

class UIManager; // Forward declaration para evitar dependência circular

/**
 * @brief Interface (Classe Base Abstrata) para todas as telas da UI.
 * Define um contrato que todas as telas devem seguir.
 */
class IScreen {
public:
    virtual ~IScreen() = default;

    // Método chamado uma vez para criar os objetos LVGL da tela.
    virtual void create(UIManager* uim) = 0;

    // Retorna o ponteiro para o objeto principal da tela (o "screen").
    lv_obj_t* get_screen_obj() { return screen_obj; }

protected:
    lv_obj_t* screen_obj = nullptr;
    UIManager* ui_manager = nullptr;
};
