#pragma once
#include <windows.h>

class Console
{
    HANDLE output;
    DWORD bytes_written = 0;

protected:
    const short screen_width = 120;
    const short screen_height = 30;
    wchar_t* screen_buffer;

public:
    Console();
    virtual ~Console();

    int getLastPressedKey();
    void clear();
    void refresh();
};
