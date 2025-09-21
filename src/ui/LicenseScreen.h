#pragma once
#include "IScreen.h"

class LicenseScreen : public IScreen {
public:
    void create(UIManager* uim) override;

private:
    lv_obj_t* kb = nullptr;
    lv_obj_t* status_label = nullptr;
};