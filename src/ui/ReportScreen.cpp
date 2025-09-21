#include "ReportScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "DiagnosticsScreen.h" // Para obter os dados das ECUs
#include "core/DiagnosticsData.h" // Para obter os DTCs detalhados
#include <LittleFS.h>
#include <sstream> // Para construir a string do relatório facilmente
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

/**
 * @brief Obtém a data e hora atuais de um servidor NTP.
 * Requer uma ligação WiFi ativa.
 * @return Uma string formatada com a data/hora (YYYY-MM-DD HH:MM:SS) ou uma mensagem de erro.
 */
std::string ReportScreen::get_current_datetime() {
    if (WiFi.status() != WL_CONNECTED) {
        return "Indisponivel (sem WiFi)";
    }

    WiFiUDP ntpUDP;
    // O terceiro argumento é o offset em segundos (ex: para UTC-1/Lisboa seria -3600 no verão)
    NTPClient timeClient(ntpUDP, "pool.ntp.org", 0);

    timeClient.begin();
    if (!timeClient.update()) {
        timeClient.end();
        return "Falha ao sincronizar NTP";
    }

    // getEpochTime() retorna o tempo em segundos desde 1 de Janeiro de 1970
    time_t epochTime = timeClient.getEpochTime();
    timeClient.end();

    char buffer[20];
    // Formata a data e hora para o formato YYYY-MM-DD HH:MM:SS
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&epochTime));
    return std::string(buffer);
}

std::string ReportScreen::generate_report_string() {
    std::stringstream report_stream;

    // --- Cabeçalho do Relatório ---
    // TODO: Obter dados reais (VIN, data/hora de um RTC, etc.)
    long report_id = esp_random(); // Gera um ID único simples para o relatório

    report_stream << "========================================\n";
    report_stream << "      RELATÓRIO DE DIAGNÓSTICO\n";
    report_stream << "========================================\n\n";
    report_stream << "VIN: WDB9066551N52XXXX\n"; // Placeholder
    report_stream << "Data: " << get_current_datetime() << "\n";
    report_stream << "ID do Relatório: " << report_id << "\n\n";
    report_stream << "----------------------------------------\n";
    report_stream << "          CENTRALINAS DETETADAS\n";
    report_stream << "----------------------------------------\n\n";

    // --- Lista de Centralinas e Erros ---
    const auto& ecus = DiagnosticsScreen::get_detected_ecus();
    if (ecus.empty()) {
        report_stream << "Nenhuma centralina com erros foi detetada.\n";
    } else {
        bool has_errors_in_any_ecu = false;
        for (const auto& ecu : ecus) {
            if (ecu.active_errors > 0 || ecu.stored_errors > 0) {
                has_errors_in_any_ecu = true;
                report_stream << "Centralina: " << ecu.name << "\n";

                // Obtém a lista detalhada de erros para esta ECU
                auto dtcs = read_dtcs_from_ecu(ecu.name);

                for (const auto& dtc : dtcs) {
                    report_stream << "  - " << (dtc.is_active ? "[ATIVO] " : "[MEMORIZADO] ");
                    report_stream << "SPN " << dtc.spn << ", FMI " << dtc.fmi << ": " << dtc.description << "\n";
                }
                report_stream << "\n";
            }
        }
        if (!has_errors_in_any_ecu) {
            report_stream << "Nenhuma centralina com erros foi detetada.\n";
        }
    }

    report_stream << "========================================\n";
    report_stream << "           FIM DO RELATÓRIO\n";
    report_stream << "========================================\n";

    return report_stream.str();
}

void ReportScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);

    lv_obj_t* header = create_header(screen_obj, "Relatório de Diagnóstico");
    lv_obj_t* footer = create_footer(screen_obj);

    // Botão de voltar
    lv_obj_t* back_btn = create_back_button(header);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("Diagnostics");
    }, LV_EVENT_CLICKED, ui_manager);

    // Área de texto para mostrar o relatório
    lv_obj_t* report_ta = lv_textarea_create(screen_obj);
    lv_obj_set_size(report_ta, 460, 350);
    lv_obj_align(report_ta, LV_ALIGN_TOP_MID, 0, 65);
    lv_textarea_set_one_line(report_ta, false); // Permite múltiplas linhas

    // Gera e exibe o relatório
    std::string report_content = generate_report_string();
    lv_textarea_set_text(report_ta, report_content.c_str());

    // Botão de guardar no rodapé
    lv_obj_t* save_btn = create_footer_button(footer, "Guardar", LV_SYMBOL_SAVE);
    lv_obj_add_event_cb(save_btn, [](lv_event_t* e) {
        lv_obj_t* ta = static_cast<lv_obj_t*>(lv_event_get_user_data(e));
        const char* content = lv_textarea_get_text(ta);

        // Gera um nome de ficheiro único
        char filename[32];
        snprintf(filename, sizeof(filename), "/report_%lu.txt", millis());

        // Guarda o ficheiro no LittleFS
        File file = LittleFS.open(filename, "w");
        if (file) {
            file.print(content);
            file.close();
            LV_LOG_USER("Relatório guardado com sucesso em: %s", filename);
            create_info_msgbox(lv_scr_act(), "Sucesso", "Relatório guardado com sucesso!");
        } else {
            LV_LOG_ERROR("Falha ao abrir o ficheiro %s para escrita", filename);
            create_info_msgbox(lv_scr_act(), "Erro", "Falha ao guardar o relatório.");
        }
    }, LV_EVENT_CLICKED, report_ta); // Passa a área de texto como user_data
}