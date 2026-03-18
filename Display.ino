void displayOut(String inputText)
{
  display.clear();
  display.drawString(0,0, inputText);
  display.display();
  Serial.println(inputText);
}

void displayLine(String inputText, int line)
{
  if (line == 1)
  {
    display.drawString(5,1, inputText);
    display.display();
  }
  else if (line == 2)
  {
    display.drawString(5,20, inputText);
    display.display();
  }
  else
  { 
    display.drawString(5,40, inputText);
    display.display();
  }
}

void displayProgress(int iCount, int iTotalCount)
{
  int progress = (iCount/iTotalCount) % 100;
  display.clear();
  Serial.println(iCount + iTotalCount + progress + " Fortschritt");
//  display.drawProgressBar(0, 32, 120, 20, progress);
}


void DisplaySetPixel(int xPoint, int yPoint, int Color)
{
//  display.setPixelColor(int16_t xPoint, int16_t yPoint, OLEDDISPLAY_COLOR )
}
