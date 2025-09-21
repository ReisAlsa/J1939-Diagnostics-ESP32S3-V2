/**
 * @file main.cpp
 * @author Gemini Code Assist
 * @brief Ponto de entrada para a Ferramenta de Diagnóstico J1939.
 * @version 0.2
 * @date 2024-05-21
 *
 * @details Este arquivo é responsável por:
 * - Inicializar o hardware (Display, Touch).
 * - Inicializar as bibliotecas (LVGL).
 * - Instanciar o UIManager.
 * - Registrar todas as telas da aplicação.
 * - Iniciar a UI.
 * - Executar o loop principal.
 */

#define LV_MEM_CUSTOM 1

#include <Arduino.h>
#include <lvgl.h>
#include "LGFX_setup.h"
#include "lvgl_setup.h"
#include "ui/UIManager.h"
#include "common/SettingsManager.h"
#include "common/DatabaseManager.h"
#include "common/CLIManager.h"
#include "common/LicenseManager.h"
#include <esp_heap_caps.h>

#if LV_MEM_CUSTOM
/**
 * @brief Aloca memória para o LVGL especificamente na PSRAM.
 * Esta função é chamada pelo LVGL sempre que precisa de memória.
 */
void *lv_mem_custom_alloc(size_t size) {
    // Tenta alocar na PSRAM. MALLOC_CAP_SPIRAM é a flag para PSRAM.
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

/**
 * @brief Liberta a memória alocada para o LVGL.
 */
void lv_mem_custom_free(void *ptr) {
    heap_caps_free(ptr);
}

/**
 * @brief Realoca um bloco de memória para o LVGL na PSRAM.
 */
void *lv_mem_custom_realloc(void *ptr, size_t new_size) {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}
#endif

UIManager uiManager;

void setup()
{
  Serial.begin(115200);
  // Espera um pouco para a porta serial estabilizar
  delay(1000); 
  Serial.println("\n--- Ferramenta de Diagnostico J1939 a iniciar ---");

        // Verifica se a PSRAM foi encontrada e inicializada com sucesso
    if (psramFound()) {
        LV_LOG_USER("PSRAM OK. Total: %d bytes, Free: %d bytes", ESP.getPsramSize(), ESP.getFreePsram());
    } else {
        LV_LOG_ERROR("PSRAM not found or failed to initialize!");
    }

  // Inicializa o gestor da base de dados
  databaseManager.begin();

  // Inicializa o gestor de configurações
  settingsManager.begin();

  // Inicialização do Hardware e Drivers
  setup_display();
  // Carrega e aplica o brilho guardado
  uint8_t saved_brightness = settingsManager.loadBrightness();
  tft.setBrightness(saved_brightness);

  setup_lvgl_drivers();

  // Inicializa a Interface de Linha de Comandos
  cliManager.begin();

  // Inicializa o gerenciador da UI e cria todas as telas
  uiManager.init();

  // Timer para mudar da splash screen para o ecrã correto após 3 segundos
  lv_timer_create([](lv_timer_t *timer)
                  { 
                    LicenseStatus status = licenseManager.checkLicenseStatus();
                    if (status == LicenseStatus::VALID) {
                        uiManager.switch_to("MainMenu"); 
                    } else {
                        // Se a licença não for válida, força o ecrã de ativação
                        uiManager.switch_to("License");
                    }
                    lv_timer_del(timer); // Deleta o timer após o uso
                  },
                  3000, NULL);

  Serial.println("Setup done.");
}

void loop()
{
  cliManager.handle();  // Processa comandos recebidos via serial
  lv_timer_handler(); // Deixe o LVGL fazer sua mágica
  delay(5);
}
