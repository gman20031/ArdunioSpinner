
#include <Windows.h>
#include <stdint.h>

#include <iostream>
#include <format>
#include <string>

#include "../SpinnerInputs/SpinnerMessage.h"

#define DISCONNECTED 0
#define FAILURE -1
#define BAD_MESSAGE -2

static std::wstring GetPort()
{
    return L"COM4";
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
    case MessageId::mouseMovement: return std::format("MouseMovement {{{},{}}}", msg.dX, msg.dY);
    case MessageId::connected: return "Connected";
    case MessageId::disconnect: return "Disconnected";
    case MessageId::attachedInterrupts: return "Interrupts attached";
    case MessageId::testConncetion: return "Connection test";
    case MessageId::failure: return "Failure";
    }
    return "";
}

int main()
{
    std::wstring port = GetPort();
    
    HANDLE serialHandle = CreateFileW(port.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (serialHandle == INVALID_HANDLE_VALUE) 
    {
        std::cout << "Error opening serial port\n";
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
        if (msg.id == MessageId::disconnect)
            return DISCONNECTED;
        if (msg.id == MessageId::failure)
            return FAILURE;
    }
    return BAD_MESSAGE;
}