#include <windows.h>
#include <stdlib.h>
#include <string>
using namespace std;

#define WRITE_DATA 1
#define READ_DATA 2


LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddControls(HWND);
void WriteData(HWND);
void ReadData(HWND);

HWND hDataToWrite, hReadData, hCharCount;
wchar_t textToWrite[200];

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    WNDCLASS wc = { 0 };

    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"Lab3";
    wc.lpfnWndProc = WindowProcedure;
        
    if (!RegisterClassW(&wc)) {
        return -1;
    }

    CreateWindowW(L"Lab3", L"Lab3", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 1400, 700, NULL, NULL, NULL, NULL);

    MSG msg = { 0 };

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        switch (wp) {
        case WRITE_DATA:
            WriteData(hWnd);
            break;
        case READ_DATA:
            ReadData(hWnd);
            break;
        }
        break;
    case WM_CREATE:
        AddControls(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }
}


void AddControls(HWND hWnd) {
    CreateWindow(L"static", L"Enter you data", WS_VISIBLE | WS_CHILD, 50, 50, 210, 38, hWnd, NULL, NULL, NULL);
    hDataToWrite = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 230, 50, 250, 100, hWnd, NULL, NULL, NULL);
    CreateWindow(L"button", L"Write data", WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 90, 98, 38, hWnd, (HMENU)WRITE_DATA, NULL, NULL);

    CreateWindow(L"static", L"Read data", WS_VISIBLE | WS_CHILD, 600, 50, 210, 38, hWnd, NULL, NULL, NULL);
    hReadData = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 750, 50, 250, 100, hWnd, NULL, NULL, NULL);
    CreateWindow(L"button", L"Read data", WS_VISIBLE | WS_CHILD | WS_BORDER, 600, 90, 98, 38, hWnd, (HMENU)READ_DATA, NULL, NULL);

    hCharCount = CreateWindow(L"static", L"0", WS_VISIBLE | WS_CHILD, 460, 160, 40, 20, hWnd, NULL, NULL, NULL);
}

void WriteData(HWND hWnd) {

    HANDLE hFile = CreateFileW(
        L"C:\\Users\\sea25\\Downloads\\ABOBS.txt",
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL
    );


    GetWindowTextW(hDataToWrite, textToWrite, 200);

    int numChars = wcslen(textToWrite) + 1;

    SetWindowTextW(hCharCount, std::to_wstring(numChars-1).c_str());


    int numBytes = numChars * sizeof(wchar_t);
    DWORD fileSize = numBytes;

    SetFilePointer(hFile, fileSize, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);


    HANDLE hMapFile = CreateFileMapping(
        hFile,
        NULL,
        PAGE_READWRITE,
        0,
        fileSize,
        NULL
    );

    LPVOID pData = MapViewOfFile(
        hMapFile,
        FILE_MAP_WRITE,
        0,
        0,
        0
    );
    memcpy(pData, textToWrite, numBytes);


    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);
    CloseHandle(hFile);
}

void ReadData(HWND hWnd) {
    HANDLE hFile = CreateFile(
        L"C:\\Users\\sea25\\Downloads\\ABOBS.txt",
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

  
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        CloseHandle(hFile);
        return;
    }

    HANDLE hMapFile = CreateFileMapping(
        hFile,
        NULL,
        PAGE_READONLY,
        0,
        fileSize,
        NULL
    );


    LPVOID pData = MapViewOfFile(
        hMapFile,
        FILE_MAP_READ,
        0,
        0,
        0
    );



    wchar_t textToRead[200];
    memcpy(textToRead, pData, fileSize);

    SetWindowText(hReadData, textToRead);


    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);
    CloseHandle(hFile);
}