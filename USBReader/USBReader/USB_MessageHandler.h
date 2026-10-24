#pragma once

#include "../SpinnerInputs/SpinnerMessage.h"

enum class USB_Error : int
{
    kBadPort = -3,
    kFailure = -2,
    kBadMessage = -1,
    kDisconnected = 0,
    kGood,
    kNoPorts,
};

enum class USB_ReadResult
{
    kGoodMessage,
    kBadMessage,
    kDisconnected,
    kTimeout,
    kIdle,
};

extern constexpr const char* ToString(USB_ReadResult error);

class MessageHandler
{
    void* m_handle = nullptr;
    bool m_running = false;
    USB_ReadResult m_readError = USB_ReadResult::kBadMessage;
public:
    USB_Error Open();
    void Close();
    USB_ReadResult ReadMessage(Message* outMessage);

    USB_ReadResult GetError() const { return m_readError; }
    bool IsRunning() const { return m_running; }
private:
    static USB_ReadResult ReadInMessage(Message* pOut, void* handle);
};