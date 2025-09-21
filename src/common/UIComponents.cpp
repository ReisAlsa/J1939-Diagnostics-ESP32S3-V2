#include "UIComponents.h"

// Função para criar o cabeçalho e rodapé padrão
void create_header_footer(lv_obj_t *parent, const char *title)
{
  // Barra de Título
  lv_obj_t *header = lv_obj_create(parent);
  lv_obj_set_size(header, lv_pct(100), 50);
  lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(header, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_obj_set_style_border_width(header, 0, 0);
  lv_obj_set_style_radius(header, 0, 0);

  lv_obj_t *title_label = lv_label_create(header);
  lv_label_set_text(title_label, title);
  lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
  lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
  lv_obj_center(title_label);

  // Rodapé
  lv_obj_t *footer = lv_obj_create(parent);
  lv_obj_set_size(footer, lv_pct(100), 30);
  lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(footer, lv_palette_main(LV_PALETTE_GREY), 0);
  lv_obj_set_style_border_width(footer, 0, 0);
  lv_obj_set_style_radius(footer, 0, 0);

  lv_obj_t *footer_label = lv_label_create(footer);
  lv_label_set_text(footer_label, "J1939 Diag Tool v0.2");
  lv_obj_set_style_text_color(footer_label, lv_color_white(), 0);
  lv_obj_center(footer_label);
}
