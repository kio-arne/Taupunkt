void blink(int rounds)
{
  for(int i=0; i <= rounds; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(LED_shine);
    digitalWrite(LED_PIN, LOW);
  }
}

void timeBlink(int round, int ledDelay)
{
  for(int i=0; i<= round; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(ledDelay);
    digitalWrite(LED_PIN, LOW);
  }
}