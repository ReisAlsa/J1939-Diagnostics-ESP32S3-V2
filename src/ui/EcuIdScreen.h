#ifndef ECU_ID_SCREEN_H
#define ECU_ID_SCREEN_H

#include "IScreen.h"

class EcuIdScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};

#endif // ECU_ID_SCREEN_H