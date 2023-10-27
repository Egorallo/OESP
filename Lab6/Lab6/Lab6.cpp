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
//unsigned long long result = 0;
const unsigned long long ARRAY_SIZE = 200000000;
CRITICAL_SECTION section;
HANDLE writeCompleteEvent;
HANDLE Thread1Complete;
HANDLE Thread2Complete;
HANDLE Thread3Complete;
HANDLE Thread4Complete;
int flag = 0;

vector<unsigned long long> data1(ARRAY_SIZE);
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void CreateThreads();
void CalculateSingleThread();
double GetTimeElapsed(LARGE_INTEGER startTime, LARGE_INTEGER endTime);
DWORD WINAPI SingleThreadCalculation(LPVOID lpParam);
DWORD WINAPI MultiThreadMessage(LPVOID lpParam);
double PCFreq;
double Thread1TimeElapsed, Thread2TimeElapsed, Thread3TimeElapsed, Thread4TimeElapsed;

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
    
    InitializeCriticalSection(&section);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteCriticalSection(&section);

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
    LARGE_INTEGER startTime, endTime;
    QueryPerformanceCounter(&startTime);
    EnterCriticalSection(&section);
    for (unsigned long long i = a; i < b; ++i) {
        result += data1[i];
    }
    results[0] = result;
    LeaveCriticalSection(&section);

    QueryPerformanceCounter(&endTime);
    Thread1TimeElapsed = GetTimeElapsed(startTime, endTime);

    flag++;
    MultiThreadMessage(NULL);

    return result;
}
DWORD WINAPI  ThreadFunction2(LPVOID aboba) {
    LARGE_INTEGER startTime, endTime;
    QueryPerformanceCounter(&startTime);
    EnterCriticalSection(&section);
    for (unsigned long long i = b; i < c; ++i) {
        result += data1[i];
    }
    results[1] = result;
    LeaveCriticalSection(&section);
    QueryPerformanceCounter(&endTime);
    Thread2TimeElapsed = GetTimeElapsed(startTime, endTime);
    
    flag++;
    MultiThreadMessage(NULL);

    return result;

}
DWORD WINAPI  ThreadFunction3(LPVOID aboba) {
    LARGE_INTEGER startTime, endTime;
    QueryPerformanceCounter(&startTime);
    EnterCriticalSection(&section);
    for (unsigned long long i = c; i < d; ++i) {
        result += data1[i];
    }
    
    results[2] = result;
    LeaveCriticalSection(&section);

    QueryPerformanceCounter(&endTime);
    Thread3TimeElapsed = GetTimeElapsed(startTime, endTime);

    flag++;
    MultiThreadMessage(NULL);

    return result;

}
DWORD WINAPI  ThreadFunction4(LPVOID aboba) {
    LARGE_INTEGER startTime, endTime;
    QueryPerformanceCounter(&startTime);
    EnterCriticalSection(&section);
    for (unsigned long long i = d; i < ARRAY_SIZE; ++i) {       
        result += data1[i];     
    }
    results[3] = result;
    LeaveCriticalSection(&section);

    QueryPerformanceCounter(&endTime);
    Thread4TimeElapsed = GetTimeElapsed(startTime, endTime);

    flag++;
    MultiThreadMessage(NULL);

    return result;


}

void CreateThreads() {
    HANDLE threads[4];

    threads[0] = CreateThread(NULL, 0, ThreadFunction1, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 0, ThreadFunction2, NULL, 0, NULL);
    threads[2] = CreateThread(NULL, 0, ThreadFunction3, NULL, 0, NULL);
    threads[3] = CreateThread(NULL, 0, ThreadFunction4, NULL, 0, NULL);


}

void CalculateSingleThread() {
    HANDLE SingleThread = CreateThread(NULL, 0, SingleThreadCalculation, NULL, 0, NULL);
}

double GetTimeElapsed(LARGE_INTEGER startTime, LARGE_INTEGER endTime) {
    return (double(endTime.QuadPart - startTime.QuadPart) / PCFreq) / 1000.0;
}

DWORD WINAPI SingleThreadCalculation(LPVOID lpParam) {

    LARGE_INTEGER startTime, endTime;
    QueryPerformanceCounter(&startTime);

    unsigned long long result_2 = 0;
    for (unsigned long long i = 0; i < ARRAY_SIZE; ++i) {
        result_2 += data1[i];
    }

    QueryPerformanceCounter(&endTime);
    double timeElapsed = GetTimeElapsed(startTime, endTime);

    wstring resultText = L"\nTime to complete (sec): " + to_wstring(timeElapsed);


    MessageBoxW(NULL, resultText.c_str(), L"Single-Threaded Result", MB_OK | MB_ICONINFORMATION);
    return 0;
}
DWORD WINAPI MultiThreadMessage(LPVOID lpParam) {
    if (flag != 4) {
        return 0;
    }

    unsigned long long totalSum = 0;
    for (int i = 0; i < 4; i++) {
        totalSum += results[i];
    }

    wstring resultText = L"\nTime to complete (sec): " + to_wstring(Thread1TimeElapsed) + L": " +  to_wstring(Thread2TimeElapsed) + L": " + to_wstring(Thread3TimeElapsed) + L": " + to_wstring(Thread4TimeElapsed);
    MessageBoxW(NULL, resultText.c_str(), L"Multi-Threaded Result", MB_OK | MB_ICONINFORMATION);
    result = 0;
    flag = 0;
    for (int i = 0; i < 4; i++) {
        results[i] = 0;
    }
    return 0;
}

