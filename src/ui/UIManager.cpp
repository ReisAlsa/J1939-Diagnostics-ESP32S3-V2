#include "UIManager.h"
#include "SplashScreen.h"
#include "MainMenuScreen.h"
#include "DiagnosticsScreen.h"
#include "ConfigScreen.h"
#include "AboutScreen.h"
#include "UpdateScreen.h"
#include "WiFiConfigScreen.h"
#include "DiagnosticsInProgressScreen.h"
#include "LicenseScreen.h"
#include "VehicleSelectScreen.h"

void UIManager::init() {
    // Registra todas as telas da aplicação
    add_screen("Splash", std::make_unique<SplashScreen>());
    add_screen("MainMenu", std::make_unique<MainMenuScreen>());
    add_screen("Diagnostics", std::make_unique<DiagnosticsScreen>());
    add_screen("Config", std::make_unique<ConfigScreen>());
    add_screen("About", std::make_unique<AboutScreen>());
    add_screen("Update", std::make_unique<UpdateScreen>());
    add_screen("WiFiConfig", std::make_unique<WiFiConfigScreen>());
    add_screen("DiagnosticsInProgress", std::make_unique<DiagnosticsInProgressScreen>());
    add_screen("License", std::make_unique<LicenseScreen>());
    add_screen("VehicleSelect", std::make_unique<VehicleSelectScreen>());

    // Manda criar os objetos LVGL de todas as telas
    create_all_screens();

    // Define a tela inicial
    switch_to("Splash");
}

void UIManager::add_screen(const std::string& name, std::unique_ptr<IScreen> screen) {
    // Adiciona a tela ao map
    screens[name] = std::move(screen);
}

void UIManager::create_all_screens() {
    // Para cada par <nome, ponteiro_tela> no map...
    for (auto const& [name, screen] : screens) {
        // ...chama o método create() da tela
        screen->create(this);
    }
}

void UIManager::switch_to(const std::string& name) {
    // Procura a tela pelo nome
    auto it = screens.find(name);
    if (it != screens.end()) {
        // Se encontrou, define como a tela atual
        current_screen = it->second.get();
        if (current_screen && current_screen->get_screen_obj()) {
            // Carrega a tela no LVGL com uma animação de fade
            lv_scr_load_anim(current_screen->get_screen_obj(), LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
        }
    }
}
