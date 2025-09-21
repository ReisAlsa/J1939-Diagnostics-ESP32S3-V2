#pragma once
#include "IScreen.h"

class AboutScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};