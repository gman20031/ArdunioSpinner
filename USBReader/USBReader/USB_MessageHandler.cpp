#include "USB_MessageHandler.h"

#include <Windows.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <iostream>
#include <format>

#include "../SpinnerInputs/SpinnerMessage.h"

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
    if (line.empty())
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

USB_Error MessageHandler::Open()
{
    std::wstring port = GetPort();
    if (port.empty())
        return USB_Error::kNoPorts;
    m_handle = CreateFileW(port.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (m_handle == INVALID_HANDLE_VALUE)
    {
        std::wcout << "Error opening serial port {" << port << "}\n";
        return USB_Error::kBadPort;
    }

    DCB serialParameters{};
    serialParameters.DCBlength = sizeof(serialParameters);
    GetCommState(m_handle, &serialParameters);
    serialParameters.BaudRate = CBR_9600;
    serialParameters.ByteSize = 8;
    serialParameters.Parity = NOPARITY;
    serialParameters.StopBits = ONE5STOPBITS;
    SetCommState(m_handle, &serialParameters);

    std::wcout << "opening on port " << port<< '\n';
    m_running = true;
    return USB_Error::kGood;
}

void MessageHandler::Close()
{
    CloseHandle(m_handle);
    m_running = false;
}

USB_ReadResult MessageHandler::ReadMessage(Message* outMessage)
{
    m_readError = ReadInMessage(outMessage, m_handle);
    return m_readError;
}

USB_ReadResult MessageHandler::ReadInMessage(Message* pOut, void* handle)
{
    constexpr int messageSize = sizeof(Message);
    char buffer[messageSize] = {};

    ULONG readBytes = 0;
    while (readBytes < messageSize)
    {
        ULONG got = 0;
        if (!ReadFile(handle, buffer, messageSize - readBytes, &got, NULL))
            return USB_ReadResult::kDisconnected;

        readBytes += got;
    }

    // not a real id.
    if (buffer[0] >= (int)MessageId::count)
        return USB_ReadResult::kBadMessage;

    pOut->id = static_cast<MessageId>(buffer[0]);
    pOut->dX = static_cast<int8_t>(buffer[1]);
    pOut->dY = static_cast<int8_t>(buffer[2]);

    return USB_ReadResult::kGoodMessage;
}

constexpr const char* ToString(USB_ReadResult error)
{
    switch (error)
    {
    case USB_ReadResult::kGoodMessage: return "Good";
    case USB_ReadResult::kBadMessage: return "Bad Message";
    case USB_ReadResult::kDisconnected: return "Disconnected";
    case USB_ReadResult::kTimeout: return "Timeout";
    case USB_ReadResult::kIdle: return "Idle";
    }
    return "Ill formed error";
}
