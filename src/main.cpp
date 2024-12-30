// /* Using LVGL with Arduino requires some extra steps...
//  *
//  * Be sure to read the docs here: https://docs.lvgl.io/master/integration/framework/arduino.html
//  * but note you should use the lv_conf.h from the repo as it is pre-edited to work.
//  *
//  * You can always edit your own lv_conf.h later and exclude the example options once the build environment is working.
//  *
//  * Note you MUST move the 'examples' and 'demos' folders into the 'src' folder inside the lvgl library folder
//  * otherwise this will not compile, please see README.md in the repo.
//  *
//  */
#include <lvgl.h>

#include <TFT_eSPI.h>

#include <examples/lv_examples.h>
#include <demos/lv_demos.h>

#include <XPT2046_Touchscreen.h>

#include "BatteryAlarm.h"
#include "Config.h"
#include "Ui.h"

#include <FrSkySportSensorXjt.h>
#include <FrSkySportSensorFlvss.h>
#include <FrSkySportDecoder.h>

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

FrSkySportSensorXjt xjt; // Create XJT sensor with default ID
FrSkySportSensorFlvss flvss;
FrSkySportDecoder decoder; // Create decoder object without polling

void test(bool alarm_set)
{
  if (alarm_set)
  {
    Serial.println("alarm");
  }
  else
  {
    Serial.println("kein alarm");
  }
}

Ui ui;
BatteryAlarm *battery_alarm;
SPIClass touchscreenSpi(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
uint16_t touchScreenMinimumX = 200, touchScreenMaximumX = 3700, touchScreenMinimumY = 240, touchScreenMaximumY = 3800;

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char *buf)
{
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}
#endif

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  if (touchscreen.touched())
  {
    TS_Point p = touchscreen.getPoint();
    // Some very basic auto calibration so it doesn't go out of range
    if (p.x < touchScreenMinimumX)
      touchScreenMinimumX = p.x;
    if (p.x > touchScreenMaximumX)
      touchScreenMaximumX = p.x;
    if (p.y < touchScreenMinimumY)
      touchScreenMinimumY = p.y;
    if (p.y > touchScreenMaximumY)
      touchScreenMaximumY = p.y;
    // Map this to the pixel position
    data->point.x = map(p.x, touchScreenMinimumX, touchScreenMaximumX, 1, TFT_HOR_RES); /* Touchscreen X calibration */
    data->point.y = map(p.y, touchScreenMinimumY, touchScreenMaximumY, 1, TFT_VER_RES); /* Touchscreen Y calibration */
    data->state = LV_INDEV_STATE_PRESSED;
    // Serial.println(data->point.x);
    // Serial.println(data->point.y);
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

lv_indev_t *indev;     // Touchscreen input device
uint8_t *draw_buf;     // draw_buf is allocated on heap otherwise the static area is too big on ESP32 at compile
uint32_t lastTick = 0; // Used to track the tick timer

void triggerCellDetectionWrapper(lv_event_t * e){
  Serial.println("pressed");
  battery_alarm->TriggerCellDetection();
}

void setup()
{
  decoder.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_22, &xjt, &flvss);

  // Some basic info on the Serial console
  String LVGL_Arduino = "LVGL demo ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
  Serial.println(LVGL_Arduino);

  battery_alarm = new BatteryAlarm(&test);

  // Initialise the touchscreen
  touchscreenSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS); /* Start second SPI bus for touchscreen */
  touchscreen.begin(touchscreenSpi);                                         /* Touchscreen init */
  touchscreen.setRotation(3);                                                /* Inverted landscape orientation to match screen */

  // Initialise LVGL
  lv_init();
  draw_buf = new uint8_t[DRAW_BUF_SIZE];
  lv_display_t *disp;
  disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, DRAW_BUF_SIZE);

  // Initialize the XPT2046 input device driver
  indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  // Done
  ui.createTabView();
  ui.SetTriggerCellDetectionCbk(triggerCellDetectionWrapper);
  battery_alarm->TriggerCellDetection();
  Serial.println("Setup done");
}

float getAddedCellVoltages()
{
  float result = 0;
  result += flvss.getCell1();
  result += flvss.getCell2();
  result += flvss.getCell3();
  result += flvss.getCell4();
  result += flvss.getCell5();
  result += flvss.getCell6();
  return result;
}

void loop()
{
  static long lastUpdate = millis();
  float voltage = 0;

  uint16_t decodeResult = decoder.decode();
  if (decodeResult != SENSOR_NO_DATA_ID)
  {
    Serial.print("Decoded data with AppID 0x");
    Serial.println(decodeResult, HEX);
  }

  if (millis() > lastUpdate + 500)
  {
    lastUpdate = millis();
    voltage = getAddedCellVoltages();
    ui.updateBattery(voltage);
    ui.updateRssi(xjt.getRssi());
    battery_alarm->UpdateVoltage(voltage);
  }

  lv_tick_inc(millis() - lastTick); // Update the tick timer. Tick is new for LVGL 9
  lastTick = millis();
  lv_timer_handler(); // Update the UI
}
