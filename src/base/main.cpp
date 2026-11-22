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
    }
    // Quando kModoDesenvolvimento = false, o Wi-Fi fica desligado e o robô aguarda a seta direita (>) do SPIKE.
}

void loop() {
    funcoes.processarSerial2();
    cenas.atualizarCenas();
    funcoes.atualizarCinematica();

    if (kModoDesenvolvimento) {
        setupBase.atualizarRede(funcoes);
    }
}