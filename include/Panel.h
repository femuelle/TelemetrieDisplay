#ifndef _INCLUDE_PANEL_H_
#define _INCLUDE_PANEL_H_

#include <lvgl.h>
#include <string>

class Panel
{
public:
    enum class Size
    {
        SMALL,
        LARGE,
    };

    enum class Type
    {
        BATTERY,
        CURRENT,
        ENERGY,
        HEIGHT,
        RSSI,
        RX_BATTERY,
        VARIO,
    };

    Panel() = default;
    Panel(lv_obj_t *parent, Size size, std::string descValue);

    void SetValue(std::string text) const;
    void SetIcon(char *hexIcon) const;

    lv_obj_t *getPanel() { return panel; }
    void SetStyle(lv_style_t *desc_style, lv_style_t *icon_style, lv_style_t *value_style) const;

private:
    Size size;

    lv_obj_t *desc;
    lv_obj_t *icon;
    lv_obj_t *panel;
    lv_obj_t *value;

    void setHeight(uint8_t percentage);
};

#endif // _INCLUDE_PANEL_H_
