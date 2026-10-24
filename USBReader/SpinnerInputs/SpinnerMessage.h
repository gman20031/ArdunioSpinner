#pragma once

enum class MessageId : signed char
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
    signed char  dX;
    signed char  dY;
};
