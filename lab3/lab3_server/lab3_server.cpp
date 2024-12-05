#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <chrono>
#include <sstream>
#include <iomanip>

#define PIPE_NAME L"\\\\.\\pipe\\LogPipe"
#define BUFFER_SIZE 1024

void logMessage(const std::string& message, const std::string& clientId) {
    std::ofstream logFile("server_log.txt", std::ios::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);

        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));

        logFile << "[" << timestamp << "] [Client " << clientId << "] " << message << "\n";
        logFile.close();
    }
}

int main() {
    HANDLE pipe;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    std::cout << "Server: Starting...\n";

    pipe = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFFER_SIZE,
        BUFFER_SIZE,
        0,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Could not create named pipe. Code: " << GetLastError() << "\n";
        return 1;
    }

    std::cout << "Server: Waiting for clients...\n";

    if (ConnectNamedPipe(pipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
        std::cout << "Server: Client connected.\n";

        while (true) {
            if (ReadFile(pipe, buffer, BUFFER_SIZE - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0'; 
                std::string message(buffer);

                std::string clientId = message.substr(0, message.find(' '));
                std::string logMessageContent = message.substr(message.find(' ') + 1);

                logMessage(logMessageContent, clientId);
                std::cout << "Server: Logged message from " << clientId << "\n";
            }
            else {
                DWORD error = GetLastError();
                if (error == ERROR_BROKEN_PIPE) {
                    std::cerr << "Server: Client disconnected. Waiting for a new connection...\n";

                    DisconnectNamedPipe(pipe);
                    ConnectNamedPipe(pipe, NULL);
                }
                else if (error == ERROR_NO_DATA) {
                    Sleep(100);
                }
                else {
                    std::cerr << "Error: Could not read from pipe. Code: " << error << "\n";
                    break;
                }
            }
        }

    }

    CloseHandle(pipe);
    return 0;
}
