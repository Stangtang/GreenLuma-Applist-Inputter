#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
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

void sendEnter() {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_RETURN;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_RETURN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

bool focusTargetWindow() {
    const wchar_t* targetWindowTitle = L"C:\\Program Files (x86)\\Steam\\Steam.exe";
    
    // find target window
    HWND hwnd = FindWindowW(nullptr, targetWindowTitle);
    if (!hwnd) {
        cerr << "Target window 'C:\\Program Files (x86)\\Steam\\Steam.exe' not found. Please ensure DLL injector is running.\n";
        return false;
    }

    // get thread IDs
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
    this_thread::sleep_for(chrono::milliseconds(100));

    return true;
}

int main() {
    vector<string> lines;
    string line;

    cerr << "Paste App IDs separated by line. When done, press Enter, then Ctrl+Z, then Enter again:\n";

    while (getline(cin, line)) {
        lines.push_back(line);
    }

    cerr << "Input captured. Focusing target window and entering App IDs...\n";

    if (!focusTargetWindow()) {
        // cerr << "Failed to focus target window. Exiting.\n";
        return 1;
    }

    // send line count
    sendString(to_string(lines.size()));
    sendEnter();
    this_thread::sleep_for(chrono::milliseconds(20));

    // send lines
    for (const auto& currentLine : lines) {
        sendString(currentLine);
        sendEnter();
        this_thread::sleep_for(chrono::milliseconds(20));
    }

    return 0;
}