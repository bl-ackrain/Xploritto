#pragma once
#include "globals.h"
#include "PokittoSettings.h"
#include "SDBench.h"

extern void start_application(unsigned long app_link_location);

class Settings
{
public:
    enum class MenuState
    {
        MainMenu=0,
        PokittoSettings,
        XplorittoSettings,
        SDBenchmark
    };
    bool update();
    void draw();
    static bool jumpToLoader(const char * arg);
    bool dumpBootRom();
    MenuState menustate=MenuState::MainMenu;
    PokittoSettings *pokittosettings;
    SDBench *sdbench;
private:
    size_t m_selected=0;
};
