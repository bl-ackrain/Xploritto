#pragma once
#include "globals.h"
//#include "PinNames.h"

class StatusBar
{
public:
    StatusBar();
    void update();
    void draw();
private:
    struct tm *m_time;
    time_t m_seconds = 0;
    bool redraw = false;
    size_t m_tbattery=0;
    float vBattery=3.7f;

    AnalogIn *m_vRefPin;
    AnalogIn *m_battPin;
};
