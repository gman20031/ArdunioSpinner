#include <assert.h>
#include <string.h>
#include <USBCore.h>

#include "SpinnerMessage.h"

#define TEST_PIN LED_BUILTIN
#define TEST_HORZ TEST_PIN
#define TEST_VERT TEST_PIN

// Interrupts on the uno are only allowed on pins 2 and 3  https://docs.arduino.cc/language-reference/en/functions/external-interrupts/digitalPinToInterrupt/?_gl=1*jx3aai*_up*MQ..*_ga*NDc2NTcyODQyLjE3OTE3MDQ4OTY.*_ga_NEXN8H46L5*czE3OTE3MDQ4OTQkbzEkZzAkdDE3OTE3MDQ4OTQkajYwJGwwJGgxMzM2MDgwMjMw 
constexpr int horzAPin = digitalPinToInterrupt(2);
constexpr int horzBPin = 4;

constexpr int vertAPin = digitalPinToInterrupt(3);
constexpr int vertBPin = 5;

volatile Message gMessage;
volatile bool mouseMoved = false;

void writeMessage(Message* pMessage)
{
  char* data = reinterpret_cast<char*>(pMessage);
  Serial.write(data, sizeof(Message) );
}

void writeMessage(MessageId id)
{
  Serial.write((uint8_t)id);
}

void fail()
{
    writeMessage(MessageId::failure);
    Serial.flush();
    abort();
}

struct scopedTask
{
  scopedTask() { digitalWrite(TEST_PIN, HIGH); }
  ~scopedTask() { digitalWrite(TEST_PIN, LOW); }
};

void makeInterrupts()
{
  if( -1 == horzAPin )
    fail();
  if ( -1 == vertAPin )
    fail();
  
  attachInterrupt( horzAPin , &interruptHorizontal, CHANGE );
  attachInterrupt( vertAPin , &interruptVerticle, CHANGE );

  writeMessage(MessageId::attachedInterrupts);
}

void setup() {
  Serial.begin(9600);

  while(!Serial) // wait for computer to be connected.
  Serial.flush();

  makeInterrupts();

  pinMode( TEST_PIN  , OUTPUT);
  pinMode( horzAPin  , INPUT);
  pinMode( horzBPin  , INPUT);
  pinMode( vertAPin  , INPUT);
  pinMode( vertBPin  , INPUT);

  gMessage.dX = 0;
  gMessage.dX = 0;
  gMessage.id = MessageId::mouseMovement;

  writeMessage(MessageId::connected);
}

void loop() {
  if ( mouseMoved )
  {
    noInterrupts();
    scopedTask task;
    
    gMessage.dY = digitalRead(vertAPin) == digitalRead(vertBPin) ? -1 : 1;
    gMessage.dX = digitalRead(horzAPin) == digitalRead(horzBPin) ? -1 : 1;
    writeMessage( &gMessage );
    
    mouseMoved = false;
    interrupts();
  }

  delay(50);
}

void interruptVerticle() {
  mouseMoved = true;
}

void interruptHorizontal(){
  mouseMoved = true;
}





