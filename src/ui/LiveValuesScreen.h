#ifndef LIVE_VALUES_SCREEN_H
#define LIVE_VALUES_SCREEN_H

#include "IScreen.h"
#include <vector>
#include "core/DiagnosticsData.h"

class LiveValuesScreen : public IScreen {
public:
    void create(UIManager* uim) override;
    static const LiveValueData& get_selected_value();

private:
    lv_timer_t* update_timer = nullptr;
    std::vector<lv_obj_t*> value_labels;
    std::vector<LiveValueData> live_values_data;
    static LiveValueData selected_value;
};

#endif // LIVE_VALUES_SCREEN_H