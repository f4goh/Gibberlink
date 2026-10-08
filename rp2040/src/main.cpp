/* 
 * Gibberlink sur 40m
 * ajuster le vfo vers 7099Khz pour avoir la fréquence basse à 1125Hz
 * utiliser https://waver.ggerganov.com/ pour le décodage
 * use dss
 * fixed length 16
 * speed normal
 * le programme utilise la carte wspr-pico version 1 sur RP2040

 =====================================================
 SYNTHÈSE DES TONALITÉS POUR L'ENVOI DE "HELLO!"
=====================================================
Shift (curTone) | Fréquence (freq_hz) | Type
-----------------------------------------------------
0               | 1125.00 Hz          | Base (Min)
2               | 1218.75 Hz          | Standard
3               | 1265.63 Hz          | Standard
4               | 1312.50 Hz          | Standard
5               | 1359.38 Hz          | Standard
7               | 1453.13 Hz          | Standard
8               | 1500.00 Hz          | Standard
9               | 1546.88 Hz          | Standard
10              | 1593.75 Hz          | Standard
11              | 1640.63 Hz          | Standard
12              | 1687.50 Hz          | Standard
13              | 1734.38 Hz          | Standard
14              | 1781.25 Hz          | Standard
15              | 1828.13 Hz          | Max
-----------------------------------------------------
Total de tonalités émises pour ce message : 44
Pas entre chaque shift (hzPerSample)      : ~46.87 Hz
=====================================================
 * 
 */

#include <Arduino.h>
#include <Wire.h>
#include "LedCouleur.h"
#include "Dds.h"
#include <ggwave.h>


#define PTT_PIN 7
#define PIN_SPEAKER 8

GGWave ggwave;
LedCouleur led;
Dds *dds;

const GGWaveParameters PARAMETERS = {
    16, // payload
    6000, // sample rate
    128 // samples par frame
};


// -----------------------------------------------------------------------------
// Vitesse choisie ici
// -----------------------------------------------------------------------------

//const GGWaveSpeed SPEED = GGWAVE_FASTEST;
const GGWaveSpeed SPEED = GGWAVE_NORMAL;

volatile bool stateObjMod = false;

void setup(void) {
    set_sys_clock_khz(CLK_KHZ, true);
    Serial.begin(115200);

    Wire.setSDA(4);
    Wire.setSCL(5);

    Wire.begin();


    pinMode(PTT_PIN, OUTPUT);
    digitalWrite(PTT_PIN, LOW);
    pinMode(PIN_SPEAKER, OUTPUT);


    Serial1.setTX(0); // GP0 = TX vers GPS RX
    Serial1.setRX(1); // GP1 = RX depuis GPS TX
    Serial1.begin(9600); // Démarre UART1


    Wire.setClock(800000);

    led.begin();
    delay(1000);
    dds = new Dds();
    stateObjMod = true; //il faut que l'objet mod ait le temps de s'initialiser avant de lancer le dds dans le core 1
    dds->setFreqGG(7100000L);

    if (!ggwave.begin(PARAMETERS)) {
        Serial.println(F("Erreur initialisation GGWave"));
        while (true);
    }
    Serial.println(F("GGWave OK"));
}


// -----------------------------------------------------------------------------
// Envoi
// -----------------------------------------------------------------------------

// tone[i] pour Hello! 14 13 10 15 8 13 3 12 4 7 0 11 14 13 5 12 5 4 5 7 8 14 14 2 15 0 2 3 10 4 15 5 13 4 14 5 15 14 11 12 9 15 0 15 

void sendText(const char *text) {

    if (!ggwave.encode(text, SPEED)) {
        Serial.println(F("Erreur encodage"));
        return;
    }

    const int8_t *tones = ggwave.tones();

    const uint16_t count =
            ggwave.toneCount();

    const uint16_t duration =
            ggwave.toneDurationMs();

    const float hzPerSample =
            ggwave.hzPerSample();

    const uint16_t freqStart =
            ggwave.protocol().freqStart;


    Serial.print(F("Envoi : "));
    Serial.println(text);

    Serial.print(F("Tons : "));
    Serial.println(count);

    Serial.print(F("Duree ton : "));
    Serial.print(duration);
    Serial.println(F(" ms"));


    for (uint16_t i = 0; i < count; ++i) {

        const int8_t toneIndex = tones[i];

        const float frequency = (freqStart + toneIndex) * hzPerSample;
        Serial.println(tones[i]);

        tone(PIN_SPEAKER,(unsigned int)frequency);
        
        dds->setGGTone(tones[i]);
        
        delay(duration);
    }
    dds->stopTx();
    noTone(PIN_SPEAKER);
}

void loop() {
    led.rouge();
    sendText("Hello!");
    led.vert();
    delay(2000);
}

void setup1(void){
    while (!stateObjMod){}
    delay(1); //nécessaire pour que le dds démarre
    dds->coreUnSetup();
}

