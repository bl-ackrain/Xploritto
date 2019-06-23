#include "Xploritto.h"
#include "Icons.h"

const uint8_t bl_ackrain[] =
{
    64,12,
    0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,
    0x90,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x30,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,
    0x03,0x30,0x03,0x33,0x30,0x03,0x33,0x33,0x03,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x03,0x30,0x03,0x33,0x30,0x03,0x33,0x30,0x30,0x33,0x33,0x00,0x00,0x30,0x00,0x00,0x30,0x03,0x33,0x33,0x09,0x03,0x33,0x33,0x00,0x33,0x33,0x03,0x30,0x33,0x33,0x30,
    0x03,0x30,0x03,0x33,0x30,0x03,0x33,0x30,0x30,0x33,0x33,0x33,0x00,0x30,0x03,0x00,0x30,0x03,0x00,0x33,0x09,0x03,0x30,0x33,0x00,0x00,0x33,0x00,0x00,0x33,0x03,0x30,
    0x03,0x30,0x00,0x00,0x30,0x03,0x33,0x03,0x33,0x03,0x30,0x00,0x00,0x30,0x03,0x33,0x30,0x00,0x33,0x33,0x09,0x03,0x30,0x00,0x03,0x33,0x33,0x03,0x30,0x33,0x03,0x30,
    0x03,0x30,0x03,0x00,0x30,0x03,0x33,0x03,0x33,0x03,0x30,0x03,0x00,0x30,0x03,0x33,0x30,0x03,0x00,0x33,0x09,0x03,0x30,0x99,0x03,0x30,0x33,0x03,0x30,0x33,0x03,0x30,
    0x03,0x30,0x03,0x00,0x30,0x03,0x33,0x30,0x00,0x33,0x30,0x03,0x00,0x30,0x03,0x33,0x30,0x03,0x00,0x33,0x09,0x03,0x30,0x99,0x03,0x30,0x33,0x03,0x30,0x33,0x03,0x30,
    0x03,0x30,0x03,0x00,0x30,0x03,0x33,0x33,0x33,0x33,0x30,0x03,0x00,0x30,0x03,0x00,0x30,0x03,0x00,0x33,0x09,0x03,0x30,0x99,0x03,0x30,0x33,0x03,0x30,0x33,0x03,0x30,
    0x03,0x30,0x00,0x00,0x30,0x00,0x30,0x00,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x03,0x00,0x33,0x09,0x03,0x30,0x99,0x03,0x33,0x33,0x03,0x30,0x33,0x03,0x30,
    0x90,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x30,0x99,0x00,0x00,0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99
 };

uint16_t popPalette[16] =
{
    0x18e4,
    0x4a8a,
    0xa513,
    0xffff,
    0x2173,
    0x52bf,
    0x0d11,
    0x3d9b,
    0x8915,
    0xf494,
    0xa102,
    0xfc23,
    0xfee7,
    0x7225,
    0x53e2,
    0x8663
};

SDFileSystem *sdFs;

void Xploritto::init()
{
    Pokitto::Core::begin();
    Pokitto::Sound::ampEnable(0);
    Pokitto::Core::setFrameRate(255);
    Pokitto::Display::setFont(font5x7);
    Pokitto::Display::load565Palette(popPalette);
    Pokitto::Display::setInvisibleColor(PINK);
    Pokitto::Display::setColor(WHITE,PINK);
    Pokitto::Display::persistence=true;
    Pokitto::Display::fillScreen(DARK_GRAY);
    Pokitto::Display::drawBitmap(78, 82, bl_ackrain);
    Pokitto::Display::update();

    sdFs =new SDFileSystem( P0_9, P0_8, P0_6, P0_7, "sd", NC, SDFileSystem::SWITCH_NONE, 25000000 );
    sdFs->crc(false);
    sdFs->write_validation(false);
    //sdFs->large_frames(true);

    if(sdFs->card_type()==SDFileSystem::CardType::CARD_UNKNOWN)
    {
        Popup::Error("Error", "SD card initialization failed!");
        return;
    }


    DirHandle *dir = sdFs->opendir(XPLORITTO_FOLDER);

    if(dir)
        dir->closedir();
    else
    {
        FRESULT res=f_mkdir(XPLORITTO_FOLDER);
        if( res != FR_OK)
            Popup::Error("Error ","mkdir error  "+std::string(XPLORITTO_FOLDER), res);
    }

    dir =sdFs->opendir(EEPROM_FOLDER);

    if(dir)
        dir->closedir();
    else
    {
        FRESULT res=f_mkdir(EEPROM_FOLDER);
        if( res != FR_OK)
            Popup::Error("Error ","mkdir error  "+std::string(EEPROM_FOLDER), res);
    }

    dir =sdFs->opendir(COOKIES_FOLDER);

    if(dir)
        dir->closedir();
    else
    {
        FRESULT res=f_mkdir(COOKIES_FOLDER);
        if( res != FR_OK)
            Popup::Error("Error ","mkdir error  "+std::string(COOKIES_FOLDER), res);
    }
}

void Xploritto::run()
{
    size_t t_intro = 10;

    while(true)
    {
        if(!Pokitto::Core::update())
            continue;

        if(t_intro)
        {
            t_intro--;
            continue;
        }

        statusbar.update();
        statusbar.draw();

        if(menustate==MenuState::MainMenu)
        {
            //update
            if(Pokitto::Buttons::repeat(BTN_UP,2))
            {
                if(m_menuselected > 2 )
                   m_menuselected -=3;
            }else if(Pokitto::Buttons::repeat(BTN_DOWN,2))
            {
                 if(m_menuselected < 2 )
                   m_menuselected +=3;
            }else if(Pokitto::Buttons::repeat(BTN_LEFT,2))
            {
                 if(m_menuselected > 0)
                   m_menuselected --;
            }else if(Pokitto::Buttons::repeat(BTN_RIGHT,2))
            {
                 if(m_menuselected < 4 )
                   m_menuselected ++;
            }
            if(Pokitto::Buttons::pressed(BTN_A))
            {
                menustate =static_cast<MenuState> (m_menuselected+1);
                if(menustate==MenuState::SDBrowser)
                {
                    sdbrowse =new SDBrowse();
                    sdbrowse->init();
                }

                if(menustate==MenuState::EEPROMManager)
                {
                    eeprommanger =new EEPROMManager();
                }

                if(menustate==MenuState::Settings)
                {
                    settings =new Settings();
                }

                if(menustate==MenuState::About)
                {
                    about =new About();
                }

                if(menustate==MenuState::Help)
                {
                    help =new Help();
                }
                continue;
            }

            //draw
            Pokitto::Display::setColor(DARK_GRAY);
            Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);


            Pokitto::Display::setColor(LIGHT_BLUE);
            Pokitto::Display::fillRect(26+(m_menuselected%3)*60, STATUSBARHEIGHT+10+(m_menuselected/3)*70,48,48);

            Pokitto::Display::setColor(WHITE);
            Pokitto::Display::drawBitmap(26, STATUSBARHEIGHT+10, Icons::MainMenu::SDCard);
            Pokitto::Display::setCursor(26,STATUSBARHEIGHT+10+52);
            Pokitto::Display::print("SD Card");

            Pokitto::Display::drawBitmap(26+60, STATUSBARHEIGHT+10, Icons::MainMenu::EEPROM);
            Pokitto::Display::setCursor(26+60,STATUSBARHEIGHT+10+52);
            Pokitto::Display::print("EEPROM");

            Pokitto::Display::drawBitmap(26+120, STATUSBARHEIGHT+10, Icons::MainMenu::Settings);
            Pokitto::Display::setCursor(26+120,STATUSBARHEIGHT+10+52);
            Pokitto::Display::print("Settings");

            Pokitto::Display::drawBitmap(26, STATUSBARHEIGHT+80, Icons::MainMenu::Help);
            Pokitto::Display::setCursor(26,STATUSBARHEIGHT+80+52);
            Pokitto::Display::print("Help");

            Pokitto::Display::drawBitmap(26+60, STATUSBARHEIGHT+80, Icons::MainMenu::About);
            Pokitto::Display::setCursor(26+60,STATUSBARHEIGHT+80+52);
            Pokitto::Display::print("About");

            Pokitto::Display::setFont(font3x5);
            Pokitto::Display::setColor(LIGHT_GRAY);
            Pokitto::Display::setCursor(Pokitto::Display::width-5-strlen(VERSION)*4,Pokitto::Display::height-10);
            Pokitto::Display::print(VERSION);
            Pokitto::Display::setFont(font5x7);
        }
        else if(menustate==MenuState::SDBrowser)
        {
            if(sdbrowse->update())
                sdbrowse->draw();
            else
            {
                menustate=MenuState::MainMenu;
                delete sdbrowse;
            }

        }
        else if(menustate==MenuState::EEPROMManager)
        {
            if(eeprommanger->update())
                eeprommanger->draw();
            else
            {
                menustate=MenuState::MainMenu;
                delete eeprommanger;
            }
        }
        else if(menustate==MenuState::Settings)
        {
            if(settings->update())
                settings->draw();
            else
            {
                menustate=MenuState::MainMenu;
                delete settings;
            }

        }else if(menustate==MenuState::Help)
        {
            if(help->update())
                help->draw();
            else
            {
                menustate=MenuState::MainMenu;
                delete help;
            }
        }
        else if(menustate==MenuState::About)
        {
            if(about->update())
                about->draw();
            else
            {
                menustate=MenuState::MainMenu;
                delete about;
            }
        }

    }

}
