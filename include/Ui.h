#ifndef _INCLUDE_UI_H_
#define _INCLUDE_UI_H_

#include <lvgl.h>
#include <memory>
#include <vector>
#include <map>
#include "panel.h"

class Ui
{

private:
    using TriggerCellDetectionCbkType = void (*)(lv_event_t *e);

    std::map<Panel::Type, std::vector<Panel>> panels_;
    lv_obj_t *tab_view_;

    void SetupPadding(lv_obj_t *parent);
    void SetupStyle();
    void CreateElectricTab(lv_obj_t *parent);
    void CreateGliderTab(lv_obj_t *parent);
    void StorePanel(Panel::Type type, Panel panel);

public:
    TriggerCellDetectionCbkType triggerCellDection_cbk_ = nullptr;

    Ui() = default;
    void createTabView();

    void SetTriggerCellDetectionCbk(TriggerCellDetectionCbkType cbk);
    void updateBattery(float voltage);
    void updateRssi(uint8_t rssi);
};

#endif // _INCLUDE_UI_H_
