#include "aleStringUtils.h"


std::string ConvertToUtf8(const std::wstring& wide) {
    std::string utf8;
    for (wchar_t wc : wide) {
        if (wc < 0x80) {
            utf8.push_back(static_cast<char>(wc));
        }
        else if (wc < 0x800) {
            utf8.push_back(0xC0 | (wc >> 6));
            utf8.push_back(0x80 | (wc & 0x3F));
        }
        else {
            utf8.push_back(0xE0 | (wc >> 12));
            utf8.push_back(0x80 | ((wc >> 6) & 0x3F));
            utf8.push_back(0x80 | (wc & 0x3F));
        }
    }
    return utf8;
}
