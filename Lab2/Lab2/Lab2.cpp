#include <windows.h>
#include <stdlib.h>

#define FILE_MENU_NEW 1
#define FILE_MENU_OPEN 2
#define FILE_MENU_EXIT 3
#define CHANGE_COLOR_MENU 4
#define GENERATE_BUTTON 5
#define CHECKBOX_BASE_ID 1000
#define COMBO_BOX_ID 6



LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);

void AddMenus(HWND);
void AddControls(HWND);
void ChangeBackgroundColor(HWND, COLORREF);
void SendToDoWithEmoji(HWND);
void SimulateButtonClick(HWND hWnd);

HWND hTitle, hBody, hAddButton, hComboBox;
HMENU hMenu;
int yPos = 50;
int checkboxID = CHECKBOX_BASE_ID;
COLORREF backgroundColor = RGB(255, 255, 255);
HHOOK hHook = NULL;

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

    CreateWindowW(L"Lab1", L"Lab1", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 1400, 700, NULL, NULL, NULL, NULL);

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
            DestroyWindow(hWnd);
            break;
        case FILE_MENU_NEW:
            MessageBeep(MB_ICONINFORMATION);
            break;
        case CHANGE_COLOR_MENU:
            if (backgroundColor == RGB(255, 255, 255)) {
                backgroundColor = RGB(205, 175, 149);
            }
            else if (backgroundColor == RGB(205, 175, 149)) {
                backgroundColor = RGB(0, 0, 128); 
            }
            else {
                backgroundColor = RGB(255, 255, 255);
            }
            ChangeBackgroundColor(hWnd, backgroundColor);
            break;
        case GENERATE_BUTTON:
            SendToDoWithEmoji(hWnd);
            break;

        }
        break;
    case WM_CREATE:
        AddMenus(hWnd);
        AddControls(hWnd);
        // Install the keyboard hook
        hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, GetModuleHandle(NULL), 0);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }
    return 0;
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
            if (keyInfo->vkCode == VK_RETURN) {
                // Simulate a button click when Enter is pressed
                SimulateButtonClick(GetActiveWindow());
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}


void AddMenus(HWND hWnd) {
    hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();

    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenu(hMenu, MF_STRING, CHANGE_COLOR_MENU, L"Change Color");
    AppendMenu(hMenu, MF_STRING, NULL, L"Help");

    SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd) {
    CreateWindow(L"static", L"Enter the title of a to-do:", WS_VISIBLE | WS_CHILD, 50, 50, 170, 38, hWnd, NULL, NULL, NULL);
    hTitle = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 230, 50,250, 20, hWnd, NULL, NULL, NULL);

    CreateWindow(L"static", L"Enter the body of a to-do:", WS_VISIBLE | WS_CHILD, 50, 150, 170, 38, hWnd, NULL, NULL, NULL);
    hBody = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 230, 150, 250, 120, hWnd, NULL, NULL, NULL);

    hComboBox = CreateWindow(L"combobox", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST, 230, 100, 250, 200, hWnd, (HMENU)COMBO_BOX_ID, NULL, NULL);
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"★");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"©");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"⚠");

    CreateWindow(L"button", L"Add to-do", WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 300, 98, 38, hWnd, (HMENU)GENERATE_BUTTON, NULL, NULL);

    
}

void ChangeBackgroundColor(HWND hWnd, COLORREF color) {
    SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(color));
    RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
}

void SendToDoWithEmoji(HWND hWnd) {
    wchar_t title[31], body[250], out[300];
    int titleLength = GetWindowTextLength(hTitle);
    int bodyLength = GetWindowTextLength(hBody);

    if (titleLength == 0 || bodyLength == 0) {
        MessageBox(hWnd, L"Both title and body must be filled with non-space characters!", L"Validation Error", MB_ICONERROR);
    }
    else {
        GetWindowText(hTitle, title, 31);
        GetWindowText(hBody, body, 250);

        if (title[0] == L' ' || body[0] == L' ') {
            MessageBox(hWnd, L"Title and body cannot start with a space character!", L"Validation Error", MB_ICONERROR);
        }
        else {
            int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
            if (selectedIndex != CB_ERR) {
               
                wchar_t emoji[2] = { 0 };
                SendMessage(hComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)emoji);
                wsprintf(out, L"%s %s\r\n\r\nTitle: %s\r\n\r\nBody: %s", emoji, emoji, title, body);
            }
            else {
                wsprintf(out, L"Title: %s\r\n\r\nBody: %s", title, body);
            }

            HWND hOut = CreateWindow(L"edit", out, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY, 600, yPos, 600, 100, hWnd, NULL, NULL, NULL);

            CreateWindow(L"button", L"Mark as Done", WS_VISIBLE | WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX, 1200, yPos, 120, 20, hWnd, (HMENU)checkboxID, NULL, NULL);

            yPos += 120;
            checkboxID++;
        }
    }
}

void SimulateButtonClick(HWND hWnd) {
    SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(GENERATE_BUTTON, BN_CLICKED), (LPARAM)hAddButton);
}
