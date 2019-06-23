#pragma once
#include "globals.h"
#include "StatusBar.h"
#include "SDBrowse.h"
#include "EEPROMManager.h"
#include "Popup.h"
#include "Settings.h"
#include "About.h"
#include "Help.h"

class Xploritto
{
public:
    enum class MenuState
    {
        MainMenu=0,
        SDBrowser,
        EEPROMManager,
        Settings,
        Help,
        About,
    };

    void init();
    void run();
    StatusBar statusbar;
    SDBrowse *sdbrowse;
    EEPROMManager *eeprommanger;
    Settings *settings;
    About *about;
    Help *help;
    MenuState menustate=MenuState::MainMenu;
private:
    size_t m_menuselected;
};
