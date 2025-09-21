#ifndef ERROR_LIST_SCREEN_H
#define ERROR_LIST_SCREEN_H

#include "IScreen.h"

class ErrorListScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};

#endif // ERROR_LIST_SCREEN_H