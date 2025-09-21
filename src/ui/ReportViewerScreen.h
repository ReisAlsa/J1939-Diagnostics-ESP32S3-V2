#ifndef REPORT_VIEWER_SCREEN_H
#define REPORT_VIEWER_SCREEN_H

#include "IScreen.h"

class ReportViewerScreen : public IScreen {
public:
    void create(UIManager* uim) override;
};

#endif // REPORT_VIEWER_SCREEN_H