#pragma once
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma unmanaged
#include <strsafe.h>
#pragma managed

// Link lib
#pragma comment(lib, "strsafe.lib")

#define UNREF(x) x;

typedef unsigned int uint;
typedef unsigned char uchar;

namespace OpenGLCpp
{
    class Helper
    {
    public:
        //
        // Taken from MSDN
        //
        static void ErrorExit(LPCTSTR lpszFunction)
        {
            // Retrieve the system error message for the last-error code
            LPVOID lpMsgBuf;
            LPVOID lpDisplayBuf;
            DWORD dw = GetLastError();

            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf,
                0, NULL);

            // Display the error message and exit the process

            lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

            StringCchPrintf((LPTSTR)lpDisplayBuf,
                LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                TEXT("%s failed with error %d: %s"),
                lpszFunction, dw, lpMsgBuf);
            ::MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

            LocalFree(lpMsgBuf);
            LocalFree(lpDisplayBuf);
            ExitProcess(dw);
        }
    };
}