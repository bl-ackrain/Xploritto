#include "PokittoSettings.h"
#include "Popup.h"

void PokittoSettings::init()
{
    m_volume=eeprom_read_byte(reinterpret_cast<uint16_t*>(EESETTINGS_VOL));
    m_loaderwait=eeprom_read_byte(reinterpret_cast<uint16_t*>(EESETTINGS_LOADERWAIT));
    m_volumewait=eeprom_read_byte(reinterpret_cast<uint16_t*>(EESETTINGS_VOLWAIT));
    m_timeformat=eeprom_read_byte(reinterpret_cast<uint16_t*>(EESETTINGS_TIMEFORMAT));
    m_dateformat=eeprom_read_byte(reinterpret_cast<uint16_t*>(EESETTINGS_DATEFORMAT));
}

bool PokittoSettings::update()
{
    if(Pokitto::Buttons::repeat(BTN_UP,2))
    {
        if(m_selected > 0 )
           m_selected --;
    }else if(Pokitto::Buttons::repeat(BTN_DOWN,2))
    {
         if(m_selected < 6 )
           m_selected ++;
    }

    if(Pokitto::Buttons::pressed(BTN_A))
    {
        switch(m_selected)
        {
        case 0:
            {
                std::uint8_t ret=Popup::volumeSlider("Set Volume", m_volume>>5, 7);
                if(ret <= 7)
                {
                    m_volume=ret << 5;
                    eeprom_write_byte(reinterpret_cast<uint16_t*>(EESETTINGS_VOL), m_volume);
                }
            }
            break;
        case 1:
            {
                std::string menu_entries[]={"1 Sec" ,"2 Sec","3 Sec","4 Sec","5 Sec"};
                size_t ret=Popup::Menu("Choose value", menu_entries, 0, 5);
                if(ret < 5)
                {
                    m_loaderwait=ret+1;
                    eeprom_write_byte(reinterpret_cast<uint16_t*>(EESETTINGS_LOADERWAIT), m_loaderwait);
                }

            }
            break;
        case 2:
            {
                std::string menu_entries[]={"1 Sec" ,"2 Sec","3 Sec","4 Sec","5 Sec", "6 Sec" ,"7 Sec","8 Sec","9 Sec","10 Sec"};
                size_t ret=Popup::Menu("Choose value", menu_entries, 0, 10);
                if(ret < 10)
                {
                    m_volumewait=ret+1;
                    eeprom_write_byte(reinterpret_cast<uint16_t*>(EESETTINGS_VOLWAIT), m_volumewait);
                }

            }
            break;
        case 3:
            {
                std::string menu_entries[]={"24 Hrs", "12 Hrs"};
                size_t ret=Popup::Menu("Choose value", menu_entries, 0, 2);
                if(ret < 2)
                {
                    m_timeformat=ret;
                    eeprom_write_byte(reinterpret_cast<uint16_t*>(EESETTINGS_TIMEFORMAT), m_timeformat);
                }

            }
            break;
        case 4:
            {
                std::string menu_entries[]={"dd-mm-yyyy", "mm-dd-yyyy"};
                size_t ret=Popup::Menu("Choose value", menu_entries, 0, 2);
                if(ret < 2)
                {
                    m_dateformat=ret;
                    eeprom_write_byte(reinterpret_cast<uint16_t*>(EESETTINGS_DATEFORMAT), m_dateformat);
                }

            }
            break;
        case 5:
            {
            time_t seconds = time(NULL);
            struct tm *tmp = gmtime(&seconds);
            size_t t=Popup::timePicker("Set Time", m_timeformat==0,(tmp->tm_hour << 16) | tmp->tm_min);
            if(t!=0xFFFFFFFF)
            {
                tmp->tm_hour=t>>16;
                tmp->tm_min=t&0xFF;
                set_time(time_to_epoch(tmp,0));
            }

            }
            break;
        case 6:
            time_t seconds = time(NULL);
            struct tm *tmp = gmtime(&seconds);
            size_t t=Popup::datePicker("Set Date", m_dateformat, (tmp->tm_year << 16) | tmp->tm_mon << 8 |tmp->tm_mday);
            if(t!=0xFFFFFFFF)
            {
                seconds = time(NULL);
                tmp = gmtime(&seconds);
                tmp->tm_year=t>>16;
                tmp->tm_mon=t>>8&0xFF;
                tmp->tm_mday=t&0xFF;
                set_time(time_to_epoch(tmp,0));
            }
            break;

        }
    }else if(Pokitto::Buttons::pressed(BTN_B))
    {
        return false;
    }

    return true;
};

void PokittoSettings::draw()
{
    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);

    std::string entries[]= {"Volume", "Loader Wait", "Volume Wait", "Time Format" , "Date Format", "Time", "Date"};
    //const unsigned char *icons [] = {settings_pokitto, settings_xploritto};
    time_t seconds = time(NULL);
    struct tm *tmp = gmtime(&seconds);

    for(size_t i = 0 ;i < 7; i++)
    {
        Pokitto::Display::setColor(WHITE);

        if(m_selected==i)
            Pokitto::Display::setColor(LIGHT_BLUE);



        Pokitto::Display::fillRect(5,STATUSBARHEIGHT+5+i*20,Pokitto::Display::width-10,20-2);
        if(m_selected==i)
            Pokitto::Display::setColor(WHITE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::setCursor(20, STATUSBARHEIGHT+11+i*20);
        Pokitto::Display::print(entries[i].c_str());

        //Pokitto::Display::drawBitmap(10,STATUSBARHEIGHT+7+i*20,icons[i]);

        switch(i)
        {
        case 0:
            //Pokitto::Display::setCursor(140, STATUSBARHEIGHT+11+i*20);
            //Pokitto::Display::print((int)m_volume>>5);
            Pokitto::Display::setColor(LIGHT_GRAY);
            Pokitto::Display::fillRoundRect(140,STATUSBARHEIGHT+10+i*20,7*8,10,2);
            Pokitto::Display::setColor(GREEN);
            if(m_volume > 0)
                Pokitto::Display::fillRoundRect(140,STATUSBARHEIGHT+10+i*20,(m_volume>>5)*8,10,2);

            Pokitto::Display::setColor(WHITE);
            Pokitto::Display::setFont(font3x5);
            Pokitto::Display::setCursor(140+22, STATUSBARHEIGHT+13+i*20);
            Pokitto::Display::printf("%u/%u", m_volume>>5, 7);
            Pokitto::Display::setFont(font5x7);
            break;
        case 1:
            Pokitto::Display::setCursor(140, STATUSBARHEIGHT+11+i*20);
            Pokitto::Display::print((int)m_loaderwait);
            Pokitto::Display::print(" Sec");
            break;
        case 2:
            Pokitto::Display::setCursor(140, STATUSBARHEIGHT+11+i*20);
            Pokitto::Display::print((int)m_volumewait);
            Pokitto::Display::print(" Sec");
            break;
        case 3:
            Pokitto::Display::setCursor(140, STATUSBARHEIGHT+11+i*20);
            if(m_timeformat==0)
                Pokitto::Display::print("24");
            if(m_timeformat==1)
                Pokitto::Display::print("12");
            Pokitto::Display::print(" Hrs");
            break;
        case 4:
            Pokitto::Display::setCursor(140, STATUSBARHEIGHT+11+i*20);
            if(m_dateformat==0)
                Pokitto::Display::print("dd-mm");
            if(m_dateformat==1)
                Pokitto::Display::print("mm-dd");
            Pokitto::Display::print("-yyyy");
            break;
        case 5:
            Pokitto::Display::setCursor(140, STATUSBARHEIGHT+11+i*20);
            if(m_timeformat==0)
                Pokitto::Display::printf("%02u:%02u",tmp->tm_hour,tmp->tm_min);
            if(m_timeformat==1)
                Pokitto::Display::printf("%02u:%02u %sM",tmp->tm_hour>12?tmp->tm_hour-12:(tmp->tm_hour==0?12:tmp->tm_hour),tmp->tm_min,tmp->tm_hour < 12?"A":"P");
            break;
        case 6:
            Pokitto::Display::setCursor(140, STATUSBARHEIGHT+11+i*20);
            if(m_dateformat==0)
                Pokitto::Display::printf("%02u-%02u-%u",tmp->tm_mday,tmp->tm_mon+1,tmp->tm_year+1900);
            if(m_dateformat==1)
                Pokitto::Display::printf("%02u-%02u-%u",tmp->tm_mon+1,tmp->tm_mday,tmp->tm_year+1900);
            break;
        }
    }
};


time_t PokittoSettings::time_to_epoch( const struct tm *ltm, int utcdiff )
{
   constexpr int mon_days [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   long tyears, tdays, leaps, utc_hrs;
   int i;

   tyears = ltm->tm_year - 70;  // tm->tm_year is from 1900.
   leaps = (tyears + 2) / 4; // no of next two lines until year 2100.
   //i = (ltm->tm_year – 100) / 100;
   //leaps -= ( (i/4)*3 + i%4 );
   tdays = 0;
   for (i=0; i < ltm->tm_mon; i++) tdays += mon_days[i];

   tdays += ltm->tm_mday-1; // days of month passed.
   tdays = tdays + (tyears * 365) + leaps;

   utc_hrs = ltm->tm_hour + utcdiff; // for your time zone.
   return (tdays * 86400) + (utc_hrs * 3600) + (ltm->tm_min * 60) + ltm->tm_sec;
}
