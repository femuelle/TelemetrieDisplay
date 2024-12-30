#ifndef _INCLUDE_CONFIG_H_
#define _INCLUDE_CONFIG_H_

/*Set to your screen resolution*/
constexpr auto TFT_HOR_RES = 320;
constexpr auto TFT_VER_RES = 240;

constexpr auto TAB_HEIGHT = 40;
constexpr auto PADDING = 5;
constexpr auto LARGE_PANEL_HEIGHT = 60;
constexpr auto SMALL_PANEL_HEIGHT = 40;

constexpr auto CELL_CHARGED_MV = 4200;
constexpr auto CELL_ALARM_MV = 3200;

constexpr auto ALARM_HYSTERESIS_MS = 2 * 1000; // 2 Seconds

#endif // _INCLUDE_CONFIG_H_
