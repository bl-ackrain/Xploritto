#include "Help.h"

bool Help::update()
{
    if(Pokitto::Buttons::pressed(BTN_B) || Pokitto::Buttons::pressed(BTN_A))
        return false;
    return true;
}

void Help::draw()
{
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+5);
    Pokitto::Display::setColor(BLACK);
    Pokitto::Display::print("Arrows buttons to change selection");
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+15);
    Pokitto::Display::print("A button to open/confirm");
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+25);
    Pokitto::Display::print("B button to return");
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+35);
    Pokitto::Display::print("C button for context menu");
}
