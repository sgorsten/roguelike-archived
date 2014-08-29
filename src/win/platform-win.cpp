#include "../platform.h"

#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class WinPlatform : public Platform
{
    HWND console;
    HANDLE buffer;
public:
    WinPlatform() : buffer()
    {
        AllocConsole();       

        SetConsoleTitleA("CONSOLE");
        Sleep(40);
        console = FindWindowA(nullptr, "CONSOLE");
        SetConsoleTitleA("");

        buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
        SetConsoleActiveScreenBuffer(buffer);

        // Set window size and console buffer size
        auto output = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFOEX info = {};
        info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        GetConsoleScreenBufferInfoEx(output, &info);
        info.dwSize = info.dwMaximumWindowSize = {WIDTH, HEIGHT};
        SetConsoleScreenBufferInfoEx(output, &info);
        SetWindowPos(console, HWND_TOP, 0, 0, 1000, 1000, SWP_NOZORDER|SWP_NOMOVE);
    }

    ~WinPlatform()
    {
        CloseHandle(buffer);
        FreeConsole();
    }

    void SetCaption(const std::string & title) override
    {
        SetConsoleTitleA(title.c_str());
    }

    void SetPalette(const Color (&colors)[NUM_COLORS]) override
    {
         // Set palette and window size
        auto output = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFOEX info = {};
        info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        GetConsoleScreenBufferInfoEx(output, &info);
        info.dwSize = info.dwMaximumWindowSize = {WIDTH, HEIGHT};
        for(int i=0; i<NUM_COLORS; ++i)
        {
            info.ColorTable[i] = RGB(colors[i].red, colors[i].green, colors[i].blue);
        }
        SetConsoleScreenBufferInfoEx(output, &info);
        SetWindowPos(console, HWND_TOP, 0, 0, 1000, 1000, SWP_NOZORDER|SWP_NOMOVE);
    }

    void SetCursor(int x, int y) override
    {
        SetConsoleCursorPosition(buffer, {x, y});
    }

    void ShowScreen(const Cell (&cells)[WIDTH*HEIGHT]) override
    {
        // Set cells
        CHAR_INFO charInfo[WIDTH*HEIGHT] = {};
        for(int i=0; i<WIDTH*HEIGHT; ++i)
        {
            charInfo[i].Char.AsciiChar = cells[i].character & 0x7f;
            charInfo[i].Attributes = cells[i].attribute;
        }
        SMALL_RECT rect = {0, 0, WIDTH, HEIGHT};
        WriteConsoleOutput(buffer, charInfo, {WIDTH, HEIGHT}, {0, 0}, &rect);
    }

    int GetChar() override
    {
        while(true)
        {
            INPUT_RECORD input;
            DWORD numRead;
            if(!ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input, 1, &numRead))
            {
                throw std::runtime_error("GetChar() failed.");
            }
            if(input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown)
            {
                return input.Event.KeyEvent.uChar.AsciiChar;
            }
        }
    }
};

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WinPlatform platform;
    return GameMain(platform);
}