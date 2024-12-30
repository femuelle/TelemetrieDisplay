#include <esp_timer.h>

#include "BatteryAlarm.h"
#include "Config.h"

DacESP32 dac(DAC_CHANNEL_2);
std::function<void()> boundMethod;

BatteryAlarm::BatteryAlarm(AlarmCbkType cbk) : alarm_cbk_(cbk)
{
    boundMethod = std::bind(&BatteryAlarm::AlarmISR, this);
    alarm_timer_ = timerBegin(kTimerId, kTimerPrescaler, true);
    timerAttachInterrupt(alarm_timer_, &BatteryAlarm::AlarmISRWrapper, true);
    timerAlarmWrite(alarm_timer_, 1000000, true);
    timerAlarmEnable(alarm_timer_);
    dac.outputCW(2000);
    dac.setCwScale(DAC_CW_SCALE_8);
}

BatteryAlarm::~BatteryAlarm()
{
    // TODO: disable dac
    if (alarm_timer_)
    {
        timerDetachInterrupt(alarm_timer_);
        // Stop and free the timer
        timerEnd(alarm_timer_);
        alarm_timer_ = nullptr;
    }
}

void IRAM_ATTR BatteryAlarm::AlarmISRWrapper()
{
    boundMethod();
}

void BatteryAlarm::AlarmISR()
{
    if (alarm_sounding_)
    {
        dac.enable();
    }
    else
    {
        dac.disable();
    }
    alarm_sounding_ = !alarm_sounding_;
    // TODO: dis- / enable dac
}

void BatteryAlarm::UpdateVoltage(uint16_t millivolt)
{
    static long last_alarm = 0;
    static bool alarm_sent = false;
    voltage_ = millivolt;

    if (millivolt <= alarm_voltage_)
    {
        if (0 == last_alarm)
        {
            last_alarm = millis();
        }
        else
        {
            // Check that voltage drop is long then hysteresis and alarm hasn't been sent already
            if (millis() >= last_alarm + ALARM_HYSTERESIS_MS && !alarm_sent)
            {
                timerAlarmEnable(alarm_timer_);
                alarm_cbk_(true);
                alarm_sent = true;
            }
        }
    }
    else
    {
        // Voltage higher than thresold
        if (alarm_sent)
        {
            timerAlarmDisable(alarm_timer_);
            alarm_cbk_(false);
        }
        alarm_sent = false;
    }
}

// This function assumes a not-empty battery
uint16_t BatteryAlarm::TriggerCellDetection()
{
    auto voltage = voltage_;

    // Check if an update has already happend
    if (0 == voltage)
    {
        return alarm_voltage_;
    }

    uint8_t number_of_cells = 0;
    for (uint8_t cells = 1; voltage > cells * CELL_CHARGED_MV; ++cells)
    {
        number_of_cells = cells;
    }
    alarm_voltage_ = (number_of_cells + 1) * CELL_ALARM_MV;
    return alarm_voltage_;
}
