#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <vector>
#include <thread>
#include <iostream>
#include <string>

using namespace std;

unsigned long long results[4] = { 0,0,0,0 };
unsigned long long a, b, c, d;
unsigned long long result1 = 0, result2 = 0, result3 = 0, result4 = 0, result = 0;
const unsigned long long ARRAY_SIZE = 200000000;
HANDLE writeCompleteEvent;
vector<unsigned long long> data1(ARRAY_SIZE);
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void CreateThreads();
void CalculateSingleThread();
double GetTimeElapsed(LARGE_INTEGER startTime, LARGE_INTEGER endTime);
double PCFreq;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

    for (unsigned long long i = 0; i < ARRAY_SIZE; ++i) {
        data1[i] = i;
    }

    a = 0;
    b = ARRAY_SIZE / 4;
    c = ARRAY_SIZE / 2;
    d = 3 * (ARRAY_SIZE / 4);


    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    PCFreq = double(frequency.QuadPart) / 1000.0;

    WNDCLASS wc = { 0 };

    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"MultithreadingApp";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClassW(&wc)) {
        return -1;
    }

    CreateWindowW(L"MultithreadingApp", L"Multithreading Example", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 400, 200, NULL, NULL, NULL, NULL);

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
        if (wp == 1) {
            CreateThreads();
        }
        else if (wp == 2) {
            CalculateSingleThread();
        }
        break;
    case WM_CREATE:
        CreateWindow(L"button", L"Start Multi-Threaded Calculation", WS_VISIBLE | WS_CHILD, 10, 10, 300, 30, hWnd, (HMENU)1, NULL, NULL);
        CreateWindow(L"button", L"Start Single-Threaded Calculation", WS_VISIBLE | WS_CHILD, 10, 50, 300, 30, hWnd, (HMENU)2, NULL, NULL);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }
    return 0;
}

DWORD WINAPI  ThreadFunction1(LPVOID aboba) {
    unsigned long long result = 0;
    for (unsigned long long i = a; i < b; ++i) {
        result += data1[i];
    }
    results[0] = result;
    return result;
}
DWORD WINAPI  ThreadFunction2(LPVOID aboba) {
    unsigned long long result = 0;
    for (unsigned long long i = b; i < c; ++i) {
        result += data1[i];
    }
    results[1] = result;
    return result;

}
DWORD WINAPI  ThreadFunction3(LPVOID aboba) {
    unsigned long long result = 0;
    for (unsigned long long i = c; i < d; ++i) {
        result += data1[i];
    }
    results[2] = result;
    return result;

}
DWORD WINAPI  ThreadFunction4(LPVOID aboba) {
    unsigned long long result = 0;
    for (unsigned long long i = d; i < ARRAY_SIZE; ++i) {
        result += data1[i];
    }
    results[3] = result;
    return result;

}

void CreateThreads() {
    LARGE_INTEGER startTime, endTime;
    QueryPerformanceCounter(&startTime);

    HANDLE threads[4];

    threads[0] = CreateThread(NULL, 0, ThreadFunction1, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 0, ThreadFunction2, NULL, 0, NULL);
    threads[2] = CreateThread(NULL, 0, ThreadFunction3, NULL, 0, NULL);
    threads[3] = CreateThread(NULL, 0, ThreadFunction4, NULL, 0, NULL);

    if (threads[0] != NULL)
        SetThreadPriority(threads[0], THREAD_PRIORITY_ABOVE_NORMAL);
    if (threads[1] != NULL)
        SetThreadPriority(threads[1], THREAD_PRIORITY_ABOVE_NORMAL);
    if (threads[2] != NULL)
        SetThreadPriority(threads[2], THREAD_PRIORITY_ABOVE_NORMAL);
    if (threads[3] != NULL)
        SetThreadPriority(threads[3], THREAD_PRIORITY_ABOVE_NORMAL);

    for (int i = 0; i < 4; i++) {
        WaitForSingleObject(threads[i], INFINITE);
        GetExitCodeThread(threads[i], (LPDWORD)(&results[i]));
    }
    unsigned long long totalSum = 0;
    for (int i = 0; i < 4; i++) {
        totalSum += results[i];
    }


    QueryPerformanceCounter(&endTime);
    double timeElapsed = GetTimeElapsed(startTime, endTime);

    wstring resultText = L"Sum using 4 threads: " + to_wstring(totalSum) + L"\nTime to complete (sec): " + to_wstring(timeElapsed);
    MessageBoxW(NULL, resultText.c_str(), L"Multi-Threaded Result", MB_OK | MB_ICONINFORMATION);
}

void CalculateSingleThread() {
    LARGE_INTEGER startTime, endTime;
    QueryPerformanceCounter(&startTime);
    unsigned long long result = 0;

    for (unsigned long long i = 0; i < ARRAY_SIZE; ++i) {
        result += data1[i];
    }

    QueryPerformanceCounter(&endTime);
    double timeElapsed = GetTimeElapsed(startTime, endTime);

    wstring resultText = L"Sum using singular thread: " + to_wstring(result) + L"\nTime to complete (sec): " + to_wstring(timeElapsed);


    MessageBoxW(NULL, resultText.c_str(), L"Single-Threaded Result", MB_OK | MB_ICONINFORMATION);
}

double GetTimeElapsed(LARGE_INTEGER startTime, LARGE_INTEGER endTime) {
    return (double(endTime.QuadPart - startTime.QuadPart) / PCFreq) / 1000.0;
}
