#ifndef GGWAVE_H
#define GGWAVE_H

#include <stdint.h>

// -----------------------------------------------------------------------------
// Protocoles
// -----------------------------------------------------------------------------

enum GGWaveSpeed : uint8_t {
    GGWAVE_SLOW = 0,
    GGWAVE_NORMAL,
    GGWAVE_FAST,
    GGWAVE_FASTEST
};

// -----------------------------------------------------------------------------
// Paramètres
// -----------------------------------------------------------------------------

struct GGWaveParameters {
    uint8_t  payloadLength;
    uint16_t sampleRate;
    uint16_t samplesPerFrame;
};

// -----------------------------------------------------------------------------
// GGWave
// -----------------------------------------------------------------------------

class GGWave {

public:

    static const uint8_t  MAX_PAYLOAD = 16;
    static const uint8_t  ECC_BYTES   = 6;
    static const uint8_t  CODEWORD    = MAX_PAYLOAD + ECC_BYTES;
    static const uint8_t  MAX_TONES   = CODEWORD * 2;

    struct Protocol {
        uint8_t  framesPerByte;
        uint16_t freqStart;
    };

public:

    GGWave();

    bool begin(const GGWaveParameters &parameters);

    bool encode(
        const char *text,
        GGWaveSpeed speed
    );

    const int8_t *tones() const;

    uint16_t toneCount() const;

    uint16_t toneDurationMs() const;

    uint16_t sampleRate() const;

    uint16_t samplesPerFrame() const;

    float hzPerSample() const;

    const Protocol &protocol() const;

private:

    void applyDSS();

    bool encodeRS();

    void encodeTones();

    void selectProtocol(GGWaveSpeed speed);

    static uint8_t dss(uint8_t index);

private:

    uint8_t m_payload[MAX_PAYLOAD];
    uint8_t m_encoded[CODEWORD];
    int8_t  m_tones[MAX_TONES];

    uint8_t  m_payloadLength;
    uint16_t m_sampleRate;
    uint16_t m_samplesPerFrame;

    float m_hzPerSample;

    uint16_t m_toneCount;

    Protocol m_protocol;
};

#endif
