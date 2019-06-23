#pragma once
#include "globals.h"

class HexEditor
{
    static constexpr std::uint8_t MAXDATA=128;
    static constexpr std::uint16_t EEPROMSIZE=4069;

public:
    bool update();
    void draw();
    bool loadEEPROM(std::size_t start);
    bool saveEEPROM(std::size_t start);
    void nextChunk();
    void prevChunk();
    void saveNeeded();
private:
    std::uint8_t m_data[MAXDATA];
    std::uint8_t m_data_changed[MAXDATA];
    std::size_t m_datasize=0;
    std::size_t m_startadd=0;
    std::size_t m_cursor_x=0;
    std::size_t m_cursor_y=0;
};
