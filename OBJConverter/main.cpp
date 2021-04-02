#include "stdafx.h"

#include "BoolkaCommon/DebugHelpers/DebugTimer.h"
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

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    if (argc != 4)
    {
        std::cerr << "Expected 3 command line argument, Got " << argc - 1 << "\n";
        return -1;
    }

    wchar_t* directory = argv[1];
    wchar_t* objFile = argv[2];
    wchar_t* outFile = argv[3];

    BOOL winSuccess = ::SetCurrentDirectoryW(directory);
    if (!winSuccess)
    {
        WinError();
        return -1;
    }

    Boolka::DebugTimer timer;
    timer.Start();
    bool res = Boolka::OBJConverter::Convert(objFile, outFile);
    float seconds = timer.Stop();
    std::cout << "Conversion took " << seconds << "s" << std::endl;

    if (!res)
    {
        return -1;
    }

    return 0;
}
