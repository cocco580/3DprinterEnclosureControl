
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SerialCommand.h>

#include "config.hpp"
#include "display.hpp"

//dichiarazione oggetti
Adafruit_BME280 bme;
SerialCommand Serial_cmd;


void setup() {

  //avvio dei display
  beginDisplayPV();
  delay(1000);
  beginDisplayFAN();
  delay(1000);

  // Setup callbacks for SerialCommand commands
  add_serial_commands();
  
  //inizializzaione BME280
  bme.begin(0x76);

  //setup analog input
  pinMode(PotSPPin, INPUT);
  pinMode(VacPVPin, INPUT);
  pinMode(AacPVPin, INPUT);
  //setup digital input
  pinMode(BottonModePin, INPUT);
  //setup digital output
  pinMode(FanInCtrlPin, OUTPUT);
  analogWrite(FanInCtrlPin,0);
  pinMode(FanOutCtrlPin, OUTPUT);
  analogWrite(FanOutCtrlPin,0);
  pinMode(AlarmPin, OUTPUT);
  digitalWrite(AlarmPin,HIGH);

  //recupero parametri di processo precedentemente impostati e scritti sulla EEPROM
  EEPROM.get(0, kiFactor);
  EEPROM.get(4, SmokeUpperLimit);
  
  //inizializzazione porta seriale
  Serial.begin(9600);
  Serial.println("Setup completato");
  Status = AUTOMATICO;
  Serial.println("Stato macchina: Automatico");
}

void loop() {

  //Serial_cmd.readSerial();
  getPV(); //raccolta dati dal campo
  printDisplayPV(TempPV, HumidityPV, SmokePV, VacPV, AacPV, ACinstantPWR, ACenergy); //print del display dei valori di processo
  Alarm = (SmokePV < SmokeUpperLimit) ? false : true; //dà l'allarme se il sensore rilevamento fumo supera la soglia
  alarmSequence(Alarm);

  //Macchina a stati
  switch (Status)
  {
  case AUTOMATICO:
    if (risingDetect(BottonMode)) //cambio di stato ogni volta che premo il pulsante
    {
      FanCtrlValue = 0;
      analogWrite(FanInCtrlPin, FanCtrlValue);
      analogWrite(FanOutCtrlPin, FanCtrlValue);      
      Status = MANUALE;
      Serial.println("Stato macchina: Manuale");
      break;
    }
    if ( (millis() - PreviousPIDTime) >= PID_PERIOD)
    {
      FanCtrlValue = controlIntegrator(TempSP, TempPV, kiFactor*(-1)); //ki negativo = raffreddamento
      analogWrite(FanInCtrlPin, FanCtrlValue);
      analogWrite(FanOutCtrlPin, FanCtrlValue);
      PreviousPIDTime = millis();
    }
    printDisplayFAN_AUTO(TempSP, FanCtrlValue); //print del display ventole in automatico
    break;

  case MANUALE:
    if (risingDetect(BottonMode)) //cambio di stato ogni volta che premo il pulsante
    { 
      FanCtrlValue = 0;
      analogWrite(FanInCtrlPin, FanCtrlValue);
      analogWrite(FanOutCtrlPin, FanCtrlValue);
      Status = STOP;
      Serial.println("Stato macchina: Stop");
      break;
    }
    FanCtrlValue = map(PotSPValue,0,1023,0,255);
    analogWrite(FanInCtrlPin, FanCtrlValue);
    analogWrite(FanOutCtrlPin, FanCtrlValue);
    printDisplayFAN_MAN(FanCtrlValue);
    break;

  case STOP:
    if (risingDetect(BottonMode)) //cambio di stato ogni volta che premo il pulsante
    {
      Status = AUTOMATICO;
      Serial.println("Stato macchina: Automatico");
      break;
    }
    printDisplayFAN_STOP();
    break;

  default:
    break;
  }
  delay(1);
  
}

//sequenza di avvio e segnalazione allarme
void alarmSequence(bool StatoAllarme) {
  if (StatoAllarme){
    digitalWrite(AlarmPin, LOW); //attiva il relè
  }
  else{
    digitalWrite(AlarmPin, HIGH);
  }
  
}

//raccolta di tutti i dati provenienti dei sensori di campo
void getPV() {
  //variabili
  static unsigned long StopTime = 0;
  static unsigned long PreviusTime = 0;
  
  //da pannello frontale
  PotSPValue = analogRead(PotSPPin);
  BottonMode = digitalRead(BottonModePin);
  
  //sensori interni alla cabina di stampa
  TempSP = map(PotSPValue,0,1023,20,60);
  TempPV = bme.readTemperature(); //in gradi celsius
  PressPV = bme.readPressure() / 100.0F; // in hPa
  HumidityPV = bme.readHumidity(); //relativa

  //misura della potenza assorbita
  VacPV = 0;
  AacPV = 0;
  StopTime = millis();
  while ((millis()-StopTime) < 20 ){ //misura i valori in continuo per 20ms un ciclo intero di sinusoidale
    VacPV = max(analogRead(VacPVPin),VacPV); //aggiorna la variabile sempre con il valore max trovato
    AacPV = max(analogRead(AacPVPin),AacPV); //aggiorna la variabile sempre con il valore max trovato
  }
  for (int i=0; i<9; i++) { //array shift left
    VacPVhistory[i] = VacPVhistory[i+1];
    AacPVhistory[i] = AacPVhistory[i+1];
  }
  VacPVhistory[9] = VacPV; //aggiungi ultimo valore raccolto allo storico
  AacPVhistory[9] = AacPV; //aggiungi ultimo valore raccolto allo storico
  //valore medio dello storico
  for (int i=0; i<10; i++){ 
    VacPV += VacPVhistory[i];
    AacPV += AacPVhistory[i];
  }
  VacPV /= 10;
  AacPV /= 10;
  //scalatura a grandenzza fisica
  VacPV = VacPV * VacPVfactor * RMSfactor; //Volt
  AacPV = AacPV * AacPVfactor * RMSfactor; //Ampere
  ACinstantPWR = VacPV * AacPV / 1000; //kW
  ACenergy += ACinstantPWR * (millis() - PreviusTime)/3600000; //kWh
  PreviusTime = millis();

  //misura fumo
  for (int i = 0; i < 10; i++){
    SmokePV = analogRead(SmokePVPin);
  }
  SmokePV = SmokePV /10; //media dei valori letti
  SmokePV = map(SmokePV,0,1023,100,10000); //trasormazione in ppm

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
bool risingDetect(bool parametro){
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


void add_serial_commands()
{
  Serial_cmd.addCommand("SetKI", setKI);
  Serial_cmd.addCommand("GetKI", getKI);
  Serial_cmd.addCommand("SetSK", setSK);
  Serial_cmd.addCommand("GetSK", getSK);
  //Serial_cmd.setDefaultHandler(unrecognized);
}

void unrecognized(const char *command)
{
  Serial.println("Invalid cmd");
}

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
