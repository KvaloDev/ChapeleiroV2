#include <Arduino.h>
#include "bracosSetup.h"
#include "bracosFuncoes.h"
#include "bracosCenas.h"

BracosSetup setupBracos;
BracosFuncoes funcoes;
BracosCenas cenas(funcoes);

void setup() {
    Serial.begin(115200);
    setupBracos.inicializarHardware();
    funcoes.inicializarServos();
}

void loop() {
    funcoes.processarSerial2(cenas);
    funcoes.atualizarCinematica();
    cenas.atualizarCenas();
    funcoes.enviarTelemetriaSerial2();
}