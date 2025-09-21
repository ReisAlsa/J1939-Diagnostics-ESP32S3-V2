#pragma once
#include "IScreen.h"

class DiagnosticsScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};
