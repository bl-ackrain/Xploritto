#include "Pop.h"
#include "HWLCD.h"

extern void write_data_16(uint16_t data);

FileHandle *Pop::popFile=nullptr;
Pop::Tag Pop::m_Tag;

bool Pop::open(const std::string path)
{
    if(popFile)
        close();
    popFile=sdFs->open(path.c_str(),O_RDONLY);
}

void Pop::close()
{
    if(popFile)
    {
        popFile->close();
        popFile=nullptr;
    }
}

bool Pop::findTag(const PopTag targetTag, bool rewind)
{
    if( !popFile )
        return false;
    if(rewind)
        popFile->lseek(0,SEEK_SET);

    while( popFile->read( &m_Tag, sizeof(m_Tag)) ){

        if( m_Tag.id == targetTag ){
            return true;
        }else if( m_Tag.id == PopTag::TAG_CODE ){
            break;
        }else{
            popFile->lseek(m_Tag.lenght, SEEK_CUR);
        }
    }
    return false;
}

std::string Pop::getAuthor()
{
    if(!findTag(PopTag::TAG_AUTHOR))
        return "";

    std::string author;
    while(true)
    {
        char c=0;
        popFile->read(&c,1);
        if(!c)
            break;
        author+=c;
    }
    return author;
}

std::string Pop::getLongName()
{
    if(!findTag(PopTag::TAG_LONGNAME))
        return "";

    std::string name;

    while(true)
    {
        char c=0;
        popFile->read(&c,1);
        if(!c)
            break;
        name+=c;

    }
    return name;
}

std::string Pop::getDescription()
{
    if(!findTag(PopTag::TAG_DESCRIPTION))
        return "";

    std::string desc;

    while(true)
    {
        char c=0;
        popFile->read(&c,1);
        if(!c)
            break;
        desc+=c;

    }
    return desc;
}

bool Pop::drawIcon24(const std::uint16_t x, const std::uint16_t y)
{
    if(!findTag(PopTag::TAG_IMG_24X24_4BPP))
        return false;
    Pokitto::Display::setInvisibleColor(0xff);
    std::uint8_t buff[12*24];

    if(popFile->read(&buff[0], 12*24) < 12*24)
        return false;
    std::size_t k=0;
    for(auto j=0;j<24;j++)
    {
        for(auto i=0;i<24;i+=2)
        {
            Pokitto::Display::drawPixel(x+i, y+j, buff[k] >> 4);
            Pokitto::Display::drawPixel(x+i+1, y+j, buff[k++] & 0xF);
        }
    }
    Pokitto::Display::setInvisibleColor(9);
    return true;
}

bool Pop::drawIcon36(const std::uint16_t x, const std::uint16_t y)
{
    if(!findTag(PopTag::TAG_IMG_36X36_4BPP))
        return false;
    Pokitto::Display::setInvisibleColor(0xff);

    std::uint8_t buff[18*36];

    if(popFile->read(&buff[0],18*36) < 18*36)
        return false;
    std::size_t k=0;
    for(auto j=0;j<36;j++)
    {
        for(auto i=0;i<36;i+=2)
        {
            Pokitto::Display::drawPixel(x+i, y+j, buff[k] >> 4);
            Pokitto::Display::drawPixel(x+i+1, y+j, buff[k++] & 0xF);
        }
    }
    Pokitto::Display::setInvisibleColor(9);
    return true;
}

bool Pop::drawScreenShoot(size_t number)
{
    if(!findTag(PopTag::TAG_IMG_220X176_565))
        return false;

    for(auto i=0;i<number;i++)
    {
        popFile->lseek(m_Tag.lenght, SEEK_CUR);
        if(!findTag(PopTag::TAG_IMG_220X176_565, false))
            return false;
    }

    std::uint16_t buff[Pokitto::Display::width];
    Pokitto::setDRAMpoint(0, 0);

    CLR_CS;
    SET_CD;
    SET_RD;
    SET_MASK_P2;

    for(auto j=0;j<Pokitto::Display::height;j++)
    {
        popFile->read(&buff[0],Pokitto::Display::width*2);

        for(auto i=0;i<Pokitto::Display::width;i++)
        {
            LPC_GPIO_PORT->MPIN[2] = buff[i]<<3;
            CLR_WR;
            SET_WR;
        }
    }
    CLR_MASK_P2;
    SET_CS;

    return true;
}
