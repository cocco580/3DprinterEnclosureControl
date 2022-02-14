#ifndef config_hpp
#define config_hpp

#include <Arduino.h>


//dichiarazione costanti
#define BUTTON_MODE_PIN 2 //pin di ingresso del bottone per la selzione del modo delle ventole
#define FAN_IN_CTRL_PIN 5 //pin di uscita PWM per controllo velocità ventola in immisione d'aria
#define FAN_OUT_CTRL_PIN 6 //pin di uscita PWM per controllo velocità ventola in estrazione d'aria
#define ALARM_PIN 7 //pin di uscita rele per l'interruzione dell'alimentazione
#define POT_PIN A0 //pin del poteziometro per il settaggio del SP
#define AC_VOLTAGE_PIN A1 //Pin del sengnale della misura della tensine AC
#define AC_CURRENT_PIN A2 //Pin del sengnale della misura della corrente AC - 2.8A valore massimo misurabile con gain al massimo
#define AC_VOLTAGE_AC_COUPLING -523
#define AC_CURRENT_AC_COUPLING -518
#define AC_VOLTAGE_FACTOR 1.6F
#define AC_VOLTAGE_OFFSET 18.8F
#define AC_CURRENT_FACTOR 0.008057554F
#define AC_CURRENT_OFFSET 0.003022F
#define SMOKE_PIN A3 //Pin del sengnale del sensore fumo
#define PWR_LOG_PERIOD 40 // Tempo di misura di tensione e corrente in millisecondi
#define PID_PERIOD 2000UL // periodo di aggiornamento del PID in millisecondi
#define BUTTON_DEBOUNCE_TIME 1000 //debouncing time del pulsante di cambio stato

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
//float PressPV = 0.0; //Process Value di pressione
float HumidityPV = 0.0; //Process Value di pressione
float kiFactor = 0.5F; //fattore integrativo del controllo
int SmokePV = 0; //Process Value di presenza fumo
int SmokeUpperLimit = 10000; //Soglia di allarme presenza fumo
int FanCtrlValue = 0; //per uscita PWM
bool ButtonMode = true; //bottone selezione modo ventole
bool Alarm = false; //stato di allarme grave
static unsigned long PreviousPIDTime = 0; //timestamp per creare un evento periodico necessario al PID

//emun
enum StateMachine{
    AUTOMATICO,
    MANUALE,
    STOP,
};
StateMachine Status = AUTOMATICO;

//prototipi di funzione
void getPV(void);
void alarmSequence(bool);
int controlIntegrator(float, float, float);
void settingProcedure(void);
void EEPROMWriteFloat(int, float);
float EEPROMReadFloat(int);
void ISRbuttomMode(void);
//void add_serial_commands(void);
//void unrecognized(const char *command);
//void setKI(void);
//void getKI(void);
//void setSK(void);
//void getSK(void);

/*
void beginDisplayPV(void);
void printDisplayPV(float, float, int, float, float, float, float);
void beginDisplayFAN(void);
void printDisplayFAN_AUTO(float, int);
void printDisplayFAN_MAN(int);
void printDisplayFAN_STOP(void);
*/
#endif