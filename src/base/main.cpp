#include <Arduino.h>
#include "config.h"
#include "baseSetup.h"
#include "baseFuncoes.h"
#include "baseCenas.h"

BaseSetup setupBase;
BaseFuncoes funcoes;
BaseCenas cenas(funcoes);

void setup() {
    Serial.begin(115200);
    setupBase.inicializarHardware();

    if (kModoDesenvolvimento) {
        setupBase.inicializarRedeEInterface(funcoes, cenas);
    } else {
        cenas.iniciarCena(1);
    }

    setupBase.inicializarBleSpike();
}

void loop() {
    funcoes.atualizarCinematica();
    funcoes.processarSerial2();
    cenas.atualizarCenas();

    if (kModoDesenvolvimento) {
        setupBase.atualizarRede(funcoes);
    }
}