#include "ggwave.h"
#include "reed-solomon/rs.hpp"

#include <string.h>

#ifdef ARDUINO
#include <Arduino.h>
#include <avr/pgmspace.h>
#endif

// -----------------------------------------------------------------------------
// Protocoles
// -----------------------------------------------------------------------------
//
// framesPerByte = nombre de frames pendant lequel un ton est joué.
//
// Plus cette valeur est petite -> transmission plus rapide.
// -----------------------------------------------------------------------------

static const GGWave::Protocol PROTOCOL_SLOW = {
    9,
    24
};

static const GGWave::Protocol PROTOCOL_NORMAL = {
    6,
    24
};

static const GGWave::Protocol PROTOCOL_FAST = {
    4,
    24
};

static const GGWave::Protocol PROTOCOL_FASTEST = {
    3,
    24
};


// -----------------------------------------------------------------------------
// DSS
// -----------------------------------------------------------------------------

static const uint8_t DSS_TABLE[64] PROGMEM = {

    0x96, 0x9f, 0xb4, 0xaf,
    0x1b, 0x91, 0xde, 0xc5,
    0x45, 0x75, 0xe8, 0x2e,
    0x0f, 0x32, 0x4a, 0x5f,

    0xb4, 0x56, 0x95, 0xcb,
    0x7f, 0x6a, 0x54, 0x6a,
    0x48, 0xf2, 0x0b, 0x7b,
    0xcd, 0xfb, 0x93, 0x6d,

    0x3c, 0x77, 0x5e, 0xc3,
    0x33, 0x47, 0xc0, 0xf1,
    0x71, 0x32, 0x33, 0x27,
    0x35, 0x68, 0x47, 0x1f,

    0x4e, 0xac, 0x23, 0x42,
    0x5f, 0x00, 0x37, 0xa4,
    0x50, 0x6d, 0x48, 0x24,
    0x91, 0x7c, 0xa1, 0x4e
};


// -----------------------------------------------------------------------------
// Constructeur
// -----------------------------------------------------------------------------

GGWave::GGWave()
    : m_payloadLength(MAX_PAYLOAD),
      m_sampleRate(6000),
      m_samplesPerFrame(128),
      m_hzPerSample(0),
      m_toneCount(0),
      m_protocol(PROTOCOL_NORMAL) {
}


// -----------------------------------------------------------------------------
// Initialisation
// -----------------------------------------------------------------------------

bool GGWave::begin(
    const GGWaveParameters &parameters) {

    if (parameters.payloadLength == 0) {
        return false;
    }

    if (parameters.payloadLength > MAX_PAYLOAD) {
        return false;
    }

    if (parameters.sampleRate == 0) {
        return false;
    }

    if (parameters.samplesPerFrame == 0) {
        return false;
    }

    m_payloadLength   = parameters.payloadLength;
    m_sampleRate      = parameters.sampleRate;
    m_samplesPerFrame = parameters.samplesPerFrame;

    m_hzPerSample =
        (float)m_sampleRate /
        (float)m_samplesPerFrame;

    return true;
}


// -----------------------------------------------------------------------------
// Sélection de la vitesse
// -----------------------------------------------------------------------------

void GGWave::selectProtocol(
    GGWaveSpeed speed) {

    switch (speed) {

        case GGWAVE_SLOW:
            m_protocol = PROTOCOL_SLOW;
            break;

        case GGWAVE_NORMAL:
            m_protocol = PROTOCOL_NORMAL;
            break;

        case GGWAVE_FAST:
            m_protocol = PROTOCOL_FAST;
            break;

        case GGWAVE_FASTEST:
            m_protocol = PROTOCOL_FASTEST;
            break;

        default:
            m_protocol = PROTOCOL_NORMAL;
            break;
    }
}


// -----------------------------------------------------------------------------
// Encode
// -----------------------------------------------------------------------------

bool GGWave::encode(
    const char *text,
    GGWaveSpeed speed) {

    if (text == nullptr) {
        return false;
    }

    selectProtocol(speed);

    m_toneCount = 0;

    memset(m_payload, 0, sizeof(m_payload));
    memset(m_encoded, 0, sizeof(m_encoded));

    strncpy(
        (char *)m_payload,
        text,
        m_payloadLength
    );

    applyDSS();

    if (!encodeRS()) {
        return false;
    }

    encodeTones();

    return true;
}


// -----------------------------------------------------------------------------
// DSS
// -----------------------------------------------------------------------------

void GGWave::applyDSS() {

    for (uint8_t i = 0; i < m_payloadLength; ++i) {

#ifdef ARDUINO
        m_payload[i] ^=
            pgm_read_byte(
                &DSS_TABLE[i & 63]
            );
#else
        m_payload[i] ^=
            DSS_TABLE[i & 63];
#endif
    }
}


// -----------------------------------------------------------------------------
// Reed-Solomon
// -----------------------------------------------------------------------------

bool GGWave::encodeRS() {

    RS::ReedSolomon rs(
        m_payloadLength,
        ECC_BYTES
    );

    rs.Encode(
        m_payload,
        m_encoded
    );

    return true;
}


// -----------------------------------------------------------------------------
// Conversion RS -> tons
// -----------------------------------------------------------------------------

void GGWave::encodeTones() {

    for (uint8_t i = 0; i < CODEWORD; ++i) {

        const uint8_t value = m_encoded[i];

        // Nibble bas
        m_tones[m_toneCount++] =
            value & 0x0F;

        // Nibble haut
        m_tones[m_toneCount++] =
            (value >> 4) & 0x0F;
    }
}


// -----------------------------------------------------------------------------
// Durée d'un ton
// -----------------------------------------------------------------------------

uint16_t GGWave::toneDurationMs() const {

    return
        (uint32_t)m_protocol.framesPerByte *
        m_samplesPerFrame *
        1000UL /
        m_sampleRate;
}


// -----------------------------------------------------------------------------
// Accesseurs
// -----------------------------------------------------------------------------

const int8_t *GGWave::tones() const {
    return m_tones;
}


uint16_t GGWave::toneCount() const {
    return m_toneCount;
}


uint16_t GGWave::sampleRate() const {
    return m_sampleRate;
}


uint16_t GGWave::samplesPerFrame() const {
    return m_samplesPerFrame;
}


float GGWave::hzPerSample() const {
    return m_hzPerSample;
}


const GGWave::Protocol &GGWave::protocol() const {
    return m_protocol;
}

