#include "SplashScreen.h"
#include "UIManager.h"

void SplashScreen::create(UIManager* uim) {
    ui_manager = uim;
    screen_obj = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_obj, lv_color_hex(0x000030), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(screen_obj);
    lv_label_set_text(label, "Ferramenta de Diagnóstico\nJ1939");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(label);
}
