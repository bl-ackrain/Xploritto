#pragma once

#include "globals.h"

class SDBench
{
    enum class State
    {
        Waiting=0,
        WriteTest,
        ReadTest,
        End
    };

    constexpr static size_t BUFFERSIZE = 512;
    constexpr static size_t FILE_SIZE_KB = 128;
    constexpr static uint8_t WRITE_COUNT = 1;
    constexpr static uint8_t READ_COUNT = 1;
    constexpr static size_t FILE_SIZE = 1024*FILE_SIZE_KB;
public:
    bool init();
    bool update();
    void draw();
private:
    State m_state=State::Waiting;
    FileHandle *file;
    uint8_t  m_buffer[BUFFER_SIZE];
	uint32_t m_time = 0;
	uint32_t m_maxWriteLatency = 0;
	uint32_t m_minWriteLatency = 0xFFFFFFFF;
	uint32_t m_totalWriteLatency = 0;

    uint32_t m_maxReadLatency = 0;
	uint32_t m_minReadLatency = 0xFFFFFFFF;
	uint32_t m_totalReadLatency = 0;

	uint8_t m_writeProgress = 0;
	uint8_t m_readProgress = 0;

	uint8_t m_nTest = 0;
	uint32_t m_progress = 0;
};
