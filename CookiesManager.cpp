#include "CookiesManager.h"
#include "Popup.h"
#include "Icons.h"

void CookiesManager::init()
{
    Popup::Loading("Loading Cookies..");
    readEEPROM(reinterpret_cast<std::uint16_t*>(0), reinterpret_cast<std::uint8_t*>(&m_keystable[0]), SBKEYSIZE*SBMAXKEYS);
    readEEPROM(reinterpret_cast<std::uint16_t*>(SBKEYSIZE*SBMAXKEYS), &m_blockstable[0], SBMAXBLOCKS);

    updateCookies();

    m_redraw=true;
}

void CookiesManager::updateCookies()
{
    m_cookiescount=0;
    m_cookiestotalsize=0;
    std::memset(m_sizetable,0,SBMAXKEYS);
    auto j=0;

    for(auto i=0;i<SBMAXKEYS;i++)
    {
        if(m_keystable[i*SBKEYSIZE])
        {
            m_cookiescount++;
            m_indextable[j++]=i;
        }

    }

    for(auto i=0;i<SBMAXBLOCKS;i++)
    {
        if(m_blockstable[i])
        {
            m_sizetable[m_blockstable[i] & 0x7F]++;
            m_cookiestotalsize++;
        }
    }
}

bool CookiesManager::backupCookie(size_t numbre)
{
    char fstr[64];
    FileHandle *ckfile=0;
    char _key[SBKEYSIZE+1];
    _key[SBKEYSIZE]=0;
    std::memcpy(_key, &m_keystable[SBKEYSIZE*numbre], SBKEYSIZE);

    for(auto i=0;;i++)
    {
        sprintf(fstr,"%s/%s_%02u.pck", COOKIES_FOLDER,_key, i);
        ckfile=sdFs->open(fstr,O_RDONLY);
        if(!ckfile)
        {
            ckfile=sdFs->open(fstr, O_CREAT|O_WRONLY);
            break;
        }
        else
            ckfile->close();
    }

    if(ckfile->write(&m_keystable[SBKEYSIZE*numbre], SBKEYSIZE) < 0)
        return false;

    if(ckfile->write(&m_sizetable[numbre], sizeof(size_t)) < 0)
        return false;

    std::uint8_t buff[SBBLOCKSIZE];

    std::size_t count=0;

    for(auto j=0;j<SBMAXBLOCKS;j++)
    {
        if(m_blockstable[j])
        {
            if((m_blockstable[j] & 0x7F)==numbre)
            {
                count++;
                readEEPROM(reinterpret_cast<std::uint16_t*>(SBKEYSIZE*SBMAXKEYS+SBMAXBLOCKS+j*SBBLOCKSIZE),&buff[0],SBBLOCKSIZE);
                if(ckfile->write(buff,SBBLOCKSIZE) < 0)
                    return false;
            }
        }
    }
    if(count!=m_sizetable[numbre])
        return false;

    ckfile->close();

    return true;
}

bool CookiesManager::restoreCookie(std::string path)
{
    FileHandle *ckfile=sdFs->open(path.c_str(), O_RDONLY);

    if(!ckfile)
        return false;

    char _key[SBKEYSIZE+1];
    _key[SBKEYSIZE]=0;
    if(ckfile->read(_key,SBKEYSIZE) < 0)
        return false;

    size_t csize=0;
    if(ckfile->read(&csize,sizeof(size_t)) < 0)
        return false;

    std::uint8_t buff[HARDCODEDOFFSET+csize*SBBLOCKSIZE];


    if(ckfile->read(&buff[HARDCODEDOFFSET], csize*SBBLOCKSIZE) < 0)
        return false;

    Pokitto::Cookie tmpCookie;
    tmpCookie.begin(&_key[0],csize*SBBLOCKSIZE,(char *)&buff[0]);

    tmpCookie.saveCookie();

    ckfile->close();

    return true;
}

void CookiesManager::deleteCookie(size_t numbre)
{
    std::uint8_t buff[SBBLOCKSIZE];
    std::memset(buff,0,SBBLOCKSIZE);
    for (int i=0; i<SBMAXBLOCKS; i++) {
        size_t blockid = (m_blockstable[i] & 0x7F);

        if ( blockid == numbre)
        {
            eeprom_write_byte(reinterpret_cast<std::uint16_t*>(SBKEYSIZE*SBMAXKEYS+i),0);
            writeEEPROM(reinterpret_cast<std::uint16_t*>(SBKEYSIZE*SBMAXKEYS+SBMAXBLOCKS+i*SBBLOCKSIZE), buff, SBBLOCKSIZE);
        }
    }

    writeEEPROM(reinterpret_cast<std::uint16_t*>(SBKEYSIZE*numbre), buff, SBKEYSIZE);
}

bool CookiesManager::update()
{
    if(Pokitto::Buttons::repeat(BTN_RIGHT,5) && m_cookiescount > 6)
    {
        if(m_begincookie < m_cookiescount-6 )
        {
            m_begincookie+=6;
            m_selectedcookie=0;
            m_redraw=true;
        }
        return true;
    }

    if(Pokitto::Buttons::repeat(BTN_LEFT,5) && m_cookiescount > 6)
    {
        if(m_begincookie > 0)
        {
            m_begincookie -= 6;
            m_selectedcookie=0;
            m_redraw=true;
        }
        return true;
    }

    if(Pokitto::Buttons::repeat(BTN_UP,1))
    {
        if(m_selectedcookie==0)
        {
            if(m_begincookie >= 6)
            {
                m_selectedcookie=5;
                m_begincookie-=6;
            }
            else
                return true;
        }
        else
            m_selectedcookie--;
       m_redraw=true;
       return true;
    }

    if(Pokitto::Buttons::repeat(BTN_DOWN,1))
    {
        if(m_selectedcookie+m_begincookie >= m_cookiescount-1)
            return true;

        m_selectedcookie++;

        if(m_selectedcookie == 6)
        {
            m_selectedcookie=0;
            m_begincookie+=6;
        }

        m_redraw=true;
        return true;
    }

    if(Pokitto::Buttons::pressed(BTN_A) || Pokitto::Buttons::pressed(BTN_C))
    {
        std::string cookiemenu[]={"Restore Cookie", "Backup Cookie", "Delete Cookie"};
        const uint8_t * menuicons[]={Icons::Actions::Restore, Icons::Actions::Backup, Icons::Actions::Delete};
        size_t ret=Popup::Menu("Cookie menu", cookiemenu, menuicons, m_cookiescount ==0?1:3);

        switch(ret)
        {
        case 0:
            {
                Popup::Loading("Searching for files..");
                DirHandle *dir = sdFs->opendir(COOKIES_FOLDER);
                std::string filemenu[10];
                size_t fcount=0;
                for(size_t i=0;i<10;)
                {
                    char *fileName=dir->readdir()->d_name;
                    if(!fileName)
                        break;
                    static FILINFO info;
                    std::string f=COOKIES_FOLDER;
                    f +="/";
                    f += fileName;
                    f_stat(f.c_str(), &info);
                    if(info.fattrib & AM_ARC)
                    {
                        if(f.find(".pck") != std::string::npos || f.find(".PCK") != std::string::npos)
                        {
                            filemenu[i++]=fileName;
                            fcount++;
                        }
                    }

                }
                dir->closedir();
                if(fcount==0)
                {
                    Popup::InfoBox("Restoring","0 Cookies File found! put your file in "+std::string(COOKIES_FOLDER));
                    break;
                }
                if(fcount > 15)
                    fcount = 15;
                size_t ret=Popup::Menu("Choose a file",filemenu, 0, fcount);
                if(ret < fcount)
                {
                    std::string path=COOKIES_FOLDER;
                    path +="/";
                    path += filemenu[ret];
                    if(restoreCookie(path))
                        Popup::InfoBox("Restoring","Restore Complete.");
                    else
                        Popup::Error("Error","File Read failed");
                    init();
                }

            }
            break;
        case 1:

            if(backupCookie(m_indextable[m_selectedcookie+m_begincookie]))
                Popup::InfoBox("Backup Cookie","Cookie backed up Successfully");
            else
                Popup::Error("Error","Backing up Cookie");
            break;
        case 2:
            deleteCookie(m_indextable[m_selectedcookie+m_begincookie]);
            init();
            m_selectedcookie=0;
            Popup::InfoBox("Delete Cookie","Cookie deleted Successfully");
            break;
        };
        m_redraw=true;
        return true;
    }
    else if(Pokitto::Buttons::pressed(BTN_B))
    {
        return false;
    }

    return true;
}

void CookiesManager::draw()
{
    if(!m_redraw)
        return;

    Pokitto::Display::setColor(DARK_GRAY);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,40);

    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fillRoundRect(5,STATUSBARHEIGHT+3,Pokitto::Display::width-10,16,2);
    Pokitto::Display::setColor(GREEN);
    if(m_cookiescount > 0)
        Pokitto::Display::fillRoundRect(5,STATUSBARHEIGHT+3,((Pokitto::Display::width-10)*m_cookiescount)/SBMAXKEYS,16,2);

    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fillRoundRect(5,STATUSBARHEIGHT+5+16,Pokitto::Display::width-10,16,2);
    Pokitto::Display::setColor(GREEN);
    if(m_cookiestotalsize > 0)
        Pokitto::Display::fillRoundRect(5,STATUSBARHEIGHT+5+16,((Pokitto::Display::width-10)*m_cookiestotalsize)/SBMAXBLOCKS,16,2);

    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(15, STATUSBARHEIGHT+8);
    Pokitto::Display::printf("Cookies %02u/%02u", m_cookiescount, SBMAXKEYS);
    Pokitto::Display::setCursor(15, STATUSBARHEIGHT+10+16);
    Pokitto::Display::printf("Size %u B / %u B", m_cookiestotalsize*SBBLOCKSIZE,SBMAXBLOCKS*SBBLOCKSIZE);


    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT+40,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT-40);

    Pokitto::Display::setColor(DARK_GRAY);
    Pokitto::Display::fillRect(Pokitto::Display::width-3,STATUSBARHEIGHT+43,2, 118);

    if(m_cookiescount > 6)
    {
        Pokitto::Display::setColor(WHITE);
        Pokitto::Display::fillRect(Pokitto::Display::width-3,STATUSBARHEIGHT+43+(118/((m_cookiescount+5)/6))*m_begincookie/6,2,118/((m_cookiescount+5)/6));
    }

    for(size_t i = 0 ;i < 6; i++)
    {
        if(i+m_begincookie >= m_cookiescount)
            break;

        Pokitto::Display::setColor(WHITE);

        if(m_selectedcookie==i)
            Pokitto::Display::setColor(LIGHT_BLUE);

        Pokitto::Display::fillRect(5,STATUSBARHEIGHT+40+3+i*20,Pokitto::Display::width-10,20-2);

        if(m_selectedcookie==i)
            Pokitto::Display::setColor(WHITE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::setCursor(35, STATUSBARHEIGHT+40+9+i*20);
        char _key[SBKEYSIZE+1];
        _key[SBKEYSIZE]=0;
        std::memcpy(_key, &m_keystable[SBKEYSIZE*m_indextable[i+m_begincookie]], SBKEYSIZE);
        Pokitto::Display::print(_key);

        if(m_selectedcookie==i)
            Pokitto::Display::setColor(DARK_GRAY);
        else
            Pokitto::Display::setColor(LIGHT_GRAY);

        Pokitto::Display::setCursor(Pokitto::Display::width-40, STATUSBARHEIGHT+40+9+i*20);
        Pokitto::Display::printf("%u B",m_sizetable[m_indextable[i+m_begincookie]]*SBBLOCKSIZE);
        Pokitto::Display::drawBitmap(10, STATUSBARHEIGHT+40+4+i*20, Icons::EEPROM::Cookie);
    }

    Pokitto::Display::update();
    m_redraw=false;
}
