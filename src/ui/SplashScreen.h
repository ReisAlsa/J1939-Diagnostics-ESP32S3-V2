#pragma once
#include "IScreen.h"

class SplashScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};
