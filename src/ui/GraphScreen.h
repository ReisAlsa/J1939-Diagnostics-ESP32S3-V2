#ifndef GRAPH_SCREEN_H
#define GRAPH_SCREEN_H

#include "IScreen.h"
#include <string>

class GraphScreen : public IScreen {
public:
    void create(UIManager* uim) override;

private:
    lv_timer_t* update_timer = nullptr;
    lv_obj_t* chart = nullptr;
    std::string series1_name;
    std::string series2_name;
    uint16_t zoom_level = 256; // 256 = LV_CHART_ZOOM_NONE
    bool is_paused = false;
};

#endif // GRAPH_SCREEN_H