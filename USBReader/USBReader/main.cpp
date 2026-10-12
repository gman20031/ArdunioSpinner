
#include <Windows.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <iostream>
#include <format>
#include <string>

#include "../SpinnerInputs/SpinnerMessage.h"

#define NO_PORTS 1
#define DISCONNECTED 0
#define FAILURE -1
#define BAD_MESSAGE -2

static std::wstring GetPort()
{
    HKEY key{};

    std::vector<std::wstring> names;

    if (ERROR_SUCCESS == RegOpenKeyW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", &key))
    {
        WCHAR valueName[255];
        BYTE portName[255];
        DWORD nameSize, dataSize;
        DWORD index = 0;

        while (true) {
            nameSize = sizeof(valueName);
            dataSize = sizeof(portName);

            // Enumerate the values under the key
            LSTATUS status = RegEnumValueW(key, index, valueName, &nameSize, NULL, NULL, portName, &dataSize);

            if (status == ERROR_SUCCESS) {
                names.emplace_back(reinterpret_cast<WCHAR*>(portName));
                index++;
            }
            else {
                break; // No more entries
            }
        }

        RegCloseKey(key);
    }

    if (names.empty())
    {
        std::cout << "No ports available!\n";
        return {};
    }

    std::cout << "Pick your port. Ports avialable:\n";
    for (std::wstring& name : names)
    {
        std::wcout << "   " << name << '\n';
    }

    std::cout << "Port: ";
    std::wstring line;
    std::getline(std::wcin, line);
    if (line.empty() )
        line = names[0];

    bool good = false;
    for (std::wstring& name : names)
    {
        if (name == line)
        {
            good = true;
            break;
        }
    }

    return line;
}

static bool ReadInMessage(Message* out, HANDLE file)
{
    char buffer[sizeof(Message)] = {};
    ULONG readBytes = 0;
    while (!ReadFile(file, buffer, sizeof(Message), &readBytes, NULL));

    if (buffer[0] >= (int)MessageId::count)
        return false;
    out->id = static_cast<MessageId>(buffer[0]);
    out->dX = static_cast<int8_t>(buffer[1]);
    out->dY = static_cast<int8_t>(buffer[2]);

    return true;
}

static std::string MessageText(const Message msg)
{
    switch (msg.id)
    {
    case MessageId::testConncetion: return "Connection test";
    case MessageId::movement: return std::format("movement {{{},{}}}", msg.dX, msg.dY);
    case MessageId::arduinoConnected: return "Arduino Connected";
    case MessageId::arduinoDisconnect: return "Arduino Disconnected";
    case MessageId::computerConnected: return "?? This Connected ??";
    case MessageId::computerDisconnected: return "?? This Disconnected ??";
    case MessageId::attachedInterrupts: return "Interrupts attached";
    case MessageId::failure: return "Failure";
    }
    return "";
}

static void UseMovement(int dx, int dy)
{
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = dx;
    input.mi.dy = dy;

    SendInput(1, &input, sizeof(INPUT));
}

int main()
{
    std::wstring port = GetPort();
    if (port.empty())
        return NO_PORTS;
    
    HANDLE serialHandle = CreateFileW(port.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (serialHandle == INVALID_HANDLE_VALUE) 
    {
        std::wcout << "Error opening serial port {" << port << "}\n";
        return 1;
    }

    DCB serialParameters{};
    serialParameters.DCBlength = sizeof(serialParameters);
    GetCommState(serialHandle, &serialParameters);
    serialParameters.BaudRate = CBR_9600,
    serialParameters.ByteSize = 8,
    serialParameters.Parity = NOPARITY,
    serialParameters.StopBits = ONE5STOPBITS,
    SetCommState(serialHandle, &serialParameters);

    std::wcout << "Opened port " << port << '\n';

    Message msg{};
    while (ReadInMessage(&msg, serialHandle))
    {
        std::cout << MessageText(msg) << '\n';
        switch (msg.id)
        {
        case MessageId::movement: UseMovement(msg.dX, msg.dY); break;
        case MessageId::arduinoDisconnect: return DISCONNECTED;
        case MessageId::failure: return FAILURE;
        default:
            break;
        }
    }
    return BAD_MESSAGE;
}