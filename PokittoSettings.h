#pragma once
#include "globals.h"

class PokittoSettings
{
public:
    void init();
    bool update();
    void draw();
    static time_t time_to_epoch ( const struct tm *ltm, const int utcdiff );

private:
    std::size_t m_selected=0;
    std::uint8_t m_volume;
    std::uint8_t m_loaderwait;
    std::uint8_t m_volumewait;
    std::uint8_t m_timeformat;
    std::uint8_t m_dateformat;
};
