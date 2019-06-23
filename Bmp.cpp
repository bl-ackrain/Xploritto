#include "Bmp.h"
#include "Popup.h"

#include "HWLCD.h"

extern void write_command_16(uint16_t data);

bool Bmp::draw(std::string path)
{
    FileHandle *bmpFile=sdFs->open(path.c_str(), O_RDONLY);
    if(!bmpFile)
    {
        Popup::Error("ERROR","Open file failed");
        bmpFile->close();
        return false;
    }

    uint16_t magic, planes, bpp;
    uint32_t width, offset, compression;
    int32_t  height;
    bool flipped = true;

    bmpFile->read(&magic, 2);
    if(magic!=0x4D42)
    {
        Popup::Error("ERROR","Not a valid bmp file");
        bmpFile->close();
        return false;
    }
    bmpFile->lseek(8, SEEK_CUR);
    bmpFile->read(&offset, 4);
    bmpFile->lseek(4, SEEK_CUR);
    bmpFile->read(&width, 4);
    bmpFile->read(&height, 4);
    bmpFile->read(&planes, 2);
    if(planes!=1)
    {
        Popup::Error("ERROR","Planes must be 1");
        bmpFile->close();
        return false;
    }
    bmpFile->read(&bpp, 2);

    if(bpp!=24)
    {
        Popup::Error("ERROR","Only 24 bits Bmp are supported");
        bmpFile->close();
        return false;
    }
    bmpFile->read(&compression, 4);
    if(compression!=0)
    {
        Popup::Error("ERROR","Only uncompressed Bmp are supported");
        bmpFile->close();
        return false;
    }
    Pokitto::Display::fillLCD(0);
    if(height < 0)
    {
        height=-height;
        flipped=false;
    }

    uint32_t rowSize;
    uint8_t  sdbufferLen = BUFFPIXEL * 3;
    uint8_t  sdbuffer[sdbufferLen];
    uint8_t  buffidx = sdbufferLen;
    uint16_t w, h, row, col;
    uint8_t  r, g, b;
    uint32_t pos = 0;

    w = width;
    h = height;
    if((w-1) >= Pokitto::Display::width)
        w = Pokitto::Display::width;
    if((h-1) >= Pokitto::Display::height)
        h = Pokitto::Display::height;
    rowSize = (width * 3 + 3) & ~3;

    Pokitto::setWindow((Pokitto::Display::height-h)/2,(Pokitto::Display::width-w)/2,(Pokitto::Display::height-h)/2+h-1,(Pokitto::Display::width-w)/2+w-1);
    write_command_16(0x22);

    CLR_CS;
    SET_CD;
    SET_RD;
    SET_MASK_P2;

    for (row=0; row<h; row++)
    {
        if (flipped){
          pos = offset + (height - 1 - row) * rowSize;
        }
        else {
          pos = offset + row * rowSize;
        }
          bmpFile->lseek(pos, SEEK_SET);
          buffidx = sdbufferLen;

        for (col=0; col<w; col++)
        {
          if (buffidx >= sdbufferLen)
          {
              bmpFile->read(sdbuffer, sdbufferLen);
              buffidx = 0;
          }

          b = sdbuffer[buffidx++];
          g = sdbuffer[buffidx++];
          r = sdbuffer[buffidx++];

          uint16_t color = b>>3;
          color |= ((g >> 2) << 5);
          color |= ((r >> 3) << 11);
          LPC_GPIO_PORT->MPIN[2] = color<<3;

          //CLR_WR;
          LPC_GPIO_PORT->CLR[LCD_WR_PORT] = 1 << LCD_WR_PIN;
          SET_WR;

        }
    }

    CLR_MASK_P2;
    SET_CS;
    bmpFile->close();
    Pokitto::setWindow(0, 0, 175, 219);
    return true;
}
