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

int main() {
    vector<string> lines;
    string line;

    cerr << "Paste App IDs separated by line (press Enter, then Ctrl+Z, then Enter to finish):\n"; // Windows EOF signal

    while (getline(cin, line)) {
        lines.push_back(line);
    }

    cerr << "Input captured. Switch to target window in 2 seconds...\n";
    this_thread::sleep_for(chrono::milliseconds(2000));

    sendString(to_string(lines.size()));
    sendEnter();
    this_thread::sleep_for(chrono::milliseconds(10));

    for (const auto& currentLine : lines) {
        sendString(currentLine);
        sendEnter();
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    return 0;
}