#pragma once
#include "globals.h"

class SoundPlayer
{
public:
    SoundPlayer(std::string filename);
    bool open(std::string filename);
    bool update();
    void draw();
private:
    bool m_redraw=true;
    std::string m_filename;
    size_t m_duration=0;
    size_t m_elapsedSec=0;
    uint8_t m_volume;
    bool m_playing=false;
};
