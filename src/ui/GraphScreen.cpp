#include "GraphScreen.h"
#include "UIManager.h"
#include "common/UIComponents.h"
#include "LiveValuesScreen.h" // Para obter o valor selecionado
#include "DiagnosticsScreen.h" // Para obter a ECU

static void timer_callback(lv_timer_t* timer) {
    GraphScreen* screen = static_cast<GraphScreen*>(timer->user_data);
    lv_obj_t* chart = (lv_obj_t*)lv_obj_get_user_data(screen->get_screen_obj());

    // Obtém as séries do gráfico
    lv_chart_series_t* series1 = lv_chart_get_series(chart, 0);
    lv_chart_series_t* series2 = lv_chart_get_series(chart, 1);

    // Obtém os nomes dos valores a serem plotados
    const std::string* s1_name = (const std::string*)lv_obj_get_user_data(lv_obj_get_child(chart, 0));
    const std::string* s2_name = series2 ? (const std::string*)lv_obj_get_user_data(lv_obj_get_child(chart, 1)) : nullptr;

    // Lê todos os valores da ECU
    const EcuData& selected_ecu = DiagnosticsScreen::get_selected_ecu();
    auto all_values = read_live_values_from_ecu(selected_ecu.name);

    // Itera sobre os valores lidos e atualiza as séries correspondentes
    for (const auto& val : all_values) {
        // Atualiza a primeira série
        if (s1_name && val.name == *s1_name) {
            lv_chart_set_next_value(chart, series1, (int16_t)val.value);
        }
        // Atualiza a segunda série, se existir
        if (s2_name && val.name == *s2_name) {
            lv_chart_set_next_value(chart, series2, (int16_t)val.value);
        }
    }

    lv_chart_refresh(chart); // Atualiza o desenho do gráfico
}

void GraphScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);

    const LiveValueData& selected_value = LiveValuesScreen::get_selected_value();
    series1_name = selected_value.name;

    lv_obj_t* header = create_header(screen_obj, "Gráfico em Tempo Real");
    lv_obj_t* footer = create_footer(screen_obj);
    // Garante que os botões no rodapé se alinham corretamente
    lv_obj_set_layout(footer, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(footer, 10, 0);


    lv_obj_t* back_btn = create_back_button(header);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        UIManager* uim = static_cast<UIManager*>(lv_event_get_user_data(e));
        uim->switch_to("LiveValues");
    }, LV_EVENT_CLICKED, ui_manager);

    // Botão para adicionar uma nova série
    lv_obj_t* add_series_btn = create_footer_button(footer, "Adicionar Série", LV_SYMBOL_PLUS);
    lv_obj_add_event_cb(add_series_btn, [](lv_event_t* e) {
        GraphScreen* screen = static_cast<GraphScreen*>(lv_event_get_user_data(e));
        
        // Cria uma janela (msgbox sem botões) para a lista de seleção
        lv_obj_t* win = lv_msgbox_create(NULL, "Adicionar Valor", "", NULL, true);
        lv_obj_set_size(win, 300, 300);
        lv_obj_center(win);

        lv_obj_t* list = lv_list_create(lv_msgbox_get_content(win));
        lv_obj_set_size(list, lv_pct(100), lv_pct(100));

        // Popula a lista com outros valores disponíveis
        const EcuData& selected_ecu = DiagnosticsScreen::get_selected_ecu();
        auto all_values = read_live_values_from_ecu(selected_ecu.name);

        for (const auto& val : all_values) {
            // Adiciona apenas se não for a série 1 e a série 2 ainda não existir
            if (val.name != screen->series1_name && screen->series2_name.empty()) {
                lv_obj_t* list_btn = lv_list_add_btn(list, NULL, val.name.c_str());
                lv_obj_add_event_cb(list_btn, [](lv_event_t* e_btn) {
                    GraphScreen* screen_inner = static_cast<GraphScreen*>(lv_event_get_user_data(e_btn));
                    const char* new_series_name = lv_list_get_btn_text(lv_obj_get_parent(lv_event_get_target(e_btn)), lv_event_get_target(e_btn));
                    
                    screen_inner->series2_name = new_series_name;

                    // Adiciona a segunda série ao gráfico
                    lv_chart_series_t* series2 = lv_chart_add_series(screen_inner->chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_SECONDARY_Y);
                    lv_obj_set_user_data(lv_obj_get_child(screen_inner->chart, 1), (void*)&screen_inner->series2_name);

                    // Ativa e configura o eixo Y secundário para ser visível
                    lv_chart_set_axis_tick(screen_inner->chart, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 5, 2, true, 40);

                    // Encontra os dados do novo valor para definir a gama do eixo secundário
                    for (const auto& val_item : all_values) {
                        if (val_item.name == new_series_name) {
                            float min_val2 = val_item.value * 0.5f;
                            float max_val2 = val_item.value * 1.5f;
                            if (max_val2 < 10) max_val2 = 10;
                            if (max_val2 == min_val2) max_val2 += 10;
                            lv_chart_set_range(screen_inner->chart, LV_CHART_AXIS_SECONDARY_Y, (lv_coord_t)min_val2, (lv_coord_t)max_val2);
                            break;
                        }
                    }

                    // Adiciona à legenda
                    lv_obj_t* legend_cont = (lv_obj_t*)lv_obj_get_user_data(lv_obj_get_parent(screen_inner->chart));
                    lv_obj_t* legend_item2 = lv_label_create(legend_cont);
                    lv_label_set_text_fmt(legend_item2, "%s %s", LV_SYMBOL_MINUS, new_series_name);
                    lv_obj_set_style_text_color(legend_item2, lv_palette_main(LV_PALETTE_RED), 0); // Cor da legenda

                    // Fecha a janela e desativa o botão "Adicionar"
                    lv_msgbox_close(lv_obj_get_parent(lv_obj_get_parent(lv_event_get_target(e_btn))));
                    lv_obj_add_state(lv_event_get_target(e), LV_STATE_DISABLED);

                }, LV_EVENT_CLICKED, screen);
            }
        }
    }, LV_EVENT_CLICKED, this);

    // Botão para Pausar/Resumir
    lv_obj_t* pause_btn = create_footer_button(footer, "Pausar", LV_SYMBOL_PAUSE);
    lv_obj_add_event_cb(pause_btn, [](lv_event_t* e) {
        GraphScreen* screen = static_cast<GraphScreen*>(lv_event_get_user_data(e));
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* label = lv_obj_get_child(btn, 0); // O label é o primeiro filho do botão

        screen->is_paused = !screen->is_paused; // Inverte o estado

        if (screen->is_paused) {
            lv_timer_pause(screen->update_timer);
            lv_label_set_text_fmt(label, "%s Resumir", LV_SYMBOL_PLAY);
        } else {
            lv_timer_resume(screen->update_timer);
            lv_label_set_text_fmt(label, "%s Pausar", LV_SYMBOL_PAUSE);
        }
    }, LV_EVENT_CLICKED, this);

    // Botão para Zoom In
    lv_obj_t* zoom_in_btn = create_footer_button(footer, "Zoom +", LV_SYMBOL_PLUS);
    lv_obj_add_event_cb(zoom_in_btn, [](lv_event_t* e) {
        GraphScreen* screen = static_cast<GraphScreen*>(lv_event_get_user_data(e));
        screen->zoom_level += 128; // Aumenta o zoom
        lv_chart_set_zoom_x(screen->chart, screen->zoom_level);
    }, LV_EVENT_CLICKED, this);

    // Botão para Zoom Out
    lv_obj_t* zoom_out_btn = create_footer_button(footer, "Zoom -", LV_SYMBOL_MINUS);
    lv_obj_add_event_cb(zoom_out_btn, [](lv_event_t* e) {
        GraphScreen* screen = static_cast<GraphScreen*>(lv_event_get_user_data(e));
        if (screen->zoom_level > 256) {
            screen->zoom_level -= 128; // Diminui o zoom, com limite em 256 (sem zoom)
        }
        lv_chart_set_zoom_x(screen->chart, screen->zoom_level);
    }, LV_EVENT_CLICKED, this);

    // Container para a legenda
    lv_obj_t* legend_container = lv_obj_create(screen_obj);
    lv_obj_remove_style_all(legend_container);
    lv_obj_set_size(legend_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(legend_container, LV_ALIGN_TOP_RIGHT, -10, 70);
    lv_obj_set_flex_flow(legend_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(legend_container, 5, 0);
    lv_obj_set_user_data(footer, legend_container); // Truque para passar para o callback do botão

    // Adiciona o primeiro item à legenda
    lv_obj_t* legend_item1 = lv_label_create(legend_container);
    lv_label_set_text_fmt(legend_item1, "%s %s", LV_SYMBOL_MINUS, series1_name.c_str());
    lv_obj_set_style_text_color(legend_item1, lv_palette_main(LV_PALETTE_BLUE), 0);

    // Cria o gráfico
    chart = lv_chart_create(screen_obj);
    lv_obj_set_size(chart, 450, 350);
    lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 65);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 100); // Número de pontos visíveis no gráfico

    // Aplica o zoom inicial e ativa o scroll horizontal
    lv_chart_set_zoom_x(chart, zoom_level);
    lv_obj_set_scroll_dir(chart, LV_DIR_HOR);
    lv_obj_set_scrollbar_mode(chart, LV_SCROLLBAR_MODE_AUTO);

    // Configura a aparência do eixo Y primário
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 50);

    // Define a gama de valores do eixo Y (ajuste conforme necessário)
    // Ex: para RPM, pode ser de 0 a 4000. Para temperatura, 0 a 120.
    // Vamos usar um valor genérico e depois ajustar se necessário.
    float min_val = selected_value.value * 0.5f;
    float max_val = selected_value.value * 1.5f;
    if (max_val < 100) max_val = 100; // Garante uma gama mínima
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, (lv_coord_t)min_val, (lv_coord_t)max_val);

    // Adiciona uma série de dados ao gráfico
    lv_chart_series_t* series1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    // Armazena o nome da série como user_data no objeto da série (que é o primeiro filho do gráfico)
    lv_obj_set_user_data(lv_obj_get_child(chart, 0), (void*)&series1_name);

    // Preenche o gráfico com o valor inicial para não começar vazio
    for (int i = 0; i < 100; ++i) {
        lv_chart_set_next_value(chart, series1, (int16_t)selected_value.value);
    }

    // Cria um temporizador para atualizar o gráfico a cada 250ms
    lv_obj_set_user_data(screen_obj, chart); // Passa o gráfico para o timer
    update_timer = lv_timer_create(timer_callback, 250, this);

    // Adiciona um evento para apagar o temporizador quando a tela for destruída
    lv_obj_add_event_cb(screen_obj, [](lv_event_t* e) {
        GraphScreen* screen = static_cast<GraphScreen*>(lv_event_get_user_data(e));
        if (screen->update_timer) {
            lv_timer_del(screen->update_timer);
            screen->update_timer = nullptr;
        }
    }, LV_EVENT_DELETE, this);
}