#include "About.h"
#include "Icons.h"

bool About::update()
{
    if(Pokitto::Buttons::pressed(BTN_B) || Pokitto::Buttons::pressed(BTN_A))
        return false;
    return true;
}


void About::draw()
{
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);
    Pokitto::Display::drawBitmap(5,STATUSBARHEIGHT+5,Icons::About::Icon36);
    Pokitto::Display::setColor(LIGHT_BLUE);
    Pokitto::Display::setCursor(50,STATUSBARHEIGHT+10);
    Pokitto::Display::fontSize=2;
    Pokitto::Display::print("X");
    Pokitto::Display::setColor(BLACK);
    Pokitto::Display::print("ploritto");
    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fontSize=1;
    Pokitto::Display::setCursor(50,STATUSBARHEIGHT+30);
    Pokitto::Display::print(VERSION);
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+45);
    Pokitto::Display::setColor(BLACK);
    Pokitto::Display::print("Copyright(c) bl_ackrain, 2019");
    Pokitto::Display::setColor(LIGHT_BLUE);
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+57);
    Pokitto::Display::print(GITHUB_LINK);
    Pokitto::Display::drawLine(5,STATUSBARHEIGHT+70,210,STATUSBARHEIGHT+70);
    Pokitto::Display::setColor(BLACK);
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+75);
    Pokitto::Display::print("PokittoLib version: ");
    Pokitto::Display::print(POKITTO_LIBRARY_VERSION);

    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+75+10);
    Pokitto::Display::print("mbed version: ");
    Pokitto::Display::print(MBED_LIBRARY_VERSION);
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+75+20);
    Pokitto::Display::print("FatFs version: ");
    Pokitto::Display::print(_FFCONF);

    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRoundRect(75,155,70,15,3);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(86,155+4);
    Pokitto::Display::print("A: Close");

}
