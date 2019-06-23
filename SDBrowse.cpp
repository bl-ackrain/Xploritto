#pragma once
#include "SDBrowse.h"
#include "Icons.h"
#include "Pop.h"
#include "Popup.h"
#include "Bmp.h"
#include "Settings.h"


SDBrowse::SDBrowse(): m_curDir("")
{

}

void SDBrowse::init()
{
    Popup::Loading("Loading SD Card..");
    m_itemcount=fileCount(m_curDir);
}

size_t SDBrowse::fileCount(std::string path)
{
   m_redraw=true;
   auto c=0;
   DirHandle *dir = sdFs->opendir(path.c_str());
   while(dir->readdir())
       c++;
    dir->closedir();
   return c;
}

bool SDBrowse::update()
{
    if(menustate==MenuState::SoundPlayer)
    {
        if(!soundplayer->update())
        {
            delete soundplayer;
            menustate=MenuState::MainMenu;
        }
        return true;
    }

    if(Pokitto::Buttons::repeat(BTN_RIGHT,5) && m_itemcount > 5)
    {
        if(m_beginitem < m_itemcount-5 )
        {
            m_beginitem+=5;
            m_selecteditem=0;
            m_redraw=true;
        }
        return true;
    }

    if(Pokitto::Buttons::repeat(BTN_LEFT,5) && m_itemcount > 5)
    {
        if(m_beginitem > 0)
        {
            m_beginitem -= 5;
            m_selecteditem=0;
            m_redraw=true;
        }
        return true;
    }

    if(Pokitto::Buttons::repeat(BTN_UP,1))
    {
        if(m_selecteditem==0)
        {
            if(m_beginitem >= 5)
            {
                m_selecteditem=4;
                m_beginitem-=5;
            }
            else
                return true;
        }
        else
            m_selecteditem--;
       m_redraw=true;
       return true;
    }

    if(Pokitto::Buttons::repeat(BTN_DOWN,1))
    {
        if(m_selecteditem+m_beginitem >= m_itemcount-1)
            return true;

        m_selecteditem++;

        if(m_selecteditem == 5)
        {
            m_selecteditem=0;
            m_beginitem+=5;
        }

        m_redraw=true;
        return true;
    }

    if(Pokitto::Buttons::pressed(BTN_A))
    {
        DirHandle *dir = sdFs->opendir(m_curDir.c_str());
        for(auto j=0;j<m_beginitem+m_selecteditem;j++)
            dir->readdir();

        std::string fileName = m_curDir + "/";
        fileName += dir->readdir()->d_name;
        DirHandle *isdir=sdFs->opendir(fileName.c_str());
        dir->closedir();
        if(isdir)
        {
            m_curDir=fileName;
            isdir->closedir();

            m_oldbeginitem=m_beginitem;
            m_oldselecteditem=m_selecteditem;
            m_beginitem=0;
            m_selecteditem=0;

            m_itemcount=fileCount(m_curDir);
            m_redraw=true;
        }else
        {
            if(fileName.find(".pop") != std::string::npos || fileName.find(".POP") != std::string::npos)
            {

                if(Popup::PopInfo(fileName))
                {

                    size_t num=0;
                    if(Pop::drawScreenShoot(num++))
                    {
                        while(true)
                        {
                            Pokitto::Buttons::update();

                            if(Pokitto::Buttons::pressed(BTN_A))
                                if(!Pop::drawScreenShoot(num++))
                                    break;
                            if(Pokitto::Buttons::pressed(BTN_B))
                                break;

                        }
                    }


                    //Settings::jumpToLoader(std::string("Load:"+fileName).c_str());
                }
            }
            //else if(fileName.find(".bin") != std::string::npos || fileName.find(".BIN") != std::string::npos)
            //{
                 //Settings::jumpToLoader(std::string("Load:"+fileName).c_str());
            //}
            else if(fileName.find(".der") != std::string::npos || fileName.find(".DER") != std::string::npos)
            {
                if(Popup::LoaderInfo(fileName))
                {

                    FileHandle *loaderfile=sdFs->open(fileName.c_str(),O_RDONLY);
                    if(!loaderfile)
                        return false;

                    size_t fsize=loaderfile->flen();
                    loaderfile->lseek(fsize-4,SEEK_SET);
                    //size_t version= 0;
                    //loaderfile->read(&version,4);
                    size_t jumpadd=0;
                    loaderfile->read(&jumpadd,4);
                    loaderfile->lseek(0,SEEK_SET);
                    if (fsize>0x40000-jumpadd)
                        fsize = 0x40000-jumpadd;

                    uint32_t counter=0;
                    uint8_t data[0x100];


                    while (1) {
                            if(counter==0)
                                Popup::Loading("Erasing old loader...");
                            else
                                Popup::progressBar("Flashing Loader",(counter*100)/fsize);
                        if (counter >= fsize) {
                            break;
                        }
                        if (loaderfile->read(&data[0],0x100)<0x100) {
                            if (fsize-counter>0x100) {
                                    Popup::Error("Error", "Reading Loader file");
                                    return true;
                            }
                        }

                        if (CopyPageToFlash(jumpadd+counter, reinterpret_cast<std::uint8_t*>(data))){
                            Popup::Error("Error","Flash write error");
                            return true;
                        } else {
                            counter += 0x100;
                        }
                    }

                    Popup::InfoBox("Loader","Loader installed successfully");
                }
            }else if(fileName.find(".raw") != std::string::npos || fileName.find(".RAW") != std::string::npos || fileName.find(".snd") != std::string::npos || fileName.find(".SND") != std::string::npos || fileName.find(".wav") != std::string::npos || fileName.find(".WAV") != std::string::npos)
            {
                soundplayer=new SoundPlayer(fileName);
                menustate=MenuState::SoundPlayer;
            }
            else if(fileName.find(".bmp") != std::string::npos || fileName.find(".BMP") != std::string::npos )
            {
                if(Bmp::draw(fileName))
                {
                    while(true)
                    {
                        Pokitto::Buttons::update();
                        if(Pokitto::Buttons::pressed(BTN_A) || Pokitto::Buttons::pressed(BTN_B))
                            break;
                    }
                }

            }
            else
                Popup::fileInfo(fileName);

            m_redraw=true;
        }

    }else if(Pokitto::Buttons::pressed(BTN_B))
    {
        if(m_curDir=="")
            return false;

        auto pos = m_curDir.find_last_of('/');
        m_curDir = m_curDir.substr(0,pos);
        //m_oldbeginitem=m_beginitem;
        //m_oldselecteditem=m_selecteditem;
        m_beginitem=m_oldbeginitem;
        m_selecteditem=m_oldselecteditem;
        m_itemcount=fileCount(m_curDir);
        m_redraw=true;

    }else if(Pokitto::Buttons::pressed(BTN_C))
    {
        if(m_movemode)
        {
            if(Popup::YesNo("Are you sure?","Do you want to move file here:\n"+m_movepath))
            {
                std::string moveto=m_curDir + "/" + m_movepath.substr(m_movepath.find_last_of('/')+1,m_movepath.size());
                FRESULT res = f_rename(m_movepath.c_str(), moveto.c_str());
                if( res != FR_OK)
                    Popup::Error("Error ","moving file\n"+moveto, res);
                else
                {
                    m_itemcount=fileCount(m_curDir);
                    m_movemode=false;
                }

            }
            m_redraw=true;
            return true;
        }

        DirHandle *dir = sdFs->opendir(m_curDir.c_str());
        for(auto j=0;j<m_beginitem+m_selecteditem;j++)
            dir->readdir();

        std::string fileName = m_curDir + "/";
        fileName += dir->readdir()->d_name;
        dir->closedir();

        std::string filemenu[]={"Info", "Open as","Copy", "Move", "Delete", "Rename", "New file", "New folder"};
        const unsigned char * menuicons[]={Icons::Actions::Info, Icons::Actions::Open, Icons::Actions::Copy, Icons::Actions::Move, Icons::Actions::DeleteFile, Icons::Actions::Rename, Icons::Actions::NewFile, Icons::Actions::NewFolder};
        size_t ret=Popup::Menu("File menu",filemenu, menuicons, 8);

        switch(ret)
        {
        case 0:
            Popup::fileInfo(fileName);
            break;
        case 1:
            Popup::Error("Error","Not implemented");
            break;
        case 2:
            Popup::Error("Error","Not implemented");
            break;
        case 3:
            m_movepath=fileName;
            m_movemode=true;

            break;
        case 4:

            if(Popup::YesNo("Are you sure?","Do you want to delete:\n"+fileName))
            {
                FRESULT res = f_unlink(fileName.c_str());
                if( res != FR_OK)
                    Popup::Error("Error ","deleting file\n"+fileName, res);
            }

            break;
        case 5:
            {
                std::string strname = fileName.substr(fileName.find_last_of('/')+1,fileName.size());
                char name[30];
                strcpy(name,strname.c_str());
                Popup::keyboard("enter file name", name, 30);
                if(strlen(name)==0)
                    break;
                FRESULT res = f_rename(fileName.c_str(), std::string(m_curDir+"/"+name).c_str());
                if( res != FR_OK)
                    Popup::Error("Error ","renaming file\n"+fileName, res);
            }

            break;
        case 6:
            {

                char name[30];
                name[0]=0;
                Popup::keyboard("enter file name", name, 30);
                if(strlen(name)==0)
                    break;
                std::string file = m_curDir + "/" + name;
                FileHandle *f=sdFs->open(file.c_str(), O_CREAT | O_WRONLY);
                if( !f)
                    Popup::Error("Error ","file create error\n"+file);
                f->close();
                m_itemcount=fileCount(m_curDir);
            }

            break;

        case 7:
            {
                char name[30];
                sprintf(name,"New Folder");
                Popup::keyboard("enter folder name", name, 30);
                if(strlen(name)==0)
                    break;
                std::string folder = m_curDir + "/" + name;
                FRESULT res=f_mkdir(folder.c_str());
                if( res != FR_OK)
                    Popup::Error("Error ","mkdir error\n"+folder, res);
                else
                    m_itemcount=fileCount(m_curDir);
            }

            break;

        case -1:
            break;
        };

        m_redraw=true;
    }
    return true;
}

void SDBrowse::draw()
{
    if(menustate==MenuState::SoundPlayer)
    {
        soundplayer->draw();
        return;
    }

    if(!m_redraw)
        return;

    Pokitto::Display::setColor(CAYAN);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);

    Pokitto::Display::setColor(DARK_GRAY);
    Pokitto::Display::fillRect(Pokitto::Display::width-3,STATUSBARHEIGHT+16,2,145);

    if(m_itemcount > 5)
    {
        Pokitto::Display::setColor(WHITE);
        Pokitto::Display::fillRect(Pokitto::Display::width-3,STATUSBARHEIGHT+16+(145/((m_itemcount+4)/5))*m_beginitem/5,2,145/((m_itemcount+4)/5));
    }

    Pokitto::Display::setColor(DARK_GRAY);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,12);

    //DWORD fre_clust;
    //FATFS *fs=&sdFs->_fs;
    //f_getfree(0, &fre_clust, &fs);

    //Pokitto::Display::setColor(GREEN);
    //Pokitto::Display::fillRect(0,STATUSBARHEIGHT+12,Pokitto::Display::width*(sdFs->_fs.n_fatent-fre_clust)/sdFs->_fs.n_fatent,1);

    Pokitto::Display::setFont(font5x7);
    Pokitto::Display::setCursor(15,STATUSBARHEIGHT+2);
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::print(m_curDir.c_str());
    Pokitto::Display::print("/");

    if(m_movemode)
    {
        Pokitto::Display::setColor(WHITE);
        Pokitto::Display::fillRect(Pokitto::Display::width-15,STATUSBARHEIGHT,12,12);
        Pokitto::Display::drawBitmap(Pokitto::Display::width-13,STATUSBARHEIGHT+2,Icons::Actions::Move);
    }


    DirHandle *dir = sdFs->opendir(m_curDir.c_str());
    for(size_t j=0;j<m_beginitem;j++)
        dir->readdir();

    char *fileName;
    for(size_t i=0;i<5;i++)
    {
        fileName=0;
        fileName=dir->readdir()->d_name;
        if(!fileName)
            break;
        Pokitto::Display::setColor(WHITE);
        if(m_selecteditem==i)
            Pokitto::Display::setColor(LIGHT_BLUE);

        Pokitto::Display::fillRect(5,STATUSBARHEIGHT+15+i*30,Pokitto::Display::width-10,30-2);
        std::string f=m_curDir+"/"+fileName;
        static FILINFO info;
        f_stat(f.c_str(), &info);
        if(info.fattrib & AM_DIR)
        {
            Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30,Icons::Files::Folder);
        }else
        {
            if(f.find(".bin") != std::string::npos || f.find(".BIN") != std::string::npos)
            {
                FileHandle *fbin=sdFs->open(f.c_str(), O_RDONLY);
                if(fbin)
                {
                    uint32_t magic=0;
                    fbin->read(&magic,4);
                    if(magic==0X10008000)
                      Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::PokiBin);
                    else
                      Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Bin);
                    fbin->close();
                }
                else
                    Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Bin);
            }
            else if(f.find(".snd") != std::string::npos || f.find(".SND") != std::string::npos || f.find(".raw") != std::string::npos || f.find(".RAW") != std::string::npos)
                Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Snd);
            else if(f.find(".wav") != std::string::npos || f.find(".WAV") != std::string::npos)
                Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Wav);
            else if(f.find(".txt") != std::string::npos || f.find(".TXT") != std::string::npos)
                Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Txt);
            else if(f.find(".rom") != std::string::npos || f.find(".ROM") != std::string::npos)
                Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Rom);
            else if(f.find(".der") != std::string::npos || f.find(".DER") != std::string::npos)
                Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Der);
            else if(f.find(".bmp") != std::string::npos || f.find(".BMP") != std::string::npos)
                Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Bmp);
            else
                Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::File);


            Pokitto::Display::setFont(font3x5);
            Pokitto::Display::setColor(GREEN);
            char s[10];
            sprintf(s,"%d.%01d KB",info.fsize/1024,(info.fsize%1024)/102);
            Pokitto::Display::setCursor(Pokitto::Display::width-10-strlen(s)*4,STATUSBARHEIGHT+15+21+i*30);
            Pokitto::Display::print(s);

            Pokitto::Display::setColor(DARK_GRAY);

            if(f.find(".pop") != std::string::npos || f.find(".POP") != std::string::npos)
            {
                Pop::open(f);
                if(!Pop::drawIcon24(7,STATUSBARHEIGHT+15+2+i*30))
                    Pokitto::Display::drawBitmap(7,STATUSBARHEIGHT+15+2+i*30, Icons::Files::Pop);
                Pokitto::Display::setColor(PURPLE);
                Pokitto::Display::setCursor(35,STATUSBARHEIGHT+15+21+i*30);
                Pokitto::Display::print("by ");
                Pokitto::Display::print(Pop::getAuthor().c_str());
                Pop::close();

            }

            Pokitto::Display::setFont(font5x7);
        }
        if(m_selecteditem==i)
            Pokitto::Display::setColor(WHITE);
        else
            Pokitto::Display::setColor(BLACK);

        Pokitto::Display::setCursor(35,STATUSBARHEIGHT+15+2+i*30);

        Pokitto::Display::print(fileName);
    }

    dir->closedir();
    m_redraw=false;
}
