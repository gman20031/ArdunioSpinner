#include <assert.h>
#include <string.h>
#include <USBCore.h>

#include "SpinnerMessage.h"

#define TEST_PIN LED_BUILTIN
#define TEST_HORZ TEST_PIN
#define TEST_VERT TEST_PIN

// Interrupts on the uno are only allowed on pins 2 and 3  https://docs.arduino.cc/language-reference/en/functions/external-interrupts/digitalPinToInterrupt/?_gl=1*jx3aai*_up*MQ..*_ga*NDc2NTcyODQyLjE3OTE3MDQ4OTY.*_ga_NEXN8H46L5*czE3OTE3MDQ4OTQkbzEkZzAkdDE3OTE3MDQ4OTQkajYwJGwwJGgxMzM2MDgwMjMw 
constexpr int horzAPin = 2;
constexpr int horzBPin = 4;

constexpr int vertAPin = 3;
constexpr int vertBPin = 5;

volatile int deltaX = 0;
volatile int deltaY = 0;

void writeMovement(int8_t dx, int8_t dy)
{
  Serial.write( (uint8_t)MessageId::movement );
  Serial.write( dx );
  Serial.write( dy );
}

void writeMessage(MessageId id)
{
  Serial.write((uint8_t)id);
  Serial.write(0);
  Serial.write(0);
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
  if( -1 == digitalPinToInterrupt(horzAPin) )
    fail();
  if ( -1 == digitalPinToInterrupt(vertAPin) )
    fail();
  
  attachInterrupt( digitalPinToInterrupt(horzAPin) , &interruptHorizontal, CHANGE );
  attachInterrupt( digitalPinToInterrupt(vertAPin) , &interruptVerticle, CHANGE );

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

  writeMessage(MessageId::arduinoConnected);
}

void loop() {
  if( deltaX != 0 || deltaY != 0)
  {
    noInterrupts();
    writeMovement( deltaX, deltaY );
    deltaX = 0;
    deltaY = 0;
    interrupts();
  }

  delay(10);
}

void interruptVerticle() {
  int A = digitalRead(vertAPin);
  int B = digitalRead(vertBPin);
  A == B ? deltaY-- : deltaY++;
}

void interruptHorizontal(){
  int A = digitalRead(horzAPin);
  int B = digitalRead(horzBPin);
  A == B ? deltaX-- : deltaX++;
}





