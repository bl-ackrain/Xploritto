#pragma once
#include <cstdint>

namespace Icons
{

namespace MainMenu
{
extern const std::uint8_t SDCard [];
extern const std::uint8_t EEPROM [];
extern const std::uint8_t Settings [];
extern const std::uint8_t Help [];
extern const std::uint8_t About [];
}

namespace Files
{
extern const std::uint8_t Folder[];
extern const std::uint8_t File[];
extern const std::uint8_t Bin[];
extern const std::uint8_t PokiBin[];
extern const std::uint8_t Pop[];
extern const std::uint8_t Snd[];
extern const std::uint8_t Wav[];
extern const std::uint8_t Txt[];
extern const std::uint8_t Der[];
extern const std::uint8_t Rom[];
extern const std::uint8_t Bmp[];
}

namespace Actions
{
extern const std::uint8_t Info [];
extern const std::uint8_t Open [];
extern const std::uint8_t Copy [];
extern const std::uint8_t Move [];
extern const std::uint8_t DeleteFile [];
extern const std::uint8_t Rename [];
extern const std::uint8_t NewFile[];
extern const std::uint8_t NewFolder [];
extern const std::uint8_t Backup [];
extern const std::uint8_t Restore [];
extern const std::uint8_t Delete [];
}

namespace EEPROM
{
extern const std::uint8_t Backup [];
extern const std::uint8_t Restore [];
extern const std::uint8_t Format [];
extern const std::uint8_t CookiesManager [];
extern const std::uint8_t Cookie [];
extern const std::uint8_t HexEdit [];
}

namespace Settings
{
extern const std::uint8_t Xploritto [];
extern const std::uint8_t Pokitto [];
extern const std::uint8_t JumpToLoader [];
extern const std::uint8_t SDBenchmark [];
}

namespace About
{
extern const std::uint8_t Icon36[];
}
}// Icons namespace
