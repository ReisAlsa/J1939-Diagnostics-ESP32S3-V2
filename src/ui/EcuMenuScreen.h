#ifndef ECU_MENU_SCREEN_H
#define ECU_MENU_SCREEN_H

#include "IScreen.h"

class EcuMenuScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};

#endif // ECU_MENU_SCREEN_H