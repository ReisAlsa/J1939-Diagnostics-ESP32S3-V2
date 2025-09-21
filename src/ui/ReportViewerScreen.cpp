#include "ReportViewerScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "ReportListScreen.h" // Para obter o nome do ficheiro
#include <LittleFS.h>
#include <SD.h>
#include <SPI.hh>

// Pinos do cartão SD e instância SPI (reutilizado de UpdateManager)
#define SD_SCK  11
#define SD_MISO 13
#define SD_MOSI 12
#define SD_CS   10
static SPIClass spi_sd(HSPI);


void ReportViewerScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);

    // Obtém o nome do ficheiro selecionado no ecrã anterior
    const std::string& filename = ReportListScreen::get_selected_report_filename();

    lv_obj_t* header = create_header(screen_obj, filename.c_str());
    lv_obj_t* footer = create_footer(screen_obj);
    // Garante que os botões no rodapé se alinham corretamente
    lv_obj_set_layout(footer, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(footer, 10, 0);

    // Botão de voltar
    lv_obj_t* back_btn = create_back_button(header);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("ReportList"); // Volta para a lista de relatórios
    }, LV_EVENT_CLICKED, ui_manager);

    // Área de texto para mostrar o conteúdo do relatório
    lv_obj_t* report_ta = lv_textarea_create(screen_obj);
    lv_obj_set_size(report_ta, 460, 400); // Mais altura para visualização
    lv_obj_align(report_ta, LV_ALIGN_TOP_MID, 0, 65);
    lv_textarea_set_one_line(report_ta, false);

    // Botão para exportar para o cartão SD
    lv_obj_t* export_btn = create_footer_button(footer, "Exportar SD", LV_SYMBOL_SD_CARD);
    lv_obj_add_event_cb(export_btn, [](lv_event_t* e) {
        const std::string* filename_ptr = static_cast<const std::string*>(lv_event_get_user_data(e));
        lv_obj_t* ta = (lv_obj_t*)lv_obj_get_user_data(lv_event_get_target(e)); // Recupera o text area

        // 1. Inicializar SD
        spi_sd.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
        if (!SD.begin(SD_CS, spi_sd)) {
            create_info_msgbox(lv_scr_act(), "Erro", "Falha ao montar cartao SD!");
            return;
        }

        // 2. Abrir ficheiro no SD e escrever
        File file = SD.open(filename_ptr->c_str(), FILE_WRITE);
        if (file) {
            file.print(lv_textarea_get_text(ta));
            file.close();
            create_info_msgbox(lv_scr_act(), "Sucesso", "Relatório exportado para o cartão SD!");
        } else {
            create_info_msgbox(lv_scr_act(), "Erro", "Falha ao escrever no cartão SD.");
        }

        SD.end(); // Liberta o barramento SPI

    }, LV_EVENT_CLICKED, (void*)&filename);
    lv_obj_set_user_data(export_btn, report_ta); // Adiciona o text area como user_data secundário

    // Botão para apagar o relatório
    lv_obj_t* delete_btn = create_footer_button(footer, "Apagar", LV_SYMBOL_TRASH);
    lv_obj_set_user_data(delete_btn, (void*)&filename); // Passa o endereço do nome do ficheiro
    lv_obj_add_event_cb(delete_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        const std::string* filename_ptr = static_cast<const std::string*>(lv_obj_get_user_data(lv_event_get_target(e)));

        // Cria uma caixa de diálogo para confirmação
        static const char* btns[] = {"Sim", "Nao", ""};
        lv_obj_t* mbox = lv_msgbox_create(NULL, "Apagar Relatório", "Tem a certeza que deseja apagar este relatório?\n\nEsta ação não pode ser desfeita.", btns, true);
        
        // Adiciona um evento para quando um botão da msgbox é pressionado
        lv_obj_add_event_cb(mbox, [](lv_event_t* e_mbox) {
            lv_obj_t* obj = lv_event_get_current_target(e_mbox);
            const char* btn_text = lv_msgbox_get_active_btn_text(obj);
            
            // Reutiliza os dados do evento anterior
            UIManager* uim_mbox = static_cast<UIManager*>(lv_event_get_user_data(e_mbox));
            const std::string* filename_mbox_ptr = static_cast<const std::string*>(lv_obj_get_user_data(lv_event_get_target(e_mbox)));

            if (btn_text != NULL && strcmp(btn_text, "Sim") == 0) {
                if (LittleFS.remove(filename_mbox_ptr->c_str())) {
                    LV_LOG_USER("Relatório %s apagado.", filename_mbox_ptr->c_str());
                    uim_mbox->switch_to("ReportList"); // Volta para a lista atualizada
                }
            }
        }, LV_EVENT_VALUE_CHANGED, e); // Passa o evento original para reutilizar os dados
        lv_obj_center(mbox);
    }, LV_EVENT_CLICKED, ui_manager);

    // Garante que a barra de scroll aparece automaticamente quando o conteúdo é maior que a área
    lv_obj_set_scrollbar_mode(report_ta, LV_SCROLLBAR_MODE_AUTO);
    lv_textarea_set_cursor_click_pos(report_ta, false); // Impede que o cursor mude de posição ao clicar

    // Lê e exibe o conteúdo do ficheiro
    File file = LittleFS.open(filename.c_str(), "r");
    if (file) {
        std::string content = "";
        // Lê o ficheiro em blocos para não usar demasiada RAM de uma só vez
        while (file.available()) {
            char buf[256];
            int len = file.readBytes(buf, sizeof(buf) - 1);
            buf[len] = '\0';
            content += buf;
        }
        file.close();
        lv_textarea_set_text(report_ta, content.c_str());
    } else {
        LV_LOG_ERROR("Falha ao abrir o relatório: %s", filename.c_str());
        lv_textarea_set_text(report_ta, "Erro: Não foi possível abrir o ficheiro do relatório.");
    }
}