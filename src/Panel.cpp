#include <lvgl.h>
#include <Arduino.h>

#include "Config.h"
#include "Panel.h"

Panel::Panel(lv_obj_t *parent, Size size, std::string descValue) : size(size)
{
    panel = lv_obj_create(parent);

    desc = lv_label_create(panel);
    icon = lv_label_create(panel);
    value = lv_label_create(panel);

    lv_obj_align(desc, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_align(value, LV_ALIGN_CENTER, 0, -10);

    lv_label_set_text(desc, descValue.c_str());
    lv_label_set_text(icon, "");
    lv_label_set_text(value, "NaN");

    if (Size::SMALL == size)
    {
        setHeight(SMALL_PANEL_HEIGHT);
    }
    else
    {
        setHeight(LARGE_PANEL_HEIGHT);
    }
}

void Panel::setHeight(uint8_t percentage)
{
    float height = TFT_VER_RES - TAB_HEIGHT - 3 * PADDING;
    lv_obj_set_height(panel, (height * (float)percentage / 100.0));
}

void Panel::SetValue(std::string text) const
{
    lv_label_set_text(value, text.c_str());
}

void Panel::SetIcon(char *hexIcon) const
{
    lv_label_set_text(icon, hexIcon);
}

void Panel::SetStyle(lv_style_t *desc_style, lv_style_t *icon_style, lv_style_t *value_style) const
{
    lv_obj_add_style(desc, desc_style, 0);
    lv_obj_add_style(icon, icon_style, 0);
    lv_obj_add_style(value, value_style, 0);
}
