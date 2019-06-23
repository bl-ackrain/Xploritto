#pragma once
#include "globals.h"
#include "CookiesManager.h"
#include "HexEditor.h"

class EEPROMManager
{
public:
    enum class MenuState
    {
        MainMenu=0,
        CookiesManager,
        HexEditor
    };

    bool update();
    void draw();
    static bool backup();
    static bool restore(const std::string path);
    MenuState menustate=MenuState::MainMenu;
    CookiesManager *cookiesmanager;
    HexEditor *hexeditor;
private:
    size_t m_selected;
};
