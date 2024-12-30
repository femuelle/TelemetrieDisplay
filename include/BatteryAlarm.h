#ifndef _INCLUDE_BATTERYALARM_H_
#define _INCLUDE_BATTERYALARM_H_

#include <Arduino.h>
#include <DacESP32.h>
#include <stdint.h>

class BatteryAlarm
{
private:
    using AlarmCbkType = void (*)(bool alarm_set);

    uint16_t voltage_ = 0;
    uint16_t alarm_voltage_ = 0;
    AlarmCbkType alarm_cbk_ = nullptr;

    const uint8_t kTimerId = 0;
    const uint16_t kTimerPrescaler = 80; // results in 1MHz tickrate
    hw_timer_t *alarm_timer_ = nullptr;
    bool alarm_sounding_ = false;

    void AlarmISR();


public:
    BatteryAlarm(AlarmCbkType cbk);
    ~BatteryAlarm();

    static void IRAM_ATTR AlarmISRWrapper();
    void UpdateVoltage(uint16_t millivolt);
    uint16_t TriggerCellDetection();
};

#endif // _INCLUDE_BATTERYALARM_H_
