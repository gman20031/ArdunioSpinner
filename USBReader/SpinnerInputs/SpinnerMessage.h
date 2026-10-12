#pragma once

enum class MessageId : uint8_t
{
    testConncetion,
    mouseMovement,
    disconnect,
    connected,
    attachedInterrupts,
    failure,
    count,
};

struct Message
{
    MessageId id;
    int8_t dX;
    int8_t dY;
};
