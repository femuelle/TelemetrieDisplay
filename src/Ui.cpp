#include <lvgl.h>
#include <Arduino.h>
#include <memory>

#include "Config.h"
#include "Panel.h"
#include "Ui.h"

static lv_style_t style_large_value;
static lv_style_t style_small_value;
static lv_style_t style_title;
static lv_style_t style_icon;

static void tabview_delete_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_DELETE)
    {
        lv_style_reset(&style_large_value);
        lv_style_reset(&style_small_value);
        lv_style_reset(&style_title);
        lv_style_reset(&style_icon);
    }
}

static void triggerCellDection_cbk_wrapper(lv_event_t *e)
{
    static_cast<Ui *>(lv_event_get_user_data(e))->triggerCellDection_cbk_(e);
}

void Ui::SetupPadding(lv_obj_t *object)
{
    lv_obj_set_style_pad_all(object, PADDING, 0);
    lv_obj_set_style_pad_row(object, PADDING, 0);
    lv_obj_set_style_pad_column(object, PADDING, 0);
}

void Ui::SetupStyle()
{
#if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          LV_FONT_DEFAULT);
#endif

    lv_style_init(&style_large_value);
    lv_style_set_text_font(&style_large_value, &lv_font_montserrat_36);

    lv_style_init(&style_title);
    lv_style_set_text_opa(&style_title, LV_OPA_50);
    lv_style_set_text_font(&style_title, LV_FONT_DEFAULT);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, &lv_font_montserrat_24);

    lv_style_init(&style_small_value);
    lv_style_set_text_font(&style_small_value, &lv_font_montserrat_28);

    // lv_style_init(&style_bullet);
    // lv_style_set_border_width(&style_bullet, 0);
    // lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);
}

void Ui::CreateGliderTab(lv_obj_t *parent)
{
    // Create gird layout
    static int32_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

    Panel local_panel;

    // Create the first panel
    local_panel = Panel(parent, Panel::Size::LARGE, "RX Spannung [V]");
    local_panel.SetStyle(&style_title, &style_icon, &style_large_value);
    local_panel.SetIcon(LV_SYMBOL_CHARGE);
    StorePanel(Panel::Type::RX_BATTERY, local_panel);
    lv_obj_set_grid_cell(local_panel.getPanel(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    // Create the second panel
    local_panel = Panel(parent, Panel::Size::LARGE, "RSSI [dBm]");
    local_panel.SetStyle(&style_title, &style_icon, &style_large_value);
    local_panel.SetIcon(LV_SYMBOL_WIFI);
    StorePanel(Panel::Type::RSSI, local_panel);
    lv_obj_set_grid_cell(local_panel.getPanel(), LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    // Create the third panel
    local_panel = Panel(parent, Panel::Size::SMALL, "Höhe [m]");
    local_panel.SetStyle(&style_title, &style_icon, &style_small_value);
    local_panel.SetValue("24.0");
    StorePanel(Panel::Type::HEIGHT, local_panel);
    lv_obj_set_grid_cell(local_panel.getPanel(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);

    // Create the fourth panel
    local_panel = Panel(parent, Panel::Size::SMALL, "Vario");
    local_panel.SetStyle(&style_title, &style_icon, &style_small_value);
    local_panel.SetValue("Ton ein");
    StorePanel(Panel::Type::VARIO, local_panel);
    lv_obj_set_grid_cell(local_panel.getPanel(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
}


void Ui::CreateElectricTab(lv_obj_t *parent)
{
    // Create gird layout
    static int32_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

    Panel local_panel;

    // Create the first panel
    local_panel = Panel(parent, Panel::Size::LARGE, "Spannung [V]");
    local_panel.SetStyle(&style_title, &style_icon, &style_large_value);
    local_panel.SetIcon(LV_SYMBOL_CHARGE);
    lv_obj_add_event_cb(local_panel.getPanel(), triggerCellDection_cbk_wrapper, LV_EVENT_CLICKED, this);
    StorePanel(Panel::Type::BATTERY, local_panel);
    lv_obj_set_grid_cell(local_panel.getPanel(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    // Create the second panel
    local_panel = Panel(parent, Panel::Size::LARGE, "RSSI [dBm]");
    local_panel.SetStyle(&style_title, &style_icon, &style_large_value);
    local_panel.SetIcon(LV_SYMBOL_WIFI);
    StorePanel(Panel::Type::RSSI, local_panel);
    lv_obj_set_grid_cell(local_panel.getPanel(), LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    // Create the third panel
    local_panel = Panel(parent, Panel::Size::SMALL, "Strom [A]");
    local_panel.SetStyle(&style_title, &style_icon, &style_small_value);
    local_panel.SetValue("24.0");
    StorePanel(Panel::Type::CURRENT, local_panel);
    lv_obj_set_grid_cell(local_panel.getPanel(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);

    // Create the fourth panel
    local_panel = Panel(parent, Panel::Size::SMALL, "Energie [mAh]");
    local_panel.SetStyle(&style_title, &style_icon, &style_small_value);
    local_panel.SetValue("24.0");
    StorePanel(Panel::Type::ENERGY, local_panel);
    lv_obj_set_grid_cell(local_panel.getPanel(), LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 1, 1);
}

void Ui::StorePanel(Panel::Type type, Panel panel)
{
    panels_.try_emplace(type, std::vector<Panel>{}).first->second.push_back(panel);
}

void Ui::createTabView()
{
    tab_view_ = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tab_view_, TAB_HEIGHT);
    lv_obj_add_event_cb(tab_view_, tabview_delete_event_cb, LV_EVENT_DELETE, NULL);

    lv_obj_set_style_text_font(lv_screen_active(), LV_FONT_DEFAULT, 0);

    lv_obj_t *t1 = lv_tabview_add_tab(tab_view_, "Motorflieger");
    lv_obj_t *t2 = lv_tabview_add_tab(tab_view_, "Segler");
    lv_obj_t *t3 = lv_tabview_add_tab(tab_view_, "Sensoren");

    SetupPadding(t1);
    SetupPadding(t2);
    SetupPadding(t3);

    SetupStyle();

    CreateElectricTab(t1);
    CreateGliderTab(t2);
    // createSensorsTab(t3);

    // color_changer_create(tab_view_);
}

void Ui::SetTriggerCellDetectionCbk(TriggerCellDetectionCbkType cbk)
{
    triggerCellDection_cbk_ = cbk;
    Serial.println("setcbk");
}

void Ui::updateBattery(float voltage)
{
    char text[5];
    snprintf(text, 5, "%4.1f", voltage);

    if (auto result = panels_.find(Panel::Type::BATTERY); result != panels_.end())
    {
        for (const auto &panel : result->second)
        {
            panel.SetValue(text);
        }
    }
}

void Ui::updateRssi(uint8_t rssi)
{
    char text[4];
    snprintf(text, 4, "%u", rssi);
    if (auto result = panels_.find(Panel::Type::RSSI); result != panels_.end())
    {
        for (const auto &panel : result->second)
        {
            panel.SetValue(text);
        }
    }
}
