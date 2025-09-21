#pragma once

#include <lvgl.h>
#include <map>
#include <string>
#include <memory>
#include "IScreen.h"

/**
 * @brief Gerencia todas as telas da UI, suas criações e transições.
 */
class UIManager {
public:
    // Inicializa o UIManager, registra e cria todas as telas.
    void init();

    // Muda para a tela especificada pelo nome.
    void switch_to(const std::string& name);

private:
    // Adiciona uma tela ao gerenciador.
    void add_screen(const std::string& name, std::unique_ptr<IScreen> screen);

    // Itera sobre todas as telas registradas e chama seus métodos `create()`.
    void create_all_screens();

    std::map<std::string, std::unique_ptr<IScreen>> screens;
    IScreen* current_screen = nullptr;
};
