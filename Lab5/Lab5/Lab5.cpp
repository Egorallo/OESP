#include <windows.h>
#include <stdlib.h>
#include <string>

#define FILE_MENU_EXIT 3
#define GENERATE_BUTTON 4
#define CLEAR_BUTTON 5


LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND);
void AddControls(HWND);
void SaveDataToRegistry(const TCHAR* keyName, const TCHAR* valueName, const TCHAR* data);
bool LoadDataFromRegistry(const TCHAR* keyName, const TCHAR* valueName, TCHAR* buffer, DWORD bufferSize);
void WriteToEventLog(const std::wstring& message);

HWND hTitle, hBody, hOut;
HMENU hMenu;
wchar_t title[31], body[200], out[250];
int titleLength = 0;
int bodyLength = 0;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    WNDCLASS wc = { 0 };

    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"Lab1";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClassW(&wc)) {
        return -1;
    }

    CreateWindowW(L"Lab1", L"Lab1", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 900, 450, NULL, NULL, NULL, NULL);

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
        case FILE_MENU_EXIT:
            MessageBeep(MB_ICONINFORMATION);
            GetWindowText(hTitle, title, 31);
            GetWindowText(hBody, body, 200);
            GetWindowText(hOut, out, 250);
            SaveDataToRegistry(L"Software\\AmazinNoteEditor22", L"NoteTitle", title);
            SaveDataToRegistry(L"Software\\AmazinNoteEditor22", L"NoteContent", body);
            SaveDataToRegistry(L"Software\\AmazinNoteEditor22", L"NoteOut", out);

            DestroyWindow(hWnd);
            break;
        case GENERATE_BUTTON:
            
            titleLength = GetWindowTextLength(hTitle);
            bodyLength = GetWindowTextLength(hBody);

            if (titleLength == 0 || bodyLength == 0) {
                MessageBox(hWnd, L"Both title and body must be filled with non-space characters!", L"Validation Error", MB_ICONERROR);
            }
            else {
                GetWindowText(hTitle, title, 31);
                GetWindowText(hBody, body, 200);

                if (title[0] == L' ' || body[0] == L' ') {
                    MessageBox(hWnd, L"Title and body cannot start with a space character!", L"Validation Error", MB_ICONERROR);
                }
                else {
                    wsprintf(out, L"Title: %s\r\n\r\nBody: %s", title, body);

                    SetWindowText(hOut, out);

                    WriteToEventLog(L"New note was added");
                    
                }
            }
            break;
        case CLEAR_BUTTON:
            SetWindowText(hOut, L"");
            break;
        }
        
        break;
    case WM_CREATE:

        AddMenus(hWnd);
        AddControls(hWnd);
        TCHAR titleBuffer[31];
        if (LoadDataFromRegistry(L"Software\\AmazinNoteEditor22", L"NoteTitle", titleBuffer, sizeof(titleBuffer))) {
            SetWindowText(hTitle, titleBuffer);
        }
        TCHAR bodyBuffer[200];
        if (LoadDataFromRegistry(L"Software\\AmazinNoteEditor22", L"NoteContent", bodyBuffer, sizeof(bodyBuffer))) {
            SetWindowText(hBody, bodyBuffer);
        }
        TCHAR outBuffer[250];
        if (LoadDataFromRegistry(L"Software\\AmazinNoteEditor22", L"NoteOut", outBuffer, sizeof(outBuffer))) {
            SetWindowText(hOut, outBuffer);
        }
        break;
    case WM_DESTROY:
        MessageBeep(MB_ICONINFORMATION);
        GetWindowText(hTitle, title, 31);
        GetWindowText(hBody, body, 200);
        GetWindowText(hOut, out, 250);
        SaveDataToRegistry(L"Software\\AmazinNoteEditor22", L"NoteTitle", title);
        SaveDataToRegistry(L"Software\\AmazinNoteEditor22", L"NoteContent", body);
        SaveDataToRegistry(L"Software\\AmazinNoteEditor22", L"NoteOut", out);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }
}

void AddMenus(HWND hWnd) {
    hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hSubMenu = CreateMenu();

    AppendMenu(hSubMenu, MF_STRING, NULL, L"Submenu item");
    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");


    SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd) {
    CreateWindow(L"static", L"Enter the title of a to-do:", WS_VISIBLE | WS_CHILD, 50, 50, 210, 38, hWnd, NULL, NULL, NULL);
    hTitle = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 230, 50, 250, 20, hWnd, NULL, NULL, NULL);

    CreateWindow(L"static", L"Enter the body of a to-do:", WS_VISIBLE | WS_CHILD, 50, 150, 210, 38, hWnd, NULL, NULL, NULL);
    hBody = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 230, 150, 250, 58, hWnd, NULL, NULL, NULL);

    CreateWindow(L"button", L"Add to-do", WS_VISIBLE | WS_CHILD | WS_BORDER, 230, 220, 98, 38, hWnd, (HMENU)GENERATE_BUTTON, NULL, NULL);
    CreateWindow(L"button", L"Clear", WS_VISIBLE | WS_CHILD | WS_BORDER, 489, 312, 98, 38, hWnd, (HMENU)CLEAR_BUTTON, NULL, NULL);

    hOut = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY, 600, 50, 240,300 , hWnd, NULL, NULL, NULL);
}

void SaveDataToRegistry(const TCHAR* keyName, const TCHAR* valueName, const TCHAR* data) {
    HKEY hKey;
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, keyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (result == ERROR_SUCCESS) {
        RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE*)data, (wcslen(data) + 1) * sizeof(TCHAR));
        RegCloseKey(hKey);
    }
}

bool LoadDataFromRegistry(const TCHAR* keyName, const TCHAR* valueName, TCHAR* buffer, DWORD bufferSize) {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, keyName, 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS) {
        DWORD dataSize = bufferSize;
        result = RegQueryValueEx(hKey, valueName, NULL, NULL, (BYTE*)buffer, &dataSize);
        RegCloseKey(hKey);

        if (result == ERROR_SUCCESS) {
            return true;
        }
    }

    return false;
}

void WriteToEventLog(const std::wstring& message) {
    HANDLE hEventLog = RegisterEventSource(NULL, L"AmazinEditor222");

    if (hEventLog) {
        LPCWSTR messageStrings[1];
        messageStrings[0] = message.c_str();

        ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, messageStrings, NULL);

        DeregisterEventSource(hEventLog);
    }
}