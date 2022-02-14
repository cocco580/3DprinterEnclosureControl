
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
//#include <SerialCommand.h>
#include "config.hpp"
#include "TFT.hpp"


//dichiarazione oggetti
Adafruit_BME280 bme;
//SerialCommand Serial_cmd;


void setup() {
  //noInterrupts();
  //inizializzazione porta seriale
  Serial.begin(115200);

  //avvio del display
  delay(2000);
  TFT_StaticLayout();
  Serial.println("TFT start");

  // Setup callbacks for SerialCommand commands
  //add_serial_commands();
  //Serial_cmd.setDefaultHandler(unrecognized);
  
  //inizializzaione BME280
  bme.begin(0x76);
  Serial.println("BME start");

  //setup analog input
  pinMode(POT_PIN, INPUT);
  pinMode(AC_VOLTAGE_PIN, INPUT);
  pinMode(AC_CURRENT_PIN, INPUT);
  //setup digital input
  pinMode(BUTTON_MODE_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_MODE_PIN), ISRbuttomMode, FALLING);
  //setup digital output
  pinMode(FAN_IN_CTRL_PIN, OUTPUT); analogWrite(FAN_IN_CTRL_PIN,0);
  pinMode(FAN_OUT_CTRL_PIN, OUTPUT); analogWrite(FAN_OUT_CTRL_PIN,0);
  pinMode(ALARM_PIN, OUTPUT);
  digitalWrite(ALARM_PIN,HIGH);

  //recupero parametri di processo precedentemente impostati e scritti sulla EEPROM
  EEPROM.get(0, kiFactor);
  EEPROM.get(4, SmokeUpperLimit);
  

  Serial.println("Setup completato");
  Status = AUTOMATICO;
  Serial.println("Stato macchina: Automatico");
  //interrupts();
}

void loop() {

  //Serial_cmd.readSerial();

  //Raccolta dati dal campo
  getPV();
  //Scrittura dati sul display
  TFT_PrintTemperaturePV((int) round(TempPV));
  TFT_PrintHumidityPV((int) round(HumidityPV));
  TFT_PrintSmoke((int) round(SmokePV));
  
  //Allarmi
  Alarm = (SmokePV < SmokeUpperLimit) ? false : true; //dà l'allarme se il sensore rilevamento fumo supera la soglia
  alarmSequence(Alarm);

  //Macchina a stati
  switch (Status)
  {
  case AUTOMATICO:
    TempSP = map(PotSPValue,0,1023,20,60);
    if ( (millis() - PreviousPIDTime) >= PID_PERIOD)
    {
      FanCtrlValue = controlIntegrator(TempSP, TempPV, kiFactor*(-1)); //ki negativo = raffreddamento
      analogWrite(FAN_IN_CTRL_PIN, FanCtrlValue);
      analogWrite(FAN_OUT_CTRL_PIN, FanCtrlValue);
      Serial.print("PID = PV: ");
      Serial.print(TempPV,1);
      Serial.print(" | SP: ");
      Serial.print(TempSP,1);
      Serial.print(" | Out: ");
      Serial.println(FanCtrlValue);
      PreviousPIDTime = millis();
    }
    TFT_PrintMode(HMI_AUTO);
    TFT_PrintSetpoitTemperature((int) round(TempSP));
    TFT_PrintFANbar(FanCtrlValue);
    break;

  case MANUALE:
    static int previus_FanCtrlValue = 0;
    FanCtrlValue = map(PotSPValue,0,1023,0,255);
    if (FanCtrlValue != previus_FanCtrlValue) // eseguo solo se il valore cambia
    {
      analogWrite(FAN_IN_CTRL_PIN, FanCtrlValue);
      analogWrite(FAN_OUT_CTRL_PIN, FanCtrlValue);
      previus_FanCtrlValue = FanCtrlValue;
    }
    TFT_PrintMode(HMI_MANUAL);
    TFT_PrintSetpoitPercentage(map(PotSPValue,0,1023,0,100));
    TFT_PrintFANbar(FanCtrlValue);
    break;

  case STOP:
    if (FanCtrlValue != 0)
    {
      FanCtrlValue = 0;
      analogWrite(FAN_IN_CTRL_PIN, FanCtrlValue);
      analogWrite(FAN_OUT_CTRL_PIN, FanCtrlValue);
    }
    TFT_PrintMode(HMI_STOP);
    TFT_PrintSetpoitPercentage(0);
    TFT_PrintFANbar(0);
    break;

  default:
    break;
  }
}

//sequenza di avvio e segnalazione allarme
void alarmSequence(bool StatoAllarme) {
  if (StatoAllarme){
    digitalWrite(ALARM_PIN, LOW); //attiva il relè
  }
  else{
    digitalWrite(ALARM_PIN, HIGH);
  }
  
}


//ISR bottone cambio modo di funzionamneto
void ISRbuttomMode()
{
  noInterrupts();
  static unsigned long timestamp = 0;
  if (millis()<(timestamp+BUTTON_DEBOUNCE_TIME))
  {
    return;
  }
  
  switch (Status)
  {
  case AUTOMATICO:
    Status = MANUALE;
    Serial.println("Stato macchina: Manuale");
    break;
  case MANUALE:
    Status = STOP;
    Serial.println("Stato macchina: Stop");
    break;
  case STOP:
    Status = AUTOMATICO;
    Serial.println("Stato macchina: Automatico");
    break;
  default:
    break;
  }
  timestamp = millis();
  interrupts();
}

//raccolta di tutti i dati provenienti dei sensori di campo
void getPV() {
  //variabili
  static unsigned long timeStamp = 0;
  static unsigned long PreviusTime = 0;
  
  //da pannello frontale
  PotSPValue = analogRead(POT_PIN);
  //ButtonMode = digitalRead(BUTTON_MODE_PIN);
  
  //sensori interni alla cabina di stampa
  TempPV = bme.readTemperature(); //in gradi celsius
  //PressPV = bme.readPressure() / 100.0F; // in hPa
  HumidityPV = bme.readHumidity(); //relativa
  //misura fumo
  for (int i = 0; i < 10; i++){
    SmokePV = analogRead(SMOKE_PIN);
  }
  SmokePV = SmokePV/10; //media dei valori letti
  SmokePV = map(SmokePV,0,1023,100,10000); //trasormazione in ppm

  //misura della potenza assorbita
	VacPV = 0;
	AacPV = 0;
	int pointNumeber = 0;
	timeStamp = millis();
	while (millis() < (timeStamp + PWR_LOG_PERIOD) ){ //misura i valori in continuo
		VacPV += pow(analogRead(AC_VOLTAGE_PIN),2);
		AacPV += pow(analogRead(AC_CURRENT_PIN),2);
		pointNumeber++;
	}
	VacPV = sqrt((1.0F/(float)pointNumeber)*VacPV);
	AacPV = sqrt((1.0F/(float)pointNumeber)*AacPV);
  //scalatura a grandenzza fisica ------TO DO---------
  VacPV = VacPV; //Volt
  AacPV = AacPV; //Ampere
  ACinstantPWR = 100; //W
  ACenergy = 1000; //Wh
  PreviusTime = millis();


//prosciutto e funghi
//salsicia e procini
//guzzi
}

//sistema di controllo base PID con solo la parte integrativa
int controlIntegrator(float Command, float Feedback, float ki) {
  static float Out = 0;
  static unsigned long PreviousTime = 0;
  float ERR = Command - Feedback; //errore come differenza tra SP e PV
  int deltaT = millis() - PreviousTime; //in millisecondi
  PreviousTime = millis();
  Out += (ki * ERR * deltaT) / 1000; //il fattore 1000 converte i millisecondi in secondi
  Out = (Out > 255) ? 255 : Out; //limite superiore
  Out = (Out < 0) ? 0 : Out; //limite inferiore
  return (int) Out;
}


/*
void settingProcedure(){

  String receivedString;
  delay(10);
  receivedString = Serial.readString();
  
  if (receivedString.startsWith("SetKI")) {
    receivedString.remove(0,5);
    kiFactor = receivedString.toFloat();
    EEPROM.put(0, kiFactor);
    Serial.print("ki factor now is: ");
    Serial.println(kiFactor);
  }

  if (receivedString.startsWith("GetKI")) {
    Serial.print("ki factor is: ");
    Serial.println(kiFactor);
  }

  if (receivedString.startsWith("SetSK")) {
    receivedString.remove(0,5);
    SmokeUpperLimit = receivedString.toInt();
    EEPROM.put(4, SmokeUpperLimit);
    Serial.print("Smoke TH now is: ");
    Serial.println(SmokeUpperLimit);
  }

  if (receivedString.startsWith("GetSK")) {
    Serial.print("Smoke TH is: ");
    Serial.println(SmokeUpperLimit);
  }

  Serial.flush();
}
*/

//funzione per controlare se un parametro booleano è passato da 0 a 1 rispetto alla chiamata precedente
bool ISRbuttomMode(bool parametro){
  static bool parametro_previus = false;
  if ( (parametro_previus == false) && (parametro == true) )
  {
    parametro_previus = parametro;
    return true;
  }
  else //if ((parametro_previus == true) && (parametro == false) || (parametro_previus == parametro) )
  {
    parametro_previus = parametro;
    return false;
  }
}


/*

void setKI()
{

}

void getKI()
{

}

void setSK()
{

}

void getSK()
{

}

void add_serial_commands()
{
  Serial_cmd.addCommand("SetKI", setKI);
  Serial_cmd.addCommand("GetKI", getKI);
  Serial_cmd.addCommand("SetSK", setSK);
  Serial_cmd.addCommand("GetSK", getSK);
  
}

void unrecognized(const char *command)
{
  Serial.println("Invalid cmd");
}

*/