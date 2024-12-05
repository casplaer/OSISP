#include <iostream>
#include <string>
#include <windows.h>

#define PIPE_NAME L"\\\\.\\pipe\\LogPipe"
#define BUFFER_SIZE 1024

void sendMessages(HANDLE pipe, const std::string& clientId) {
    while (true) {
        std::string message;
        std::cout << "Enter message to send (or 'exit' to quit): ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        std::string fullMessage = clientId + " " + message;
        DWORD bytesWritten;
        if (!WriteFile(pipe, fullMessage.c_str(), (DWORD)(fullMessage.size() + 1), &bytesWritten, NULL)) {
            std::cerr << "Error: Could not write to pipe. Code: " << GetLastError() << "\n";
            break;
        }
    }
}

int main() {
    Sleep(1000);
    HANDLE pipe = CreateFile(
        PIPE_NAME,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Could not open pipe. Code: " << GetLastError() << "\n";
        return 1;
    }

    std::string clientId;
    std::cout << "Enter client ID: ";
    std::cin >> clientId;
    std::cin.ignore();

    sendMessages(pipe, clientId);

    CloseHandle(pipe);
    return 0;
}
