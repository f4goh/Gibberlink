#include <ggwave.h>

const uint8_t PIN_LED     = 13;
const uint8_t PIN_SPEAKER = 10;

GGWave ggwave;


// -----------------------------------------------------------------------------
// Paramètres
// -----------------------------------------------------------------------------

const GGWaveParameters PARAMETERS = {
    16,     // payload
    6000,   // sample rate
    128     // samples par frame
};


// -----------------------------------------------------------------------------
// Vitesse choisie ici
// -----------------------------------------------------------------------------

//const GGWaveSpeed SPEED = GGWAVE_FASTEST;
const GGWaveSpeed SPEED = GGWAVE_NORMAL;

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

        const float frequency =
            (freqStart + toneIndex) *
            hzPerSample;

        tone(
            PIN_SPEAKER,
            (unsigned int)frequency
        );

        delay(duration);
    }

    noTone(PIN_SPEAKER);
    digitalWrite(PIN_SPEAKER, LOW);
}


// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup() {

    Serial.begin(57600);

    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_SPEAKER, OUTPUT);

    if (!ggwave.begin(PARAMETERS)) {

        Serial.println(
            F("Erreur initialisation GGWave")
        );

        while (true);
    }

    Serial.println(F("GGWave OK"));
}


// -----------------------------------------------------------------------------
// Loop
// -----------------------------------------------------------------------------

void loop() {

    digitalWrite(PIN_LED, HIGH);

    sendText("Hello!");

    digitalWrite(PIN_LED, LOW);

    delay(2000);
}

/*
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

Sending text: Hello!
14 13 10 15 8 13 3 12 4 7 0 11 14 13 5 12 5 4 5 7 8 14 14 2 15 0 2 3 10 4 15 5 13 4 14 5 15 14 11 12 9 15 0 15 


Sending text: Hello!
14  1781.25
13  1734.38
10  1593.75
15  1828.13
8  1500.00
13  1734.38
3  1265.63
12  1687.50
4  1312.50
7  1453.13
0  1125.00
11  1640.63
14  1781.25
13  1734.38
5  1359.38
12  1687.50
5  1359.38
4  1312.50
5  1359.38
7  1453.13
8  1500.00
14  1781.25
14  1781.25
2  1218.75
15  1828.13
0  1125.00
2  1218.75
3  1265.63
10  1593.75
4  1312.50
15  1828.13
5  1359.38
13  1734.38
4  1312.50
14  1781.25
5  1359.38
15  1828.13
14  1781.25
11  1640.63
12  1687.50
9  1546.88
15  1828.13
0  1125.00
15  1828.13

 */
