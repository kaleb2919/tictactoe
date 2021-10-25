#include "Console.h"

#include <clocale>

Console::Console()
{

    this->output = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(output, &info);
    info.bVisible = false;
    SetConsoleCursorInfo(output, &info);

    _COORD coord = {screen_width, screen_height};
    SetConsoleScreenBufferSize(output, coord);

    _SMALL_RECT rect = {0, 0, screen_width - 1, screen_height - 1};
    SetConsoleWindowInfo(output, TRUE, &rect);

    this->screen_buffer = new wchar_t[screen_width * screen_height];
    clear();
}

Console::~Console()
{
    delete [] screen_buffer;
}

int Console::getLastPressedKey()
{
    int last_key_pressed = 0x0;
    int allowed_keys[] = {VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_SPACE, VK_ESCAPE};
    for (auto key : allowed_keys)
    {
        if (GetAsyncKeyState(key))
        {
            last_key_pressed = key;
        }
    }

    return last_key_pressed;
}

void Console::clear()
{
    for (int i = 0; i < screen_width * screen_height; i++) screen_buffer[i] = L' ';
}

void Console::refresh()
{
    for (short y = 0; y < screen_height; ++y)
    {
        WriteConsoleOutputCharacterW(output, &screen_buffer[screen_width * y], screen_width, { 0, y }, &bytes_written);
    }
}
