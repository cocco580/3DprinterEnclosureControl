

#include "TFT.hpp"

void TFT_StaticLayout()
{
  //general preset
  tft.begin();
  Serial.println("TFT begin");
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  Serial.println("TFT preset");
  //process value title
  tft.setCursor(2, 1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Process value:");
  //Temperature
  tft.setCursor(2, 19);
  tft.setTextSize(5);
  tft.print("T:--C");
  //Humidity
  tft.setCursor(2, 61);
  tft.setTextSize(3);
  tft.print("H:--%");
  //Smoke
  tft.setCursor(2, 87);
  tft.setTextSize(3);
  tft.print("Smk:----");
  //middle line
  tft.drawFastVLine(168,0,125,ILI9341_CYAN);
  tft.drawFastVLine(169,0,125,ILI9341_CYAN);
  tft.drawFastVLine(170,0,125,ILI9341_CYAN);
  //Power monitor title
  tft.setCursor(175, 1);
  tft.setTextSize(2);
  tft.print("Power:");
  //Voltage
  tft.setCursor(175, 19);
  tft.setTextSize(3);
  tft.print("V:--- V");
  //Current
  tft.setCursor(175, 45);
  tft.setTextSize(3);
  tft.print("A:--  A");
  //Power
  tft.setCursor(175, 71);
  tft.setTextSize(3);
  tft.print("P:--- W");
  //Energy
  tft.setCursor(175, 97);
  tft.setTextColor(0x03FF);
  tft.setTextSize(3);
  tft.print("E:----Wh");
  //Operation mode
  tft.setCursor(2, 165);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.print("Mode:");
  //Set point
  tft.setCursor(2, 191);
  tft.setTextSize(3);
  tft.print("Setpoint:");
}

void TFT_PrintTemperaturePV(int temperature)
{
  static int temperature_previus = 0;
  if (temperature == temperature_previus)
  {
    return;
  }
  if (temperature < 40)
  {
    tft.setTextColor(ILI9341_WHITE);
  }
  else if (temperature < 60)
  {
    tft.setTextColor(ILI9341_GREENYELLOW);
  }
  else
  {
    tft.setTextColor(ILI9341_RED);
  }
  tft.setTextSize(5);
  tft.fillRect(60, 19, 61, 40,ILI9341_BLACK);
  tft.setCursor(60, 19);
  tft.print(temperature);
  temperature_previus = temperature;
}


void TFT_PrintHumidityPV(int humidity)
{
  static int humidity_previus = 0;
  if (humidity == humidity_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(36, 61, 36, 24,ILI9341_BLACK);
  tft.setCursor(36, 61);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(humidity);
  humidity_previus = humidity;
}

void TFT_PrintSmoke(int smoke)
{
  static int smoke_previus = 0;
  if (smoke == smoke_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(74, 87, 75, 24,ILI9341_BLACK);
  tft.setCursor(74, 87);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(smoke);
  smoke_previus = smoke;
}

void TFT_PrintVoltage(int voltage)
{
  static int voltage_previus = 0;
  if (voltage == voltage_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(210, 19, 57, 24,ILI9341_BLACK);
  tft.setCursor(210, 19);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(voltage);
  voltage_previus = voltage;
}

void TFT_PrintCurrent(int current)
{
  static int current_previus = 0;
  if (current == current_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(210, 45, 57, 24,ILI9341_BLACK);
  tft.setCursor(210, 45);
  tft.setTextColor(ILI9341_WHITE);
  tft.print((int) current/10);
  tft.print(".");
  tft.print((int) current % 10);
  current_previus = current;
}

void TFT_PrintPower(int power)
{
  static int power_previus = 0;
  if (power == power_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(210, 71, 57, 24,ILI9341_BLACK);
  tft.setCursor(210, 71);
  tft.setTextColor(ILI9341_WHITE);
  tft.print((int) power);
  power_previus = power;
}

void TFT_PrintEnergy(int energy)
{
  static int energy_previus = 0;
  if (energy == energy_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(210, 97, 72, 24,ILI9341_BLACK);
  tft.setCursor(210, 97);
  tft.setTextColor(0x03FF);
  tft.print((int) energy);
  energy_previus = energy;
}

void TFT_PrintMode(int mode)
{
  static int mode_previus = 0;
  if (mode == mode_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(90, 165, 126, 24,ILI9341_BLACK);
  tft.setCursor(90, 165);
  tft.setTextColor(ILI9341_WHITE);
  switch (mode)
  {
  case 0:
    tft.print("Auto");
    break;
  case 1:
    tft.print("Manual");
    break;
  default:
    break;
  }
  mode_previus = mode;
}

void TFT_PrintSetpoitTemperature(int temperature)
{
  static int temperature_previus = 0;
  if (temperature == temperature_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(162, 191, 72, 24,ILI9341_BLACK);
  tft.setCursor(162, 191);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(temperature);
  tft.print("C");
  temperature_previus = temperature;
}

void TFT_PrintSetpoitPercentage(int percentage)
{
  static int percentage_previus = 0;
  if (percentage == percentage_previus)
  {
    return;
  }
  tft.setTextSize(3);
  tft.fillRect(162, 191, 72, 24,ILI9341_BLACK);
  tft.setCursor(162, 191);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(percentage);
  tft.print("%");
  percentage_previus = percentage;
}

void TFT_PrintFANbar(int FANpower)
{
  static int FANpower_previus = 0;
  FANpower = map(FANpower, 0, 255, 0 ,320); //scalatura del valore con la larghezza del display

  if (FANpower == FANpower_previus)
  {
    return;
  }
  else if (FANpower > FANpower_previus)
  {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    int RGB565 = 0; //RGB565 = (((red & 0b11111000)<<8) + ((green & 0b11111100)<<3)+(blue>>3));
    for (int column = FANpower_previus; column <= FANpower; column++)
    {
      if (column < 160)
      {
        green = 255;
      }
      else
      {
        green = round(column*-1.59375F+510);
      }
      if (column < 160)
      {
        red = round(column*1.59375F+0);
      }
      else
      {
        red = 255;
      }
      RGB565 = (((red & 0b11111000)<<8) + ((green & 0b11111100)<<3)+(blue>>3));
      tft.drawFastVLine(column,217,23, RGB565);
    }
  }
  else
  {
    for (int column = FANpower_previus; column >= FANpower; column--)
    {
      tft.drawFastVLine(column,217,23,ILI9341_BLACK);
    }
  }

  FANpower_previus = FANpower;
}