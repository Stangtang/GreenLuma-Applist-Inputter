#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

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

int main(int argc, char* argv[]) {
    const string fileName = "input.txt";

    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }

    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    cout << "Switch to target window in 2 seconds...";
    Sleep(2000);

    string countStr = to_string(lines.size());
    sendString(countStr);
    sendEnter();
    Sleep(5);

    for (const auto& currentLine : lines) {
        sendString(currentLine);
        sendEnter();
        Sleep(5);  
    }

    return 0;
}