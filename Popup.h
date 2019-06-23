#pragma once
#include <string>

namespace Popup
{
    bool YesNo(const std::string title, const std::string text);
    bool Error(const std::string title, const std::string text, const size_t code=0);
    void InfoBox(const std::string title, const std::string text);
    void Loading(const std::string text);
    bool PopInfo(const std::string path);
    bool LoaderInfo(const std::string path);
    void fileInfo(const std::string path);
    size_t Menu(const std::string title, std::string entries[], const std::uint8_t *icons [], size_t NumOfEntries);
    void keyboard(const std::string title, char *text, const size_t length);
    void progressBar(const std::string title, const size_t value);
    size_t timePicker(const std::string title, bool format24, size_t value);
    size_t datePicker(const std::string title, const std::uint8_t dateFormat, const size_t value);
    unsigned char volumeSlider(const std::string title, const std::uint8_t value, const std::uint8_t maxValue);
    void drawPopup(const std::string title, const std::string text, const std::uint8_t color);
    std::string fmt(const size_t margin, const size_t width, const std::string text);
};
