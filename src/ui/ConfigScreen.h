#pragma once
#include "IScreen.h"

class ConfigScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};
