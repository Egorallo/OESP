#include <windows.h>
#include <stdlib.h>

#define FILE_MENU_NEW 1
#define FILE_MENU_OPEN 2
#define FILE_MENU_EXIT 3
#define GENERATE_BUTTON 4
#define CROSS_OUT_RADIO 5

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND);
void AddControls(HWND);

HWND hTitle, hBody, hOut;
HMENU hMenu;

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
        case GENERATE_BUTTON:
            wchar_t title[31], body[200], out[250];
            int titleLength = GetWindowTextLength(hTitle);
            int bodyLength = GetWindowTextLength(hBody);

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
                }
            }
            break;
        }
        break;
    case WM_CREATE:
        AddMenus(hWnd);
        AddControls(hWnd);
        break;
    case WM_DESTROY:
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

    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_NEW, L"New");
    AppendMenu(hFileMenu, MF_POPUP, (UINT)hSubMenu, L"Open submenu");
    AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenu(hMenu, MF_STRING, NULL, L"Help");

    SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd) {
    CreateWindow(L"static", L"Enter the title of a to-do:", WS_VISIBLE | WS_CHILD, 50, 50, 210, 38, hWnd, NULL, NULL, NULL);
    hTitle = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 230, 50,250, 20, hWnd, NULL, NULL, NULL);

    CreateWindow(L"static", L"Enter the body of a to-do:", WS_VISIBLE | WS_CHILD, 50, 150, 210, 38, hWnd, NULL, NULL, NULL);
    hBody = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 230, 150, 250, 120, hWnd, NULL, NULL, NULL);

    CreateWindow(L"button", L"Add to-do", WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 300, 98, 38, hWnd, (HMENU)GENERATE_BUTTON, NULL, NULL);

    hOut = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY, 600, 50, 700, 400, hWnd, NULL, NULL, NULL);
}
