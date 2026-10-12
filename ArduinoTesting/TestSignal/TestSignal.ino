#define TEST_PIN LED_BUILTIN

constexpr int horzAPin = 2;
constexpr int horzBPin = 4;

constexpr int vertAPin = 3;
constexpr int vertBPin = 5;

volatile int gXCount = 0;
volatile int gYCount = 0;

void interruptHorz()
{
  int A = digitalRead(horzAPin);
  int B = digitalRead(horzBPin);
  A == B ? gXCount-- : gXCount++;
}

void setup() {
  Serial.begin(9600);
  while(!Serial);

  // put your setup code here, to run once:
  pinMode( TEST_PIN  , OUTPUT);
  pinMode( horzAPin  , INPUT);
  pinMode( horzBPin  , INPUT);
  pinMode( vertAPin  , INPUT);
  pinMode( vertBPin  , INPUT);

  attachInterrupt( digitalPinToInterrupt(horzAPin), &interruptHorz, CHANGE);
}

void loop() {
  Serial.print( "{");
  Serial.print( gXCount );
  Serial.print( "," );
  Serial.print( gYCount );
  Serial.println( "}" );

  gXCount = 0;
  gYCount = 0;

  delay(10);
}
