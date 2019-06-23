#include "Settings.h"
#include "Popup.h"
#include "Icons.h"


#include "timer_11u6x.h"
#undef bool

bool Settings::update()
{
    if(menustate==MenuState::PokittoSettings)
    {
        if(!pokittosettings->update())
        {
            delete pokittosettings;
            menustate=MenuState::MainMenu;
        }
        return true;
    }

    if(menustate==MenuState::SDBenchmark)
    {
        if(!sdbench->update())
        {
            delete sdbench;
            menustate=MenuState::MainMenu;
        }
        return true;
    }

    if(Pokitto::Buttons::repeat(BTN_UP,2))
    {
        if(m_selected > 0 )
           m_selected --;
    }else if(Pokitto::Buttons::repeat(BTN_DOWN,2))
    {
         if(m_selected < 4 )
           m_selected ++;
    }

    if(Pokitto::Buttons::pressed(BTN_A))
    {
        switch(m_selected)
        {
        case 0:
            {
                pokittosettings=new PokittoSettings;
                pokittosettings->init();
                menustate=MenuState::PokittoSettings;
            }
            break;
        case 1:
            Popup::Error("Error","Not implemented");
            break;
        case 2:
            if(Popup::YesNo("are you sure?", "You want to jump to loader"))
                jumpToLoader(0);
            break;
        case 3:

            if(dumpBootRom())
                Popup::InfoBox("Dump boot ROM","Dumping boot ROM completed successfully.");
            else
                Popup::Error("Error","Error Dumping Rom");
            break;
        case 4:
           {
                sdbench=new SDBench;
                if(sdbench->init())
                    menustate=MenuState::SDBenchmark;
                else
                    delete sdbench;
           }
            break;

        }
    }else if(Pokitto::Buttons::pressed(BTN_B))
    {
        return false;
    }

    return true;
}

void Settings::draw()
{
    if(menustate==MenuState::PokittoSettings)
    {
        pokittosettings->draw();
        return;
    }
    if(menustate==MenuState::SDBenchmark)
    {
        sdbench->draw();
        return;
    }
    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);

    std::string entries[]= {"Pokitto settings", "Xploritto settings", "Jump to loader", "Dump boot ROM", "SD Benchmark"};
    const unsigned char *icons [] = {Icons::Settings::Pokitto, Icons::Settings::Xploritto, Icons::Settings::JumpToLoader, Icons::EEPROM::Backup, Icons::Settings::SDBenchmark};

    for(size_t i = 0 ;i < 5; i++)
    {
        Pokitto::Display::setColor(WHITE);

        if(m_selected==i)
            Pokitto::Display::setColor(LIGHT_BLUE);


        Pokitto::Display::fillRect(5,STATUSBARHEIGHT+5+i*20,Pokitto::Display::width-10,20-2);
        if(m_selected==i)
            Pokitto::Display::setColor(WHITE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::setCursor(35, STATUSBARHEIGHT+11+i*20);
        Pokitto::Display::print(entries[i].c_str());

        Pokitto::Display::drawBitmap(10,STATUSBARHEIGHT+7+i*20,icons[i]);
    }
}

bool Settings::jumpToLoader(const char * arg)
{


    uint32_t* bootinfo = reinterpret_cast<uint32_t*>(0x3FFF4);
    if (*bootinfo != 0xB007AB1E)
        bootinfo = reinterpret_cast<uint32_t*>(0x3FF04); //allow couple of alternative locations
    if (*bootinfo != 0xB007AB1E)
        bootinfo = reinterpret_cast<uint32_t*>(0x3FE04); //allow couple of alternative locations
    if (*bootinfo != 0xB007AB1E)
        bootinfo = reinterpret_cast<uint32_t*>(0x3F004); //for futureproofing
    if (*bootinfo != 0xB007AB1E) {
        // no bootkey found at all
        Popup::Error("Error", "No loader installed");
        return false;
    } else {
        //loader was found
        Chip_TIMER_Disable(LPC_TIMER32_0);

        char* argPtr = reinterpret_cast<char*>(0x20000000);
        if(arg)
            strcpy(argPtr, arg);
        else
            *argPtr=0;

        //to do check if we should update the loader
        // disable sound interrupt before jumping to loader

        start_application(*(bootinfo+2)); //never returns

    }
    return true;
}

bool Settings::dumpBootRom()
{
    uint8_t* bootrom = reinterpret_cast<uint8_t*>(0x1FFF0000);
    FileHandle *brfile=0;

    brfile=sdFs->open("/Xploritto/BootRom.bin", O_CREAT | O_WRONLY);
    if(!brfile)
    {
        Popup::Error("Error","Open File failed");
        return false;
    }

    for(auto i=0;i<128;i++)
    {
        if(brfile->write(bootrom+i*256, 256) < 256)
            return false;

        Popup::progressBar("/Xploritto/BootRom.bin", (i*256*100)/(32*1024));
    }
    brfile->close();
    return true;
}
