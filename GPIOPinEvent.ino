uint32_t LastTimePressResetPin;
bool IsResetPinPressed()
{
  if(digitalRead(RESET_PIN) == 0)
  {
    if(millis() - LastTimePressResetPin > 3000)
    {
      LastTimePressResetPin = millis();
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    LastTimePressResetPin == millis();
    return false;
  }
}
