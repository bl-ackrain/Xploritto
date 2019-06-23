#include "StatusBar.h"

float map(float x, float  in_min, float  in_max, float out_min, float  out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

StatusBar::StatusBar()
{
    m_vRefPin = new AnalogIn(POK_BATTERY_PIN1);
    m_battPin = new AnalogIn(POK_BATTERY_PIN2);
}
void StatusBar::update()
{
    time_t seconds = time(NULL);
    if(seconds!=m_seconds)
    {
        m_seconds=seconds;
        m_time = gmtime(&seconds);
        redraw=true;

        if(m_tbattery==0)
        {
            m_tbattery=3;
            if(m_vRefPin->read_u16()!=0)
                vBattery = (m_battPin->read()*2.67*2)/m_vRefPin->read();
        }
        else
            m_tbattery--;
    }

}
void StatusBar::draw()
{
    if(!redraw)
        return;

    Pokitto::Display::setColor(0);
    Pokitto::Display::fillRect(0,0,Pokitto::Display::width,STATUSBARHEIGHT-1);
    Pokitto::Display::setColor(3);
    Pokitto::Display::setCursor(5,2);


    Pokitto::Display::setFont(font3x5);
    Pokitto::Display::printf("%02u-%02u-%u %02u:%02u",m_time->tm_mday,m_time->tm_mon+1,m_time->tm_year+1900,m_time->tm_hour,m_time->tm_min);

    Pokitto::Display::setCursor(Pokitto::Display::width-55,2);
    Pokitto::Battery::update();
    //TODO Battery level
    char volage[20];

    std::uint32_t vPer = map(vBattery, 3.4, 4.2, 0,100);
    if(vPer > 100)
        vPer=100;

    sprintf(volage,"%01u.%01uv %u%%", (uint16_t)vBattery, (uint16_t)(vBattery*100)%100,vPer);
    Pokitto::Display::print(volage);
    Pokitto::Display::setFont(font5x7);
    Pokitto::Display::setCursor(Pokitto::Display::width-13,1);
    Pokitto::Display::print('\11');

}
