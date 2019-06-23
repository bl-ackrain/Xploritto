#include "HexEditor.h"
#include "Popup.h"

bool HexEditor::loadEEPROM(size_t start)
{
    if(start==0xF80)
    {
        readEEPROM(reinterpret_cast<uint16_t*>(start), &m_data[0], MAXDATA/2);//the last 64 bits are protected
        std::memset(&m_data[MAXDATA/2],0,MAXDATA/2);
    }
    else
        readEEPROM(reinterpret_cast<uint16_t*>(start), &m_data[0], MAXDATA);

    if(start==0xF80)
        m_datasize=MAXDATA/2;
    else
        m_datasize=MAXDATA;
    m_startadd=start;
    std::memset(m_data_changed,0,MAXDATA);
    return true;
}

bool HexEditor::saveEEPROM(size_t start)
{
    for(auto i=0;i<MAXDATA ;i++)
    {
        if(i >= m_datasize)
            break;

        if(m_data_changed[i]==1)
        {
           eeprom_write_byte(reinterpret_cast<uint16_t*>(start+i), m_data[i]);
        }

    }
    std::memset(m_data_changed,0,MAXDATA);

    return true;
}

void HexEditor::nextChunk()
{
    if(m_startadd < EEPROMSIZE-MAXDATA)
    {
        saveNeeded();
        loadEEPROM(m_startadd+MAXDATA);
        m_cursor_y=0;
    }
}

void HexEditor::prevChunk()
{
    if(m_startadd >= MAXDATA)
    {
        saveNeeded();
        loadEEPROM(m_startadd-MAXDATA);
        m_cursor_y=15;
    }
}

void HexEditor::saveNeeded()
{
    bool modified=false;
    for(auto i=0;i<MAXDATA;i++)
        if(m_data_changed[i])
        {
            modified=true;
            break;
        }
    if(!modified)
        return;
    if(Popup::YesNo("EEPROM Modified","Do you want to save the changes made to this chunk of EEPROM?"))
        saveEEPROM(m_startadd);
}

bool HexEditor::update()
{
    if(Pokitto::Buttons::timeHeld(BTN_A) < 1)
    {
        if(Pokitto::Buttons::repeat(BTN_UP,2))
        {
            if(m_cursor_y > 0)
                m_cursor_y--;
            else
                prevChunk();
            return true;
        }

        if(Pokitto::Buttons::repeat(BTN_DOWN,2))
        {
            if(m_cursor_y < 15)
                m_cursor_y++;
            else
                nextChunk();
            return true;
        }
    }else
    {
        if(Pokitto::Buttons::repeat(BTN_UP,2))
        {
            if(m_cursor_x%2)
                m_data[m_cursor_y*8+(m_cursor_x>>1)]++;
            else
                m_data[m_cursor_y*8+(m_cursor_x>>1)]+=0x10;

            m_data_changed[m_cursor_y*8+(m_cursor_x>>1)]=1;
            return true;
        }

        if(Pokitto::Buttons::repeat(BTN_DOWN,2))
        {
            if(m_cursor_x%2)
                m_data[m_cursor_y*8+(m_cursor_x>>1)]--;
            else
                m_data[m_cursor_y*8+(m_cursor_x>>1)]-=0x10;

            m_data_changed[m_cursor_y*8+(m_cursor_x>>1)]=1;
            return true;
        }

    }

    if(Pokitto::Buttons::repeat(BTN_RIGHT,2) && m_cursor_x < 15)
        m_cursor_x++;

    if(Pokitto::Buttons::repeat(BTN_LEFT,2) && m_cursor_x > 0)
        m_cursor_x--;

    if(Pokitto::Buttons::pressed(BTN_C))
    {
        std::string menu[5];
        menu[0] = "Save & Exit";
        menu[1] = "Save";
        menu[2] = "Reload";
        menu[3] = "Next chunk";
        menu[4] = "Prev chunk";

        //const unsigned char * menuicons[]={};
        size_t ret=Popup::Menu("Actions", menu, 0, 5);
        switch(ret)
        {
        case 0:
            if(saveEEPROM(m_startadd))
                return false;
            break;
        case 1:
            saveEEPROM(m_startadd);
            break;
        case 2:
            loadEEPROM(m_startadd);
            break;
        case 3:
            nextChunk();
            break;
        case 4:
            prevChunk();
            break;
        }
        return true;
    }

    if(Pokitto::Buttons::pressed(BTN_B))
    {
        saveNeeded();
        return false;
    }

    return true;
}

void HexEditor::draw()
{
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::fillRect(38,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);

    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,38,Pokitto::Display::height-STATUSBARHEIGHT);

    Pokitto::Display::setColor(WHITE);

    for(auto i=0;i<16;i++)
    {
        Pokitto::Display::setCursor(1,STATUSBARHEIGHT+3+i*10);
        Pokitto::Display::printf("%06x", (i*8)+m_startadd);
    }

    Pokitto::Display::setColor(LIGHT_BLUE);
    Pokitto::Display::fillRect(40-1+m_cursor_x*7+((m_cursor_x%2)?-1:0),STATUSBARHEIGHT+1+m_cursor_y*10,7,10);
    Pokitto::Display::fillRect(38-1+125+(m_cursor_x>>1)*7, STATUSBARHEIGHT+3+m_cursor_y*10, 7, 10);

    Pokitto::Display::setColor(BLACK);

    for(auto j=0;j<16;j++)
    {
        for(auto i=0;i<8;i++)
        {
            if(m_data_changed[j*8+i])
                Pokitto::Display::setColor(ORANGE);
            else
                Pokitto::Display::setColor(BLACK);

            Pokitto::Display::setCursor(40+i*14,STATUSBARHEIGHT+3+j*10);
            Pokitto::Display::printf("%02x", m_data[i+j*8]);

            if((i==m_cursor_x>>1) && (j==m_cursor_y) )
            {
                Pokitto::Display::setCursor(40+i*14+(m_cursor_x % 2?6:0),STATUSBARHEIGHT+3+j*10);
                Pokitto::Display::setColor(WHITE);
                if(m_cursor_x % 2)
                    Pokitto::Display::printf("%01x", m_data[i+j*8]&0xF);
                else
                    Pokitto::Display::printf("%01x", m_data[i+j*8]>>4);
            }

        }

        for(auto i=0;i<8;i++)
        {

            if((i==m_cursor_x>>1) && (j==m_cursor_y) )
                Pokitto::Display::setColor(WHITE);
            else if(m_data_changed[j*8+i])
                Pokitto::Display::setColor(ORANGE);

            Pokitto::Display::setCursor(38+125+i*7,STATUSBARHEIGHT+3+j*10);
            Pokitto::Display::print(static_cast<char>(m_data[i+j*8]<128&&m_data[i+j*8]>31?m_data[i+j*8]:'.'));
            Pokitto::Display::setColor(BLACK);
        }
    }
}
