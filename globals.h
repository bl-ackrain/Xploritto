#pragma once
#define VERSION "v0.1.0"
#define GITHUB_LINK "github.com/bl-ackrain/xploritto"

#define POKITTO_LIBRARY_VERSION "25-05-2019"

#define XPLORITTO_FOLDER "/Xploritto"
#define EEPROM_FOLDER "/Xploritto/EEPROM"
#define COOKIES_FOLDER "/Xploritto/Cookies"

#define STATUSBARHEIGHT 10

#include "Pokitto.h"
#include "SDFileSystem.h"

#ifdef max
    #undef max
#endif

#ifdef min
    #undef min
#endif

#include <string>
#include <vector>

//palette colors

#define BLACK       0
#define DARK_GRAY   1
#define LIGHT_GRAY  2
#define WHITE       3
#define DARK_BLUE   4
#define BLUE        5
#define CAYAN       6
#define LIGHT_BLUE  7
#define PURPLE      8
#define PINK        9
#define RED         10
#define ORANGE      11
#define YELLOW      12
#define BROWN       13
#define DARK_GREEN  14
#define GREEN       15

extern SDFileSystem *sdFs;
extern uint16_t popPalette[16];
