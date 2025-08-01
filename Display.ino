void displayOut(String inputText)
{
  display.clear();
  display.drawString(0,0, inputText);
  display.display();
}

void displayLine(String inputText, int line)
{
  if (line == 1)
  {
    display.drawString(10,1, inputText);
    display.display();
  }
  else if (line == 2)
  {
    display.drawString(10,20, inputText);
    display.display();
  }
  else
  { 
    display.drawString(10,30, inputText);
    display.display();
  }
}

