#pragma once
#include "globals.h"

extern uint16_t popPalette[16];

enum class PopTag {
    TAG_PADDING = 0,
    TAG_INDEX,
    TAG_OFFSETADDRESS,
    TAG_CODECHUNK,
    TAG_ENTRYPOINT,
    TAG_CRC,
    TAG_HAT,
    TAG_LONGNAME,
    TAG_AUTHOR,
    TAG_DESCRIPTION,
    TAG_IMG_36X36_4BPP,
    TAG_IMG_24X24_4BPP,
    TAG_IMG_100X24_4BPP,
    TAG_IMG_110X88_4BPP,

    TAG_IMG_36X36_565,
    TAG_IMG_24X24_565,
    TAG_IMG_100X24_565,
    TAG_IMG_110X88_565,

    TAG_IMG_220X176_565,

    TAG_CODE = 0X10008000,
};



class Pop
{
public:
    static bool open(const std::string path);
    static void close();
    static std::string getAuthor();
    static std::string getLongName();
    static std::string getDescription();
    static bool drawIcon24(const std::uint16_t x, const std::uint16_t y);
    static bool drawIcon36(const std::uint16_t x, const std::uint16_t y);
    static bool findTag(const PopTag targetTag, bool rewind=true);
    static bool drawScreenShoot(const size_t number);

    static FileHandle *popFile;

    static struct Tag{
        PopTag id;
        std::uint32_t lenght;
    } m_Tag;
};
