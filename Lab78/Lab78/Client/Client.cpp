#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

constexpr int DEFAULT_PORT = 12345;

HWND hwndOutput, hwndInput;
SOCKET clientSocket;
LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);
HHOOK hHook = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void AddMessageToOutput(const std::string& message) {
    int len = GetWindowTextLengthA(hwndOutput);
    SendMessageA(hwndOutput, EM_SETSEL, len, len);
    SendMessageA(hwndOutput, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(message.c_str()));
}

void PostMessageToOutput(const std::string& message) {
    PostMessage(hwndOutput, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(message.c_str()));
}

void SendMessageToServer() {
    char buffer[1024];
    GetWindowTextA(hwndInput, buffer, sizeof(buffer));

    if (send(clientSocket, buffer, static_cast<int>(strlen(buffer)), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send message to the server" << std::endl;
    }

    SetWindowTextA(hwndInput, "");
}

void ReceiveMessagesFromServer() {
    char buffer[1024];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "Connection closed by the server" << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0'; // Добавим нулевой символ для корректного вывода строки
        std::size_t pos = std::string(buffer).find(":");
        if (pos != std::string::npos) {
            std::string clientId = std::string(buffer).substr(0, pos);
            std::string message = "Anonymous" + clientId + ": " + std::string(buffer).substr(pos + 2) + "\n"; // +2 для пропуска ": "

            // Обновляем окно вывода сообщений
            AddMessageToOutput(message);
        }

    }
}

int main() {
    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    // Создание сокета
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create client socket" << std::endl;
        WSACleanup();
        return 1;
    }

    std::thread receiveThread(ReceiveMessagesFromServer);

    // Установка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) != 1) {
        std::cerr << "Invalid IP address" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to the server" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }


    // Создание окна
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ChatClient";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        L"ChatClient",
        L"Chat Client",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (hwnd == NULL) {
        std::cerr << "Failed to create window" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    hwndOutput = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        10, 10, 400, 200,
        hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    hwndInput = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_WANTRETURN,
        10, 220, 300, 30,
        hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    CreateWindow(
        L"BUTTON",
        L"Send",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        320, 220, 80, 30,
        hwnd,
        (HMENU)1,
        GetModuleHandle(NULL),
        NULL
    );

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Основной цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    receiveThread.join();

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CREATE:
        hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, GetModuleHandle(NULL), 0);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            OutputDebugString(L"ctuvybhunjk key pressed\n");
            SendMessageToServer();
        }
        break;
    case WM_KEYDOWN:
        OutputDebugString(L"Enter key pressed\n");
        if (wParam == VK_RETURN) {
            SendMessageToServer();
        }
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
            if (keyInfo->vkCode == VK_RETURN) {
                // Simulate a button click when Enter is pressed
                SendMessageToServer();
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}
