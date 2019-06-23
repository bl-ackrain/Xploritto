#include "EEPROMManager.h"
#include "Popup.h"
#include "Icons.h"


bool EEPROMManager::update()
{
    if(menustate==MenuState::CookiesManager)
    {
        if(!cookiesmanager->update())
        {
            delete cookiesmanager;
            menustate=MenuState::MainMenu;
        }
        return true;
    }

    if(menustate==MenuState::HexEditor)
    {
        if(!hexeditor->update())
        {
            delete hexeditor;
            menustate=MenuState::MainMenu;
        }
        return true;
    }

    if(Pokitto::Buttons::repeat(BTN_UP,2))
    {
        if(m_selected > 0 )
           m_selected --;
    }else if(Pokitto::Buttons::repeat(BTN_DOWN,2))
    {
         if(m_selected < 4 )
           m_selected ++;
    }

    if(Pokitto::Buttons::pressed(BTN_A))
    {
        switch(m_selected)
        {
        case 0:
            {
                cookiesmanager=new CookiesManager;
                cookiesmanager->init();
                menustate=MenuState::CookiesManager;
            }
            break;
        case 1:
            if(backup())
                Popup::InfoBox("Backing up","Backup Complete.");
            else
                Popup::Error("Error","EEPROM Backup failed");
            break;
        case 2:
            {
                Popup::Loading("Searching for files..");
                DirHandle *dir = sdFs->opendir(EEPROM_FOLDER);
                if(!dir)
                {
                    Popup::Error("Error","Folder open failed");
                    break;
                }
                std::string filemenu[10];
                size_t fcount=0;
                for(auto i=0;i<10;)
                {
                    char *fileName=dir->readdir()->d_name;

                    if(!fileName)
                        break;

                    static FILINFO info;
                    std::string f=EEPROM_FOLDER;
                    f+= "/";
                    f+= fileName;
                    f_stat(f.c_str(), &info);
                    if(info.fattrib & AM_ARC)
                    {
                        if(f.find(".rom") != std::string::npos || f.find(".ROM") != std::string::npos)
                        {
                            filemenu[i++]=fileName;
                            fcount++;
                        }
                    }

                }
                dir->closedir();
                if(fcount==0)
                {
                    Popup::InfoBox("Restoring","0 Rom File found! put your file in "+std::string(EEPROM_FOLDER));
                    break;
                }
                if(fcount > 15)
                    fcount = 15;
                size_t ret=Popup::Menu("Choose a file",filemenu, 0, fcount);
                if(ret < fcount)
                {
                    std::string path=EEPROM_FOLDER;
                    path +="/";
                    path += filemenu[ret];

                    if(restore(path))
                        Popup::InfoBox("Restoring","Restore Complete.");
                    else
                        Popup::Error("Error","File Read failed");
                }

            }
            break;


        case 3:
            if(Popup::YesNo("Are you sure?","WARNING: Formatting will erase\nALL data on EEPROM"))
            {
                std::uint8_t buff[0x40];
                std::memset(buff,0,0x40);
                for(auto i=0; i < 0x40-1; i++)
                {
                    writeEEPROM(reinterpret_cast<uint16_t*>(i*0x40),&buff[0],0x40);
                    Popup::progressBar("Formatting", (100*i)/0x40);
                }
                Popup::InfoBox("Formatting EEPROM","Format Complete.");
            }
            break;
        case 4:
            {
                hexeditor=new HexEditor();
                hexeditor->loadEEPROM(0);
                menustate=MenuState::HexEditor;
            }
            break;

        }
    }else if(Pokitto::Buttons::pressed(BTN_B))
    {
        return false;
    }

    return true;
}

void EEPROMManager::draw()
{
    if(menustate==MenuState::CookiesManager)
    {
        cookiesmanager->draw();
        return;
    }
    if(menustate==MenuState::HexEditor)
    {
        hexeditor->draw();
        return;
    }
    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);

    const std::string entries[]= {"Cookies Manager", "Backup EEPROM", "Restore EEPROM", "Format EEPROM","EEPROM Hex Editor"};
    const unsigned char *icons [] = {Icons::EEPROM::CookiesManager, Icons::EEPROM::Backup, Icons::EEPROM::Restore, Icons::EEPROM::Format, Icons::EEPROM::HexEdit};

    for(auto i = 0 ;i < 5; i++)
    {
        Pokitto::Display::setColor(WHITE);

        if(m_selected==i)
            Pokitto::Display::setColor(LIGHT_BLUE);

        Pokitto::Display::fillRect(5,STATUSBARHEIGHT+5+i*20,Pokitto::Display::width-10,20-2);
        if(m_selected==i)
            Pokitto::Display::setColor(WHITE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::setCursor(35, STATUSBARHEIGHT+11+i*20);
        Pokitto::Display::print(entries[i].c_str());

        Pokitto::Display::drawBitmap(10,STATUSBARHEIGHT+7+i*20,icons[i]);
    }
}

bool EEPROMManager::backup()
{
    char fstr[64];
    FileHandle *epfile=0;
    for(auto i=0;;i++)
    {
        sprintf(fstr,"%s/EEPROM_%02u.rom",EEPROM_FOLDER,i);
        epfile=sdFs->open(fstr,O_RDONLY);
        if(!epfile)
        {
            epfile=sdFs->open(fstr, O_CREAT|O_WRONLY);
            break;
        }
        else
            epfile->close();
    }

    if(!epfile)
    {
        Popup::Error("Error","Open File failed");
        return false;
    }

    std::uint8_t buff[0x40];

    for(auto i=0; i < 0x40-1; i++)
    {
        readEEPROM(reinterpret_cast<uint16_t*>(i*0x40),&buff[0],0x40);

        if(epfile->write(buff,0x40) < 0x40)
        {
            Popup::Error("Error","File write failed");
            return false;
        }
        Popup::progressBar(std::string(fstr), (100*i)/0x40-1);
    }
    epfile->close();

    return true;
}

bool EEPROMManager::restore(const std::string path)
{
    FileHandle *epfile=sdFs->open(path.c_str(), O_RDONLY);

    if(!epfile)
        return false;

    std::uint8_t buff[0x40];

    for(auto i=0; i < 0x40-0x1; i++)
    {

        if(epfile->read(buff,0x40) < 0)
            return false;

        writeEEPROM(reinterpret_cast<uint16_t*>(i*0x40),&buff[0],0x40);

        std::string t="Restoring EEPROM ";
        Popup::progressBar(t, (100*i)/0x40);
    }
    epfile->close();

    return true;
}
