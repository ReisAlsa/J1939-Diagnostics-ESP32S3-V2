#ifndef REPORT_SCREEN_H
#define REPORT_SCREEN_H

#include "IScreen.h"
#include <string>

class ReportScreen : public IScreen {
public:
    void create(UIManager* uim) override;

private:
    std::string generate_report_string();
    std::string get_current_datetime();
};

#endif // REPORT_SCREEN_H