#pragma once
#include "IScreen.h"
#include <vector>
#include <string>
#include <functional>

class MainMenuScreen : public IScreen {
public:
    void create(UIManager* uim) override;

private:
    // Estrutura para definir um item de menu e sua ação
    struct MenuItem {
        std::string label;
        std::function<void(lv_event_t*)> action;
    };
    // O vector é membro da classe para garantir que as lambdas não percam o escopo
    std::vector<MenuItem> menu_items;
};

