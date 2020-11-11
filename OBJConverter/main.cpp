#include "stdafx.h"

#include "OBJConverter.h"

void WinError()
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    DWORD formatResult = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf,
        0, NULL);

    if (formatResult == 0)
    {
        std::cout << "Error generating error string\n";
        return;
    }

    std::cout << "Error: " << (char*)lpMsgBuf;

    LocalFree(lpMsgBuf);
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Expected 3 command line argument, Got " << argc - 1 << "\n";
        return -1;
    }

    char* directory = argv[1];
    char* objFile = argv[2];
    char* outFile = argv[3];

    BOOL winSuccess = ::SetCurrentDirectoryA(directory);
    if (!winSuccess)
    {
        WinError();
        return -1;
    }

    Boolka::OBJConverter converter;
    bool success = converter.Load(objFile);

    if (success)
    {
        std::cout << "Loaded " << objFile << " successfully\n";
    } 
    else
    {
        std::cout << "Failed to open " << objFile << "\n";
        return -1;
    }

    success = converter.Save(outFile);

    if (success)
    {
        std::cout << "Saved " << outFile << " successfully\n";
    }
    else
    {
        std::cout << "Failed to save to " << outFile << "\n";
        return -1;
    }

    return 0;
}
