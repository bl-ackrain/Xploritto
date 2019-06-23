#pragma once
#include "globals.h"
#include "PokittoCookie.h"



class CookiesManager
{
public:
    static constexpr std::uint8_t HARDCODEDOFFSET=25;

    void init();
    bool update();
    void draw();
    void deleteCookie(const std::size_t numbre);
    bool backupCookie(const std::size_t numbre);
    bool restoreCookie(const std::string path);

private:
    void updateCookies();

    std::size_t m_selectedcookie=0;
    std::size_t m_begincookie=0;
    std::size_t m_cookiescount=0;
    std::size_t m_cookiestotalsize=0;
    bool m_redraw=true;
    char m_keystable[SBKEYSIZE*SBMAXKEYS];
    std::size_t m_sizetable[SBMAXKEYS];
    std::uint8_t m_indextable[SBMAXKEYS];
    std::uint8_t m_blockstable[SBMAXBLOCKS];
};
