#include "Soundplayer.h"
#include "Icons.h"

SoundPlayer::SoundPlayer(std::string filename)
{
    open(filename);
}

bool SoundPlayer::open(std::string filename)
{
    m_filename=filename;
    FileHandle *file=sdFs->open(filename.c_str(), O_RDONLY);
    size_t fSize=0;
    if(file)
    {
        fSize=file->flen();
        file->close();
    }
    else
        return false;

    m_duration=fSize/POK_AUD_FREQ;
    m_elapsedSec=0;

    streamcounter=0; //HWsound.h
    Pokitto::Sound::ampEnable(1);
    if(fSize > 0)
        Pokitto::Sound::playMusicStream(const_cast<char*>(filename.c_str()), 0);
    else
        return false;

    m_playing=true;
    return true;
}

bool SoundPlayer::update()
{
    if(Pokitto::Buttons::pressed(BTN_A))
    {
        if(m_playing)
        {
            Pokitto::Sound::pauseMusicStream();
            m_playing=false;
        }
        else
        {
            Pokitto::Sound::playMusicStream();
            m_playing=true;
        }
        m_redraw=true;
    }
    m_volume = Pokitto::Sound::getVolume();

    if(Pokitto::Buttons::repeat(BTN_UP,5))
    {
        if(m_volume < (7<<5))
        {
            m_volume=((m_volume>>5)+1)<<5;
            Pokitto::Sound::setVolume(m_volume);
        }

        m_redraw=true;
    }

    if(Pokitto::Buttons::repeat(BTN_DOWN,5))
    {
        if(m_volume > 0)
        {
            m_volume=((m_volume>>5)-1)<<5;
            Pokitto::Sound::setVolume(m_volume);
        }

        m_redraw=true;
    }



    if(Pokitto::Buttons::pressed(BTN_B))
    {
        Pokitto::Sound::pauseMusicStream();
        Pokitto::Sound::ampEnable(0);
        return false;
    }

    size_t newElapsedSec=Pokitto::Sound::getMusicStreamElapsedSec();

    if(newElapsedSec!=m_elapsedSec)
    {
        m_elapsedSec=newElapsedSec;
        m_redraw=true;
    }

    return true;
}

void SoundPlayer::draw()
{
    if(!m_redraw)
        return;
    m_redraw=false;
    std::string name = m_filename.substr(m_filename.find_last_of('/')+1,m_filename.size());

    Pokitto::Display::setColor(DARK_GRAY);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);

    Pokitto::Display::drawBitmap(3, STATUSBARHEIGHT+7, Icons::Files::Wav);

    Pokitto::Display::setColor(YELLOW);
    Pokitto::Display::setCursor(30, STATUSBARHEIGHT+10);
    if(m_playing)
        Pokitto::Display::print("\20 Now Playing");
    else
        Pokitto::Display::print("  Paused");

    if(name.size() < 15)
        Pokitto::Display::fontSize=2;
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(30, STATUSBARHEIGHT+20);
    Pokitto::Display::print(name.c_str());

    Pokitto::Display::setFont(font3x5);
    Pokitto::Display::fontSize=2;
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(Pokitto::Display::width-45, Pokitto::Display::height-33);
    Pokitto::Display::printf("%02u:%02u", m_duration/60, m_duration%60);


    Pokitto::Display::setCursor(5, Pokitto::Display::height-33);
    Pokitto::Display::printf("%02u:%02u", m_elapsedSec/60, m_elapsedSec%60);
    Pokitto::Display::fontSize=1;
    Pokitto::Display::setFont(font5x7);

    auto value = m_elapsedSec*100/m_duration;
    if(value > 100)
        value=100;

    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fillRoundRect(5, Pokitto::Display::height-50, 210, 10, 3);
    Pokitto::Display::setColor(GREEN);
    auto pr=(210*value)/100;

    if(pr > 3)
        Pokitto::Display::fillRoundRect(5, Pokitto::Display::height-50, pr, 10, 3);

    Pokitto::Display::setColor(YELLOW);
    Pokitto::Display::fillRect(0, Pokitto::Display::height-18, Pokitto::Display::width, 1);

    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::setCursor(Pokitto::Display::width-75, Pokitto::Display::height-12);
    Pokitto::Display::print("\23\24");

    Pokitto::Display::setCursor(5, Pokitto::Display::height-12);
    Pokitto::Display::print("B: back");

    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fillRoundRect(Pokitto::Display::width-60, Pokitto::Display::height-13, 7*8, 10, 2);
    Pokitto::Display::setColor(GREEN);
    if(m_volume > 0)
        Pokitto::Display::fillRoundRect(Pokitto::Display::width-60, Pokitto::Display::height-13, (m_volume>>5)*8, 10, 2);
}

