#include <windows.h>
#include <iostream>
#include <string>

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    wchar_t title[256];
    // get window title for unicode
    GetWindowTextW(hwnd, title, sizeof(title) / sizeof(wchar_t));

    std::wstring target = L"Untitled - Notepad";

    if (wcsstr(title, target.c_str())) {
        std::wcout << L"Found: " << title << L", HWND: " << hwnd << std::endl;
        // optionally store hwnd via lParam or stop search: return FALSE;
    }
    return TRUE;
}

int main() {
    EnumWindows(EnumWindowsProc, 0);
    return 0;
}
