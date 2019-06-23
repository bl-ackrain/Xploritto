#include "SDBench.h"
#include "Popup.h"

bool SDBench::init()
{
    file=sdFs->open("/Xploritto/benchmark.bin", O_RDWR | O_CREAT | O_TRUNC);
    if(!file)
    {
        Popup::Error("Error", "Open File failed");
        return false;
    }
    for (auto i = 0; i < (BUFFERSIZE-1); i++) {
		m_buffer[i] = 'A' + (i % 26);
	}
	m_buffer[BUFFERSIZE-1] = '\n';

    return true;
}

bool SDBench::update()
{
    if(m_state==State::Waiting)
    {
        if(Pokitto::Buttons::pressed(BTN_A))
            m_state=State::WriteTest;
        return true;
    }

    if(!file)
        Popup::Error("Error", "Open File failed");

    if(m_state==State::WriteTest)
    {
        m_time=Pokitto::Core::getTime();
        if(file->write(m_buffer, BUFFERSIZE) < BUFFERSIZE)
        {
            Popup::Error("Error", "File Write failed");
        }
        m_time = Pokitto::Core::getTime()-m_time;
        if (m_maxWriteLatency < m_time)
            m_maxWriteLatency = m_time;

        if (m_minWriteLatency > m_time)
            m_minWriteLatency = m_time;

        m_totalWriteLatency += m_time;
        m_progress++;
        m_writeProgress= (m_progress*BUFFERSIZE*100)/FILE_SIZE;

        if(m_progress >= FILE_SIZE/BUFFERSIZE)
        {
            m_progress=0;
            m_nTest++;
            if(m_nTest >= WRITE_COUNT)
            {
                m_state=State::ReadTest;
                file->lseek(0, SEEK_SET);
                m_nTest=0;
            }
        }
    }

    if((m_state==State::ReadTest))
    {
        m_time=Pokitto::Core::getTime();
        if(file->read(m_buffer, BUFFERSIZE) < BUFFERSIZE)
        {
            Popup::Error("Error", "File Read failed");
        }
        m_time = Pokitto::Core::getTime()-m_time;
        if (m_maxReadLatency < m_time)
            m_maxReadLatency = m_time;

        if (m_minReadLatency > m_time)
            m_minReadLatency = m_time;

        m_totalReadLatency += m_time;
        m_progress++;
        m_readProgress= (m_progress*BUFFERSIZE*100)/FILE_SIZE;

        if(m_progress >= FILE_SIZE/BUFFERSIZE)
        {
            m_progress=0;
            m_nTest++;
            if(m_nTest >= READ_COUNT)
            {
                m_state=State::End;
                file->close();
            }
        }
    }

    if(Pokitto::Buttons::pressed(BTN_B))
        return false;
    return true;
}

void SDBench::draw()
{
    Pokitto::Display::setColor(WHITE);
    Pokitto::Display::fillRect(0,STATUSBARHEIGHT,Pokitto::Display::width,Pokitto::Display::height-STATUSBARHEIGHT);
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+5);
    Pokitto::Display::setColor(BLACK);

    if(m_state==State::Waiting)
    {
        Pokitto::Display::print("Press A to start");
        return;
    }

    Pokitto::Display::print("File Size: ");
    Pokitto::Display::print(FILE_SIZE_KB);
    Pokitto::Display::print(" KB");
    Pokitto::Display::setCursor(5,STATUSBARHEIGHT+15);
    Pokitto::Display::print("Buffer Size: ");
    Pokitto::Display::print(BUFFERSIZE);
    Pokitto::Display::print(" bytes");

    Pokitto::Display::setColor(LIGHT_BLUE);
    Pokitto::Display::drawLine(5,STATUSBARHEIGHT+25,210,STATUSBARHEIGHT+25);

    Pokitto::Display::setCursor(10,STATUSBARHEIGHT+30);
    Pokitto::Display::print("Write test");
    Pokitto::Display::setCursor(10,STATUSBARHEIGHT+105);
    Pokitto::Display::print("Read test");

    Pokitto::Display::setColor(LIGHT_GRAY);
    Pokitto::Display::fillRoundRect(10,STATUSBARHEIGHT+40,200,10,2);
    Pokitto::Display::fillRoundRect(10,STATUSBARHEIGHT+115,200,10,2);
    Pokitto::Display::setColor(GREEN);
    if(m_writeProgress > 0)
        Pokitto::Display::fillRoundRect(10,STATUSBARHEIGHT+40,m_writeProgress*2,10,2);
    if(m_readProgress > 0)
        Pokitto::Display::fillRoundRect(10,STATUSBARHEIGHT+115,m_readProgress*2,10,2);

    Pokitto::Display::setColor(BLACK);

    if(m_state!=State::Waiting)
    {
        Pokitto::Display::setCursor(10,STATUSBARHEIGHT+52);
        Pokitto::Display::print("Write speed: ");
        if(m_state==State::End || m_state==State::ReadTest)
            Pokitto::Display::print(FILE_SIZE/m_totalWriteLatency);
        else
            Pokitto::Display::print(BUFFERSIZE/m_time);
        Pokitto::Display::print(" KB/s");
        Pokitto::Display::setCursor(10,STATUSBARHEIGHT+62);
        Pokitto::Display::print("Total time: ");
        Pokitto::Display::print(m_totalWriteLatency);
        Pokitto::Display::print(" ms");

        Pokitto::Display::setCursor(10,STATUSBARHEIGHT+72);
        Pokitto::Display::print("min time: ");
        Pokitto::Display::print(m_minWriteLatency);
        Pokitto::Display::print(" ms");

        Pokitto::Display::setCursor(10,STATUSBARHEIGHT+82);
        Pokitto::Display::print("max time: ");
        Pokitto::Display::print(m_maxWriteLatency);
        Pokitto::Display::print(" ms");
    }

    if(m_state==State::ReadTest || m_state==State::End)
    {
        Pokitto::Display::setCursor(10,STATUSBARHEIGHT+127);
        Pokitto::Display::print("Read speed: ");
        if(m_state==State::End)
            Pokitto::Display::print(FILE_SIZE/m_totalReadLatency);
        else
            Pokitto::Display::print(BUFFERSIZE/m_time);

        Pokitto::Display::print(" KB/s");
        Pokitto::Display::setCursor(10,STATUSBARHEIGHT+137);
        Pokitto::Display::print("Total time: ");
        Pokitto::Display::print(m_totalReadLatency);
        Pokitto::Display::print(" ms");

        Pokitto::Display::setCursor(10,STATUSBARHEIGHT+147);
        Pokitto::Display::print("min: ");
        Pokitto::Display::print(m_minReadLatency);
        Pokitto::Display::print(" ms");

        Pokitto::Display::setCursor(10,STATUSBARHEIGHT+157);
        Pokitto::Display::print("max: ");
        Pokitto::Display::print(m_maxReadLatency);
        Pokitto::Display::print(" ms");
    }
}
