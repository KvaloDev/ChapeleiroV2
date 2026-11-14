#include "bracosSetup.h"
#include "config.h"

volatile long ticksOmbroEsq = 0;
volatile long ticksOmbroDir = 0;

constexpr int8_t kTabelaQuad[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
static volatile uint8_t estEsq = 0, estDir = 0;
static volatile unsigned long ultMsEsq = 0, ultMsDir = 0;

void IRAM_ATTR atualizarQuadBraco(uint8_t pinA, uint8_t pinB, volatile uint8_t& est, volatile unsigned long& ult, volatile long& ticks, int dir) {
    unsigned long agora = micros();
    if (agora - ult < 50) return;
    uint8_t atual = (digitalRead(pinA) << 1) | digitalRead(pinB);
    ticks += kTabelaQuad[(est << 2) | atual] * dir;
    est = atual;
    ult = agora;
}

void IRAM_ATTR isrOmbroEsq() { atualizarQuadBraco(kPinEncOmbroEsqA, kPinEncOmbroEsqB, estEsq, ultMsEsq, ticksOmbroEsq, kDirEncOmbroEsq); }
void IRAM_ATTR isrOmbroDir() { atualizarQuadBraco(kPinEncOmbroDirA, kPinEncOmbroDirB, estDir, ultMsDir, ticksOmbroDir, kDirEncOmbroDir); }

void BracosSetup::inicializarHardware() {
    Serial2.begin(kBaudSerial2, SERIAL_8N1, kPinRxSerial2, kPinTxSerial2);

    // TODO: Habilitar pinMode e attachInterrupt após preencher os pinos reais em config.h.
    if (kPinOmbroEsqRpwm != 255) {
        pinMode(kPinOmbroEsqRpwm, OUTPUT); pinMode(kPinOmbroEsqLpwm, OUTPUT);
        pinMode(kPinOmbroDirRpwm, OUTPUT); pinMode(kPinOmbroDirLpwm, OUTPUT);

        pinMode(kPinEncOmbroEsqA, INPUT_PULLUP); pinMode(kPinEncOmbroEsqB, INPUT_PULLUP);
        pinMode(kPinEncOmbroDirA, INPUT_PULLUP); pinMode(kPinEncOmbroDirB, INPUT_PULLUP);

        attachInterrupt(digitalPinToInterrupt(kPinEncOmbroEsqA), isrOmbroEsq, CHANGE);
        attachInterrupt(digitalPinToInterrupt(kPinEncOmbroEsqB), isrOmbroEsq, CHANGE);
        attachInterrupt(digitalPinToInterrupt(kPinEncOmbroDirA), isrOmbroDir, CHANGE);
        attachInterrupt(digitalPinToInterrupt(kPinEncOmbroDirB), isrOmbroDir, CHANGE);
    }
}