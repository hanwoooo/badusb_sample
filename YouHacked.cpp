#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>

struct WindowData {
    int x;
    int y;
    int id;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 0, 0));
        HFONT hFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Arial");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        TextOutW(hdc, 30, 30, L"YouHacked!!", 11);
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI CreateRandomWindow(LPVOID lpParam) {
    WindowData* data = (WindowData*)lpParam;

    // 클래스 이름을 static으로 저장 (중요!)
    static wchar_t className[50];
    wsprintfW(className, L"HackWindow%d", data->id);

    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        className,
        L"WARNING",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        data->x, data->y, 300, 150,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete data;
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    srand((unsigned int)time(NULL));  // unsigned int로 캐스팅

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HANDLE threads[10];

    for (int i = 0; i < 10; ++i) {
        WindowData* data = new WindowData;
        data->x = rand() % (screenWidth - 300);
        data->y = rand() % (screenHeight - 150);
        data->id = i;

        threads[i] = CreateThread(NULL, 0, CreateRandomWindow, data, 0, NULL);
        Sleep(100);
    }

    WaitForMultipleObjects(10, threads, TRUE, INFINITE);

    for (int i = 0; i < 10; ++i) {
        CloseHandle(threads[i]);
    }

    return 0;
}