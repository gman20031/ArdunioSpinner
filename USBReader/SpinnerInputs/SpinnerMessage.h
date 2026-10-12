#pragma once

enum class MessageId : uint8_t
{
    testConncetion = 0,
    movement,
    arduinoDisconnect,
    arduinoConnected,
    computerConnected,
    computerDisconnected,
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
