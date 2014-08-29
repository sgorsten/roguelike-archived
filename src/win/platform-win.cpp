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
        
        buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
        SetConsoleActiveScreenBuffer(buffer);

        // Set window size and console buffer size
        auto output = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFOEX info = {};
        info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        GetConsoleScreenBufferInfoEx(output, &info);
        info.dwSize = info.dwMaximumWindowSize = {Screen::WIDTH, Screen::HEIGHT};
        SetConsoleScreenBufferInfoEx(output, &info);
        SetWindowPos(console, HWND_TOP, 0, 0, 1000, 1000, SWP_NOZORDER|SWP_NOMOVE);
    }

    ~WinPlatform()
    {
        CloseHandle(buffer);
        FreeConsole();
    }

    void SetPalette(const Color (&colors)[Color::NUM_COLORS]) override
    {
         // Set palette and window size
        auto output = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFOEX info = {};
        info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        GetConsoleScreenBufferInfoEx(output, &info);
        info.dwSize = info.dwMaximumWindowSize = {Screen::WIDTH, Screen::HEIGHT};
        for(int i=0; i<Color::NUM_COLORS; ++i)
        {
            info.ColorTable[i] = RGB(colors[i].red, colors[i].green, colors[i].blue);
        }
        SetConsoleScreenBufferInfoEx(output, &info);
        SetWindowPos(console, HWND_TOP, 0, 0, 1000, 1000, SWP_NOZORDER|SWP_NOMOVE);
    }

    void ShowScreen(const Screen & screen) override
    {
        // Set console title
        SetConsoleTitleA(screen.caption);

        // Set cells
        CHAR_INFO charInfo[Screen::WIDTH * Screen::HEIGHT] = {};
        for(int i=0; i<Screen::WIDTH * Screen::HEIGHT; ++i)
        {
            charInfo[i].Char.AsciiChar = screen.cells[i].character;
            charInfo[i].Attributes = screen.cells[i].attribute;
        }
        SMALL_RECT rect = {0, 0, Screen::WIDTH, Screen::HEIGHT};
        WriteConsoleOutput(buffer, charInfo, {Screen::WIDTH, Screen::HEIGHT}, {0, 0}, &rect);

        // Set cursor
        SetConsoleCursorPosition(buffer, {screen.cursorX, screen.cursorY});
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