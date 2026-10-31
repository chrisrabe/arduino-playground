const int ledPin = 12;
const int switchPin = 2;

void setup()
{ 
  pinMode(ledPin,OUTPUT);
  pinMode(switchPin,INPUT_PULLUP);
}

void loop() 
{  
  int digitalVal = digitalRead(switchPin);
  if(HIGH == digitalVal)
  {
    digitalWrite(ledPin,LOW);
  }
  else
  {
    digitalWrite(ledPin,HIGH);
  }
}

