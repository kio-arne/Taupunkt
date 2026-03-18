float tp(Measure innen, Measure aussen)
  {
    float returnValue = tp_one(innen) - tp_one(aussen);
    return returnValue;
  }

  float tp_one(Measure input)
  {
    //Defaults
    float a, b, sdd, dd, v, returnValue;
    if (input.Temperature >= 0) 
      {
        a = 7.5; 
        b = 237.3;
      } else {
        a = 7.6;
        b = 240.7;
      }

    //Sättigungsdampfdruck in hPa
    sdd = 6.1078 * pow(10, (a*input.Temperature)/(b+input.Temperature));  

    //Dampfdruck
    dd = sdd * (input.Humidity/100);

    //v-Paramter
    v = log10(dd/6.1078);

    //Taupunkttemperatur
    returnValue = (b*v) /(a-v);
    return returnValue;
  }