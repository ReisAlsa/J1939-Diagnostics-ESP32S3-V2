#ifndef REPORT_LIST_SCREEN_H
#define REPORT_LIST_SCREEN_H

#include "IScreen.h"
#include <string>

class ReportListScreen : public IScreen {
public:
    void create(UIManager* uim) override;
    static const std::string& get_selected_report_filename();

private:
    static std::string selected_report_filename;
};

#endif // REPORT_LIST_SCREEN_H