#ifndef Definizioni
#define Definizioni

#include <Arduino.h>

//dichiarazione costanti
#define BottonModePin 2 //pin di ingresso del bottone per la selzione del modo delle ventole
#define FanInCtrlPin 5 //pin di uscita PWM per controllo velocità ventola in immisione d'aria
#define FanOutCtrlPin 6 //pin di uscita PWM per controllo velocità ventola in estrazione d'aria
#define AlarmPin 7 //pin di uscita rele per l'interruzione dell'alimentazione
#define PotSPPin A0 //pin del poteziometro per il settaggio del SP
#define VacPVPin A1 //Pin del sengnale della misura della tensine AC
#define AacPVPin A2 //Pin del sengnale della misura della corrente AC
#define SmokePVPin A3 //Pin del sengnale del sensore fumo
#define VacPVfactor 0.47659557F //fattore di conversione grandezza
#define AacPVfactor 0.002356949F //fattore di conversione grandezza
#define RMSfactor 0.707106781186547524400F //1 su radice di 2

//dichiarazione variabili globali
int PotSPValue = 0; // valore del potenziometro per il controllo del Set Point di temperatura
float VacPV = 0; //valore della misura della tensione AC
float AacPV = 0; //valore della misura della corrente AC
int VacPVhistory[10] = {0,0,0,0,0,0,0,0,0,0};
int AacPVhistory[10] = {0,0,0,0,0,0,0,0,0,0};
float ACinstantPWR = 0; //potenza istantanea AC
float ACenergy = 0; //energia asorbita AC
float TempSP = 20; //Set Point di temperatura
float TempPV = 30; //Process Value di temperatura
float PressPV = 0.0; //Process Value di pressione
float HumidityPV = 0.0; //Process Value di pressione
float kiFactor = 0.5F; //fattore integrativo del controllo
int SmokePV = 0; //Process Value di presenza fumo
int SmokeTH = 10000; //Soglia di allarme presenza fumo
int FanInCtrlValue = 0; //per uscita PWM
int FanOutCtrlValue = 0; //per uscita PWM
bool BottonMode = true; //bottone selezione modo ventole
bool Alarm = false; //stato di allarme grave



//prototipi di funzione
void getPV(void);
void alarmSequence(bool);
int controlIntegrator(float, float, float);
void printDisplayPV();
void printDisplayFAN(int);
void settingProcedure(void);
void EEPROMWriteFloat(int, float);
float EEPROMReadFloat(int);

#endif