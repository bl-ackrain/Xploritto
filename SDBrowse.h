#pragma once
#include "globals.h"
    #include "SoundPlayer.h"


class SDBrowse
{
public:
    enum class MenuState
    {
        MainMenu=0,
        SoundPlayer
    };

    SDBrowse();
    void init();
    bool update();
    void draw();
    size_t fileCount(std::string path);
    MenuState menustate=MenuState::MainMenu;
    SoundPlayer *soundplayer;
private:
    size_t m_beginitem=0;
    size_t m_selecteditem=0;
    size_t m_oldbeginitem=0;
    size_t m_oldselecteditem=0;
    size_t m_itemcount=0;
    bool m_redraw=true;
    std::string m_curDir;
    std::string m_movepath;
    bool m_movemode=false;
};
