// statically link cpp runtime libraries into exe

#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

using namespace std;

void sendString(const string& str) {
    vector<INPUT> inputs;

    for (char c : str) {
        INPUT down;
        down.type = INPUT_KEYBOARD;
        down.ki.wVk = 0;
        down.ki.wScan = static_cast<WORD>(c);
        down.ki.dwFlags = KEYEVENTF_UNICODE;
        down.ki.time = 0;
        down.ki.dwExtraInfo = 0;
        inputs.push_back(down);

        INPUT up = down;
        up.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        inputs.push_back(up);
    }

    if (!inputs.empty()) {
        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }
}

void sendTab() {
    INPUT inputs[2] = {};

    // key down
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_TAB;

    // key up
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_TAB;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

void sendEnter() {
    INPUT inputs[2] = {};

    // key down
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_RETURN;

    // key up
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_RETURN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

bool focusTargetWindow(const wchar_t* targetWindowTitle) {
    // find target window
    HWND hwnd = FindWindowW(nullptr, targetWindowTitle);
    if (!hwnd) {
        wcerr << L"Target window '" << targetWindowTitle << L"' not found. Please ensure target window is running.\n"; // targetWindowTitle is wide
        return false;
    }

    // get thread ids
    DWORD targetThreadId = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThreadId = GetCurrentThreadId();

    // attach to target thread's input context
    AttachThreadInput(currentThreadId, targetThreadId, TRUE);

    // bring window to foreground
    ShowWindow(hwnd, SW_RESTORE);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // detach from thread input
    AttachThreadInput(currentThreadId, targetThreadId, FALSE);

    // give window time to focus
    this_thread::sleep_for(chrono::milliseconds(50));

    return true;
}

int main() {
    const string inputFile = "applist.txt";

    // if (argc != 2) {
    //     cerr << "Usage: " << fileName << " <input file>\n";
    //     return 1;
    // }

    ifstream file(inputFile);
    // if (!file.is_open()) {
    //     cerr << "Error opening file: " <<  << "\n";
    //     return 1;
    // }

    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    const wchar_t* targetWindowTitle1 = L"GreenLuma 2025";
    if (!focusTargetWindow(targetWindowTitle1)) {
        return 1;
    }

    sendTab();
    this_thread::sleep_for(chrono::milliseconds(10));
    sendEnter();
    this_thread::sleep_for(chrono::milliseconds(50));

    const wchar_t* targetWindowTitle2 = L"C:\\Program Files (x86)\\Steam\\Steam.exe";
    if (!focusTargetWindow(targetWindowTitle2)) {
        return 1;
    }

    string countStr = to_string(lines.size());
    sendString(countStr);
    sendEnter();
    this_thread::sleep_for(chrono::milliseconds(10));

    for (const auto& currentLine : lines) {
        sendString(currentLine);
        sendEnter();
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    return 0;
}