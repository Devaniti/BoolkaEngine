#include "stdafx.h"

#include "OBJConverter.h"

void WinError()
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    DWORD formatResult = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsgBuf, 0, NULL);

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

    bool res = Boolka::OBJConverter::Convert(objFile, outFile);

    if (!res)
    {
        return -1;
    }

    return 0;
}
