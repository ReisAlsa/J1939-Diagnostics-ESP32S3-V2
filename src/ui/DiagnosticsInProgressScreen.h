#pragma once
#include "IScreen.h"

class DiagnosticsInProgressScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};
